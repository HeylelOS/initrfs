#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cpio.h>

struct mkinitrfs_args {
	const char *init;
};

struct cpio_newc_header {
	uint32_t ino;
	uint32_t mode;
	uint32_t uid;
	uint32_t gid;
	uint32_t nlink;
	uint32_t mtime;
	uint32_t filesize;
	uint32_t devmajor;
	uint32_t devminor;
	uint32_t rdevmajor;
	uint32_t rdevminor;
	uint32_t check;
};

static int
mkinitrfs_output_entry(FILE *output, const struct cpio_newc_header *header, const char *pathname) {
	long position;

	while(position = ftell(output), position != -1 && !!(position & 0x3)) {
		if(fputc('\0', output) == EOF) {
			return -1;
		}
	}

	if(position == -1) {
		return -1;
	}

	const size_t namesize = strlen(pathname) + 1;
	if(namesize > UINT32_MAX) {
		errno = ENAMETOOLONG;
		return -1;
	}

	int headerlen = fprintf(output, "070701%.8X%.8X%.8X%.8X%.8X%.8X%.8X%.8X%.8X%.8X%.8X%.8X%.8X%s",
		header->ino, header->mode, header->uid, header->gid, header->nlink, header->mtime, header->filesize,
		header->devmajor, header->devminor, header->rdevmajor, header->rdevminor, (uint32_t)namesize, header->check, pathname);

	if(headerlen < 0) {
		return -1;
	}

	do {
		if(fputc('\0', output) == EOF) {
			return -1;
		}
		headerlen++;
	} while(!!(headerlen & 0x3));

	return 0;
}

static int
mkinitrfs_output_file(FILE *output, int fd, uint32_t mode, const char *filename) {
	struct stat st;

	if(fstat(fd, &st) != 0) {
		return -1;
	}

	if(st.st_size > UINT32_MAX) {
		errno = EFBIG;
		return -1;
	}

	const struct cpio_newc_header header = {
		.mode = mode,
		.nlink = 1,
		.mtime = st.st_mtim.tv_sec,
		.filesize = st.st_size,
	};

	if(mkinitrfs_output_entry(output, &header, filename) != 0) {
		return -1;
	}

	ssize_t readval;
	char buffer[st.st_blksize];
	while(readval = read(fd, buffer, sizeof(buffer)), readval > 0) {
		if(fwrite(buffer, 1, readval, output) != readval) {
			return -1;
		}
	}

	if(readval != 0) {
		return -1;
	}

	return 0;
}

static int
mkinitrfs_output_trailer(FILE *output) {
	const struct cpio_newc_header header = { .nlink = 1 };

	if(mkinitrfs_output_entry(output, &header, "TRAILER!!!") != 0) {
		return -1;
	}

	return 0;
}

static int
mkinitrfs_output(FILE *output, const struct mkinitrfs_args *args) {
	int fd = open(args->init, O_RDONLY);
	int retval = -1;

	if(fd >= 0) {

		if(mkinitrfs_output_file(output, fd, C_ISREG | 0744, "init") == 0
			&& mkinitrfs_output_trailer(output) == 0) {
			retval = 0;
		}

		close(fd);
	}

	return retval;
}

static void
mkinitrfs_usage(const char *mkinitrfsname) {
	fprintf(stderr, "usage: %s [-I <init>] [<output>]\n", mkinitrfsname);
	exit(EXIT_FAILURE);
}

static struct mkinitrfs_args
mkinitrfs_parse_args(int argc, char **argv) {
	struct mkinitrfs_args args = {
		.init = "init",
	};
	int c;

	while((c = getopt(argc, argv, ":I:")) != -1) {
		switch(c) {
		case 'I':
			args.init = optarg;
			break;
		case '?':
			fprintf(stderr, "%s: Invalid option -%c\n", *argv, optopt);
			mkinitrfs_usage(*argv);
		case ':':
			fprintf(stderr, "%s: Missing option argument after -%c\n", *argv, optopt);
			mkinitrfs_usage(*argv);
		}
	}

	const int outputs = argc - optind;

	if(outputs > 1) {
		fprintf(stderr, "%s: Expected one or zero output file, found %d\n", *argv, outputs);
		mkinitrfs_usage(*argv);
	}

	if(outputs == 0 && isatty(STDOUT_FILENO)) {
		fprintf(stderr, "%s: Cannot output initrfs on a terminal!\n", *argv);
		exit(EXIT_FAILURE);
	}

	return args;
}

int
main(int argc, char **argv) {
	const struct mkinitrfs_args args = mkinitrfs_parse_args(argc, argv);
	char ** const argpos = argv + optind, ** const argend = argv + argc;
	int retval = EXIT_SUCCESS;

	if(argpos != argend) {
		const char *output = *argpos;
		FILE *filep = fopen(output, "w");

		if(filep != NULL) {
			if(mkinitrfs_output(filep, &args) != 0) {
				fprintf(stderr, "%s: Unable to write output in %s for %s: %s\n", *argv, output, args.init, strerror(errno));
				retval = EXIT_FAILURE;
				remove(output);
			}
		} else {
			fprintf(stderr, "%s: Unable to open output %s: %s\n", *argv, output, strerror(errno));
			retval = EXIT_FAILURE;
		}

		fclose(filep);
	} else {
		if(mkinitrfs_output(stdout, &args) != 0) {
			fprintf(stderr, "%s: Unable to write output on stdout for %s: %s\n", *argv, args.init, strerror(errno));
			retval = EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

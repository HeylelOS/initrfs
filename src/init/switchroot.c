#include "switchroot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

static void
obliterate_directory(int fd) {
	DIR *dirp = fdopendir(fd);
	struct dirent *entry;

	if(dirp == NULL) {
		fprintf(stderr, "initrfs: fdopendir: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while(errno = 0, entry = readdir(dirp), entry != NULL) {
		if(strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0) {
			int flags = 0;

			if(entry->d_type == DT_DIR) {
				int entryfd = openat(dirfd(dirp), entry->d_name, O_RDONLY);

				if(entryfd < 0) {
					fprintf(stderr, "initrfs: openat: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}

				obliterate_directory(entryfd);

				flags = AT_REMOVEDIR;
			}

			if(unlinkat(dirfd(dirp), entry->d_name, flags) != 0) {
				fprintf(stderr, "initrfs: unlinkat: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}

	if(errno != 0) {
		fprintf(stderr, "initrfs: readdir: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(closedir(dirp) != 0) {
		fprintf(stderr, "initrfs: closedir: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void
switch_root(const char *rootmnt) {
	int oldrootfd = open("/", O_RDONLY);

	if(oldrootfd == -1) {
		fprintf(stderr, "initrfs: Unable to open /");
		exit(EXIT_FAILURE);
	}

	if(chdir(rootmnt) == -1) {
		fprintf(stderr, "initrfs: Unable to chdir to %s: %s\n", rootmnt, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(mount(rootmnt, "/", NULL, MS_MOVE, NULL) == -1) {
		fprintf(stderr, "initrfs: Unable to move %s to /: %s\n", rootmnt, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(chroot(".") == -1) {
		fprintf(stderr, "initrfs: Unable to chroot to new root: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	obliterate_directory(oldrootfd);
}

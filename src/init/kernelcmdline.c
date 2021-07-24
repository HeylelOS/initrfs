#include "kernelcmdline.h"
#include "trimstr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <errno.h>

static void
kernel_cmdline_parse_option_value(struct kernel_cmdline *cmdline, const char *option, const char *value) {

	if(strcmp("root", option) == 0) {
		cmdline->root = strdup(value);
		return;
	}

	if(strcmp("rootfstype", option) == 0) {
		cmdline->rootfstype = strdup(value);
		return;
	}

#if 0
	if(strcmp("rootflags", option) == 0) {
		cmdline->rootflags = mount_parse_flags(value);
		return;
	}
#endif
}

static void
kernel_cmdline_parse_option(struct kernel_cmdline *cmdline, const char *option) {
}

void
kernel_cmdline_parse(struct kernel_cmdline *cmdline) {
	const char filename[] = "/proc/cmdline";
	FILE *filep = fopen(filename, "r");

	if(filep == NULL) {
		fprintf(stderr, "initrfs: Unable to open kernel cmdline '%s': %s\n", filename, strerror(errno));
		exit(EXIT_FAILURE);
	}

	ssize_t length;
	char *line = NULL;
	size_t capacity = 0;

	while(length = getdelim(&line, &capacity, ' ', filep), length != -1) {
		char *value = trimstr(line, length);
		char * const option = strsep(&value, "=");

		if(*option != '\0') {
			if(value != NULL) {
				kernel_cmdline_parse_option_value(cmdline, option, value);
			} else {
				kernel_cmdline_parse_option(cmdline, option);
			}
		}
	}

	fclose(filep);
}

void
kernel_cmdline_mount_root(const struct kernel_cmdline *cmdline, const char *rootmnt) {

	if(cmdline->root == NULL) {
		fprintf(stderr, "initrfs: Missing root option from kernel\n");
		exit(EXIT_FAILURE);
	}

	if(cmdline->rootfstype == NULL) {
		fprintf(stderr, "initrfs: Missing rootfstype option from kernel\n");
		exit(EXIT_FAILURE);
	}

	if(mount(cmdline->root, rootmnt, cmdline->rootfstype, cmdline->rootflags, NULL) != 0) {
		fprintf(stderr, "initrfs: Unable to mount '%s' (%s) to '%s': %s\n", cmdline->root, cmdline->rootfstype, rootmnt, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

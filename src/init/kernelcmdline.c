/* SPDX-License-Identifier: BSD-3-Clause */
#include "kernelcmdline.h"
#include "mountutils.h"
#include "trimstr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>
#include <err.h>

static char *
kernel_cmdline_init(const char *init) {
	char * const copy = strdup(init);

	if (copy == NULL) {
		errx(1, "init path '%s' is too long", init);
	}

	return copy;
}

static void
kernel_cmdline_parse_option_value(struct kernel_cmdline *cmdline, const char *option, const char *value) {

	if (strcmp("init", option) == 0) {
		free(cmdline->init);
		cmdline->init = kernel_cmdline_init(value);
		return;
	}

	if (strcmp("root", option) == 0) {
		free(cmdline->root);
		cmdline->root = mount_resolve_device(value);
		return;
	}

	if (strcmp("rootfstype", option) == 0) {
		free(cmdline->rootfstype);
		cmdline->rootfstype = mount_resolve_fstype(value);
		return;
	}

	if (strcmp("rootflags", option) == 0) {
		free(cmdline->rootdata);
		cmdline->rootflags = mount_resolve_options(value, &cmdline->rootdata);
		return;
	}

	if (strcmp("rootdelay", option) == 0) {
		cmdline->rootdelay = strtoul(value, NULL, 0);
		return;
	}
}

static void
kernel_cmdline_parse_option(struct kernel_cmdline *cmdline, const char *option) {

	if (strcmp("rw", option) == 0) {
		cmdline->rootflags &= ~MS_RDONLY;
	}

	if (strcmp("ro", option) == 0) {
		cmdline->rootflags |= MS_RDONLY;
	}
}

void
kernel_cmdline_parse(struct kernel_cmdline *cmdline) {
	static const char filename[] = "/proc/cmdline";
	FILE * const filep = fopen(filename, "r");

	if (filep == NULL) {
		err(1, "Unable to open kernel cmdline '%s'", filename);
	}

	ssize_t length;
	char *line = NULL;
	size_t capacity = 0;

	while (length = getdelim(&line, &capacity, ' ', filep), length != -1) {
		char *value = trimstr(line, length);
		char * const option = strsep(&value, "=");

		if (*option != '\0') {
			if (value != NULL) {
				kernel_cmdline_parse_option_value(cmdline, option, value);
			} else {
				kernel_cmdline_parse_option(cmdline, option);
			}
		}
	}

	free(line);
	fclose(filep);
}

void
kernel_cmdline_mount_root(const struct kernel_cmdline *cmdline, const char *rootmnt) {

	if (cmdline->root == NULL) {
		errx(1, "Missing root option from kernel");
	}

	if (cmdline->rootfstype == NULL) {
		errx(1, "Missing rootfstype option from kernel");
	}

	if (cmdline->rootdelay != 0) {
		unsigned int left = cmdline->rootdelay;
		while (left = sleep(left), left != 0);
	}

	if (mount(cmdline->root, rootmnt, cmdline->rootfstype, cmdline->rootflags, cmdline->rootdata) != 0) {
		err(1, "Unable to mount '%s' (%s) to '%s'", cmdline->root, cmdline->rootfstype, rootmnt);
	}
}

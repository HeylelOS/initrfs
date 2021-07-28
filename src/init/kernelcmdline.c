#include "kernelcmdline.h"
#include "mountutils.h"
#include "trimstr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>
#include <err.h>

#define INTERNAL_BUFFER_CAPACITY_INIT 128
#define INTERNAL_BUFFER_CAPACITY_ROOTDATA 128

const char *
kernel_cmdline_init(const char *init) {
	static char buffer[INTERNAL_BUFFER_CAPACITY_INIT];

	strncpy(buffer, init, sizeof(buffer));

	if(buffer[sizeof(buffer) - 1] != '\0') {
		errx(1, "init path '%s' is too long", init);
	}

	return buffer;
}

static void
kernel_cmdline_parse_option_value(struct kernel_cmdline *cmdline, const char *option, const char *value) {

	if(strcmp("init", option) == 0) {
		cmdline->init = kernel_cmdline_init(value);
		return;
	}

	if(strcmp("root", option) == 0) {
		cmdline->root = mount_resolve_device(value);
		return;
	}

	if(strcmp("rootfstype", option) == 0) {
		cmdline->rootfstype = mount_resolve_fstype(value);
		return;
	}

	if(strcmp("rootflags", option) == 0) {
		static char rootdata[INTERNAL_BUFFER_CAPACITY_ROOTDATA];
		cmdline->rootflags = mount_resolve_options(value, rootdata, sizeof(rootdata));
		cmdline->rootdata = rootdata;
		return;
	}

	if(strcmp("rootdelay", option) == 0) {
		cmdline->rootdelay = strtoul(value, NULL, 0);
		return;
	}
}

static void
kernel_cmdline_parse_option(struct kernel_cmdline *cmdline, const char *option) {
}

void
kernel_cmdline_parse(struct kernel_cmdline *cmdline) {
	const char filename[] = "/proc/cmdline";
	FILE *filep = fopen(filename, "r");

	if(filep == NULL) {
		err(1, "Unable to open kernel cmdline '%s'", filename);
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

	free(line);
	fclose(filep);
}

void
kernel_cmdline_mount_root(const struct kernel_cmdline *cmdline, const char *rootmnt) {

	if(cmdline->root == NULL) {
		errx(1, "Missing root option from kernel");
	}

	if(cmdline->rootfstype == NULL) {
		errx(1, "Missing rootfstype option from kernel");
	}

	if(cmdline->rootdelay != 0) {
		unsigned int left = cmdline->rootdelay;
		while(left = sleep(left), left != 0);
	}

	if(mount(cmdline->root, rootmnt, cmdline->rootfstype, cmdline->rootflags, cmdline->rootdata) != 0) {
		err(1, "Unable to mount '%s' (%s) to '%s'", cmdline->root, cmdline->rootfstype, rootmnt);
	}
}

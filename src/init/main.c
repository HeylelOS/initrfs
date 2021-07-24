#include "kernelcmdline.h"
#include "configuration.h"
#include "switchroot.h"
#include "mountutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static void
configure_system(const char *configsys, const char **initp) {
	/* TODO */
}

static void _Noreturn
init_system(const char *init) {
	char * const argv[] = {
		"init", NULL
	};

	if(execv(init, argv) != 0) {
		fprintf(stderr, "initrfs: Unable to execv %s: %s\n", init, strerror(errno));
	}

	exit(EXIT_FAILURE);
}

int
main(void) {
	static const struct mount_description support[] = {
		{ "", "/dev", "devtmpfs", NULL, 0 },
		{ "", "/proc", "proc", NULL, 0 },
		{ },
	};
	static const char rootmnt[] = "/mnt";
	static const char configsys[] = "/boot/config.sys";
	struct kernel_cmdline cmdline = { };
	const char *init = "/sbin/init";

	mount_filesystems(support);

	kernel_cmdline_parse(&cmdline);
	kernel_cmdline_mount_root(&cmdline, rootmnt);

	unmount_filesystems(support);

	switch_root(rootmnt);

	configure_system(configsys, &init);

	init_system(init);

	/* Never reached */
}


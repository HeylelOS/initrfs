#include "kernelcmdline.h"
#include "configuration.h"
#include "switchroot.h"
#include "mountutils.h"

#include <stdnoreturn.h>
#include <unistd.h>
#include <err.h>

static void noreturn
init_system(const char *init) {
	char * const argv[] = {
		"init", NULL
	};

	execv(init, argv);
	err(1, "Unable to execv %s", init);
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
	struct kernel_cmdline cmdline = {
		.init = "/sbin/init",
	};

	mount_filesystems(support);

	kernel_cmdline_parse(&cmdline);
	kernel_cmdline_mount_root(&cmdline, rootmnt);

	unmount_filesystems(support);

	switch_root(rootmnt);

	configure_system(configsys, &cmdline.init);

	init_system(cmdline.init);

	/* Never reached */
}


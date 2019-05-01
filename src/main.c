#include "early_io.h"
#include "partition.h"

#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <sys/mount.h>
#include <fstab.h>

#include <stdio.h>
#include <sys/stat.h>

int
main(int argc,
	char * const argv[]) {
	struct partition_device root = { .major = 8, .minor = 0 };

	/* Find block device associated to root physical device */

	if(mount("", "/proc", "proc", 0, NULL) == -1) {
		early_abort("Unable to mount temporary /proc.", NULL);
	}

	struct partition *partition;
	if(partition_list_create(&partition) == -1) {
		early_abort("Unable to fetch partitions list.", NULL);
	}

	struct partition *iterator = partition;
	while(iterator != NULL
		&& (iterator->device.major != root.major
			|| iterator->device.minor != root.minor)) {
		iterator = iterator->next;
	}

	if(iterator != NULL) {
		early_print("Early userspace matched %d:%d with %s (%lu blocks)\n",
			iterator->device.major, iterator->device.minor,
			iterator->name, iterator->blocks);
	} else {
		early_abort("Unable to find / device.",
			"Expected device %d:%d", root.major, root.minor);
	}

	/* Now that we found the device, we access /dev and mount it */

	if(mount("", "/dev", "devtmpfs", 0, NULL) == -1) {
		early_abort("Unable to mount temporary /dev.", NULL);
	}

	if(mount("/dev/sda", "/mnt", "ext4", 0, NULL) == -1) {
		early_abort("Unable to mount root.", NULL);
	}

	/* TODO: switch_root */

	char * const arguments[] = {
		"init",
		NULL
	};
	if(execv("/sbin/init", arguments) == -1) {
		early_abort("Unable to execute init.", NULL);
	}

	/* Never reached */
	return 0;
}


#include "early_io.h"
#include "partition.h"

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/mount.h>

struct root_candidate {
	struct partition partition;
	char fstype[32];
};

static void
find_root_candidate(struct root_candidate *candidate) {
	struct partition *partition, *iterator;

	/* Find block device associated to root physical device */

	if(mount("", "/proc", "proc", 0, NULL) == -1) {
		early_abort("Unable to mount temporary /proc.", NULL);
	}

	if(partition_list_create(&partition) == -1) {
		early_abort("Unable to fetch partitions list.", NULL);
	}

	iterator = partition;
	while(iterator != NULL
		&& (iterator->device.major != candidate->partition.device.major
			|| iterator->device.minor != candidate->partition.device.minor)) {
		iterator = iterator->next;
	}

	if(iterator != NULL) {
		early_print("Early userspace matched %d:%d with %s (%lu blocks)\n",
			iterator->device.major, iterator->device.minor,
			iterator->name, iterator->blocks);

		candidate->partition = *iterator;
	} else {
		early_abort("Unable to find / device.",
			"Expected device %d:%d",
			candidate->partition.device.major,
			candidate->partition.device.minor);
	}

	partition_list_destroy(partition);
}

static void
mount_root_candidate(struct root_candidate *candidate) {
	char path[PATH_MAX];

	/* Now that we found the device, we access /dev and mount it */

	if(mount("", "/dev", "devtmpfs", 0, NULL) == -1) {
		early_abort("Unable to mount temporary /dev.", NULL);
	}

	snprintf(path, sizeof(path), "/dev/%s", candidate->partition.name);
	if(mount(path, "/mnt", candidate->fstype, 0, NULL) == -1) {
		early_abort("Unable to mount root.", NULL);
	}
}

int
main(int argc,
	char * const argv[]) {
	struct root_candidate candidate = {
		.partition = {
			.device = {
				.major = 8,
				.minor = 0
			}
		},
		.fstype = "ext4"
	};

	find_root_candidate(&candidate);

	mount_root_candidate(&candidate);

	/* TODO: switch_root(); */

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


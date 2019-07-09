#include "candidate.h"
#include "filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <limits.h>
#include <err.h>

struct mountargs {
	const char *source;
	const char *target;
	const char *fstype;
	unsigned long mountflags;
	const void *data;
};

int
candidate_list_create(struct candidate **listp) {
	FILE *candidatesfile = fopen("/candidates", "r");

	if(candidatesfile == NULL) {
		warn("Unable to open candidates list file\n");
		return -1;
	}

	int scanval;
	struct candidate candidate;
	struct candidate *previous = &candidate;

	while((scanval = fscanf(candidatesfile, "%31s %15s %31s %31s %31s",
		candidate.device, candidate.fstype, candidate.options, candidate.recover, candidate.fstab)) != EOF) {

		if(scanval == 5) {
			struct candidate *next = malloc(sizeof(candidate));
			*next = candidate;
			previous->next = next;
			previous = next;
		}
	}

	if(ferror(candidatesfile) != 0) {
		warn("Error when fetching a candidate");
	}

	previous->next = NULL;
	*listp = candidate.next;

	return 0;
}

void
candidate_list_destroy(struct candidate *list) {

	while(list != NULL) {
		struct candidate *next = list->next;
		free(list);
		list = next;
	}
}

int
candidate_prepare(struct candidate *candidate) {
	static const struct mountargs mounts[] = {
		{ "", "/mnt/proc", "proc", 0, NULL },
		{ "", "/mnt/sys", "sysfs", 0, NULL },
		{ "", "/mnt/var", "tmpfs", 0, NULL },
		{ "", "/mnt/tmp", "tmpfs", 0, NULL },
	};
	char source[PATH_MAX];
	snprintf(source, sizeof(source), "/dev/%s", candidate->device);

	/* Mounting candidate */
	if(mount(source, "/mnt", candidate->fstype,
		filesystem_mount_flags(candidate->options), NULL) == -1) {
		warn("Unable to mount candidate %s as %s at /mnt", candidate->device, candidate->fstype);
		goto candidate_prepare_err0;
	}

	/* Moving /dev */
	if(mount("/dev", "/mnt/dev", NULL, MS_MOVE, NULL) == -1) {
		warn("Unable to mount /dev for candidate %s", candidate->device);
		goto candidate_prepare_err1;
	}

	const struct mountargs *iterator = mounts, * const mountsend = mounts + sizeof(mounts) / sizeof(*mounts);
	while(iterator != mountsend
		&& mount(iterator->source, iterator->target, iterator->fstype,
			iterator->mountflags, iterator->data) == 0) {
		iterator++;
	}

	if(iterator != mountsend) {
		warn("Unable to mount %s for candidate %s", iterator->target, candidate->device);
		goto candidate_prepare_err2;
	}

	/* Better move before next mount */
	if(chdir("/mnt") == -1) {
		warn("Unable to chdir to /mnt for candidate %s", candidate->device);
		goto candidate_prepare_err2;
	}

	return 0;
candidate_prepare_err2:
	while(--iterator > mounts) {
		if(umount(iterator->target) == -1) {
			warn("Unable to reset %s to %s for %s",
				iterator->target, iterator->source, candidate->device);
		}
	}

	if(mount("/mnt/dev", "/dev", NULL, MS_MOVE, NULL) == -1) {
		warn("Unable to reset /mnt/dev to /dev for %s", candidate->device);
	}

candidate_prepare_err1:
	if(umount("/mnt") == -1) {
		warn("Unable to unmount failing candidate %s", candidate->device);
	}

candidate_prepare_err0:
	return -1;
}


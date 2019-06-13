#include "candidate.h"
#include "filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <limits.h>
#include <err.h>

int
candidate_list_create(struct candidate **listp) {
	FILE *candidatesfile = fopen("/candidates", "r");

	if(candidatesfile == NULL) {
		warn("Unable to open candidates list file\n");
		return -1;
	}

	int scanval;
	struct candidate candidate;
	memset(&candidate, 0, sizeof(candidate));
	*listp = NULL;
	while((scanval = fscanf(candidatesfile, "%31s %15s %31s %31s %31s",
		candidate.device, candidate.fstype, candidate.options, candidate.recover, candidate.fstab)) != EOF) {

		if(scanval == 5) {
			candidate.next = *listp;
			*listp = malloc(sizeof(**listp));
			**listp = candidate;
		}
	}

	if(ferror(candidatesfile) != 0) {
		warn("Error when fetching a candidate");
	}

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

	/* Mounting /proc */
	if(mount("", "/mnt/proc", "proc", 0, NULL) == -1) {
		warn("Unable to mount /proc for candidate %s", candidate->device);
		goto candidate_prepare_err2;
	}

	/* Mounting /sys */
	if(mount("", "/mnt/sys", "sysfs", 0, NULL) == -1) {
		warn("Unable to mount /sys for candidate %s", candidate->device);
		goto candidate_prepare_err3;
	}

	/* Mounting /run */
	if(mount("", "/mnt/run", "tmpfs", 0, NULL) == -1) {
		warn("Unable to mount /run for candidate %s", candidate->device);
		goto candidate_prepare_err4;
	}

	/* Mounting /tmp */
	if(mount("", "/mnt/tmp", "tmpfs", 0, NULL) == -1) {
		warn("Unable to mount /tmp for candidate %s", candidate->device);
		goto candidate_prepare_err5;
	}

	/* Better move before next mount */
	if(chdir("/mnt") == -1) {
		warn("Unable to chdir to /mnt for candidate %s", candidate->device);
		goto candidate_prepare_err6;
	}

	return 0;
candidate_prepare_err6:
	if(umount("/mnt/tmp") == -1) {
		warn("Unable to unmount /tmp for failing candidate %s", candidate->device);
	}

candidate_prepare_err5:
	if(umount("/mnt/run") == -1) {
		warn("Unable to unmount /run for failing candidate %s", candidate->device);
	}

candidate_prepare_err4:
	if(umount("/mnt/sys") == -1) {
		warn("Unable to unmount /sys for failing candidate %s", candidate->device);
	}

candidate_prepare_err3:
	if(umount("/mnt/proc") == -1) {
		warn("Unable to unmount /proc for failing candidate %s", candidate->device);
	}

candidate_prepare_err2:
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


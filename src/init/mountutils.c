#include "mountutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <errno.h>

void
mount_filesystems(const struct mount_description *description) {

	while(description->source != NULL && mount(description->source, description->target, description->fstype, description->flags, description->data) == 0) {
		description++;
	}

	if(description->source != NULL) {
		fprintf(stderr, "initrfs: Unable to mount filesystem '%s' (%s): %s\n", description->target, description->fstype, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void
unmount_filesystems(const struct mount_description *description) {

	while(description->source != NULL && umount(description->target) == 0) {
		description++;
	}

	if(description->source != NULL) {
		fprintf(stderr, "initrfs: Unable to unmount filesystem '%s' (%s): %s\n", description->target, description->fstype, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

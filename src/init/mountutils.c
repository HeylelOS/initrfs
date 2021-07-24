#include "mountutils.h"

#include <string.h>
#include <sys/mount.h>
#include <err.h>

#define INTERNAL_BUFFER_CAPACITY_DEVICE 128
#define INTERNAL_BUFFER_CAPACITY_FSTYPE 16

void
mount_filesystems(const struct mount_description *description) {

	while(description->source != NULL && mount(description->source, description->target, description->fstype, description->flags, description->data) == 0) {
		description++;
	}

	if(description->source != NULL) {
		err(1, "Unable to mount filesystem '%s' (%s)", description->target, description->fstype);
	}
}

void
unmount_filesystems(const struct mount_description *description) {

	while(description->source != NULL && umount(description->target) == 0) {
		description++;
	}

	if(description->source != NULL) {
		err(1, "Unable to unmount filesystem '%s' (%s)", description->target, description->fstype);
	}
}

const char *
mount_resolve_device(const char *device) {
	static char buffer[INTERNAL_BUFFER_CAPACITY_DEVICE];

	if(*device != '/') {
		/* Only the device is specified (eg. 'sda') */
		static const char dev[] = "/dev/";
		strncpy(buffer, dev, sizeof(dev) - 1);
		strncpy(buffer + sizeof(dev) - 1, device, sizeof(buffer) - (sizeof(dev) - 1));
	} else {
		/* Absolute path is specified */
		strncpy(buffer, device, sizeof(buffer));
	}

	if(buffer[sizeof(buffer) - 1] != '\0') {
		errx(1, "Device name '%s' is too long", device);
	}

	return buffer;
}

const char *
mount_resolve_fstype(const char *fstype) {
	static char buffer[INTERNAL_BUFFER_CAPACITY_FSTYPE];

	strncpy(buffer, fstype, sizeof(buffer));

	if(buffer[sizeof(buffer) - 1] != '\0') {
		errx(1, "Filesystem type name '%s' is too long", fstype);
	}

	return buffer;
}

unsigned long
mount_resolve_flags(const char *flags) {
	return 0;
}

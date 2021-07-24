#ifndef MOUNTUTILS_H
#define MOUNTUTILS_H

struct mount_description {
	const char *source, *target;
	const char *fstype, *data;
	unsigned long flags;
};

void
mount_filesystems(const struct mount_description *description);

void
unmount_filesystems(const struct mount_description *description);

const char *
mount_resolve_device(const char *device);

const char *
mount_resolve_fstype(const char *fstype);

unsigned long
mount_resolve_flags(const char *flags);

/* MOUNTUTILS_H */
#endif

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

/* MOUNTUTILS_H */
#endif

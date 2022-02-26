/* SPDX-License-Identifier: BSD-3-Clause */
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

char *
mount_resolve_device(const char *device);

char *
mount_resolve_fstype(const char *fstype);

unsigned long
mount_resolve_options(const char *options, char **datap);

/* MOUNTUTILS_H */
#endif

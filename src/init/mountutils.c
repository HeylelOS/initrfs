/* SPDX-License-Identifier: BSD-3-Clause */
#include "mountutils.h"

#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <err.h>

struct mount_option {
	unsigned long mask;
	const char *on, *off;
};

void
mount_filesystems(const struct mount_description *description) {

	while (description->source != NULL && mount(description->source, description->target, description->fstype, description->flags, description->data) == 0) {
		description++;
	}

	if (description->source != NULL) {
		err(1, "Unable to mount filesystem '%s' (%s)", description->target, description->fstype);
	}
}

void
unmount_filesystems(const struct mount_description *description) {

	while (description->source != NULL && umount(description->target) == 0) {
		description++;
	}

	if (description->source != NULL) {
		err(1, "Unable to unmount filesystem '%s' (%s)", description->target, description->fstype);
	}
}

char *
mount_resolve_device(const char *device) {
	char *path;

	if (*device != '/') {
		if (strchr(device, '/') != NULL) {
			/* Relative paths are invalid */
			errx(1, "Invalid relative device path '%s'", device);
		} else if (strcmp("none", device) == 0) {
			/* None expands to empty string */
			path = strdup("");
		} else {
			/* Only the device is specified (eg. 'sda') */
			static const char dev[] = "/dev/";
			const size_t length = strlen(device);
			char buffer[sizeof (dev) + length];

			strncpy(buffer, dev, sizeof (dev) - 1);
			strncpy(buffer + sizeof (dev) - 1, device, length + 1);

			path = strdup(buffer);
		}
	} else {
		/* Absolute path is specified */
		path = strdup(device);
	}

	if (path == NULL) {
		errx(1, "Device name '%s' is too long", device);
	}

	return path;
}

char *
mount_resolve_fstype(const char *fstype) {
	char * const copy = strdup(fstype);

	if (copy == NULL) {
		errx(1, "Filesystem type name '%s' is too long", fstype);
	}

	return copy;
}

static const char *
strchrnul(const char *string, int c) {
	const char *end = strchr(string, c);

	if (end == NULL) {
		end = string + strlen(string);
	}

	return end;
}

static int
mount_option_next(const char **optionsp, const char **optionp, size_t *optionlenp, size_t *keylenp) {
	const char *option = *optionsp;
	const size_t optionlen = strchrnul(option, ',') - option;
	const char *key = memchr(option, '=', optionlen);

	*optionp = option;
	*optionlenp = optionlen;
	*keylenp = key == NULL ? optionlen : key - option;

	if (option[optionlen] != '\0') {
		*optionsp = option + optionlen + 1;
		return 0;
	} else {
		return -1;
	}
}

static const struct mount_option mountoptions[] = {
	{ MS_RDONLY, "ro", "rw" },
	{ MS_STRICTATIME, "strictatime", "nostrictatime" },
	{ MS_NODIRATIME, "nodiratime", "diratime" },
	{ MS_RELATIME, "relatime", "norelatime" },
	{ MS_LAZYTIME, "lazytime", "nolazytime" },
	{ MS_NOATIME, "noatime", "atime" },
	{ MS_SILENT, "silent", "loud" },
	{ MS_DIRSYNC, "dirsync", "nodirsync" },
	{ MS_SYNCHRONOUS, "sync", "nosync" },
	{ MS_MANDLOCK, "mand", "nomand" },
	{ MS_NOSYMFOLLOW, "nosymfollow", "symfollow" },
	{ MS_NOEXEC, "noexec", "exec" },
	{ MS_NOSUID, "nosuid", "suid" },
	{ MS_NODEV, "nodev", "dev" },
	{ MS_I_VERSION, "iversion", "noiversion" },
};

unsigned long
mount_resolve_options(const char *options, char **datap) {
	unsigned long flags = 0;
	char *data = NULL;

	size_t keylen, optionlen;
	const char *option;
	while (mount_option_next(&options, &option, &optionlen, &keylen) == 0) {
		if (optionlen == keylen) {

			if (strncmp("defaults", option, optionlen) == 0) {
				flags &= ~(MS_RDONLY | MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_SYNCHRONOUS);
				continue;
			}

			const struct mount_option *current = mountoptions,
				* const end = mountoptions + sizeof (mountoptions) / sizeof (*mountoptions);
			while (current != end) {

				if (strncmp(current->on, option, optionlen) == 0) {
					flags |= current->mask;
					break;
				}

				if (strncmp(current->off, option, optionlen) == 0) {
					flags &= ~current->mask;
					break;
				}

				current++;
			}

			if (current == end) {
				warnx("Unknown mount option '%.*s'", (int)optionlen, option);
			}
		} else if (strncmp("data", option, keylen) == 0) {

			free(data);
			data = strndup(option + keylen + 1, optionlen - keylen - 1);

			if (data != NULL) {
				warnx("Mount data option too long '%.*s'", (int)optionlen, option);
			}
		}
	}

	*datap = data;

	return flags;
}

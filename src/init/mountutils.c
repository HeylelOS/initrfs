#include "mountutils.h"

#include <string.h>
#include <sys/mount.h>
#include <linux/mount.h>
#include <err.h>

#define INTERNAL_BUFFER_CAPACITY_DEVICE 128
#define INTERNAL_BUFFER_CAPACITY_FSTYPE 16

struct mount_option {
	unsigned long mask;
	const char *on, *off;
};

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
		if(strchr(device, '/') != NULL) {
			/* Relative paths are invalid */
			errx(1, "Invalid relative device path '%s'", device);
		} else if(strcmp("none", device) == 0) {
			/* None expands to empty string */
			strncpy(buffer, "", sizeof(buffer));
		} else {
			/* Only the device is specified (eg. 'sda') */
			static const char dev[] = "/dev/";
			strncpy(buffer, dev, sizeof(dev) - 1);
			strncpy(buffer + sizeof(dev) - 1, device, sizeof(buffer) - (sizeof(dev) - 1));
		}
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

static const char *
strchrnul(const char *string, int c) {
	const char *end = strchr(string, c);

	if(end == NULL) {
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

	if(option[optionlen] != '\0') {
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
mount_resolve_options(const char *options, char *databuffer, size_t datasize) {
	unsigned long flags = 0;

	memset(databuffer, 0, datasize);

	size_t keylen, optionlen;
	const char *option;
	while(mount_option_next(&options, &option, &optionlen, &keylen) == 0) {
		if(optionlen == keylen) {

			if(strncmp("defaults", option, optionlen) == 0) {
				flags &= ~(MS_RDONLY | MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_SYNCHRONOUS);
				continue;
			}

			const struct mount_option *current = mountoptions,
				* const end = mountoptions + sizeof(mountoptions) / sizeof(*mountoptions);
			while(current != end) {

				if(strncmp(current->on, option, optionlen) == 0) {
					flags |= current->mask;
					break;
				}

				if(strncmp(current->off, option, optionlen) == 0) {
					flags &= ~current->mask;
					break;
				}

				current++;
			}

			if(current == end) {
				warnx("Unknown mount option '%.*s'", (int)optionlen, option);
			}
		} else if(strncmp("data", option, keylen) == 0) {
			const char *value = option + keylen + 1;

			strncpy(databuffer, value, datasize);
			if(databuffer[datasize - 1] != '\0') {
				warnx("Mount data option too long for buffer of size %lu in '%*s'", datasize, (int)optionlen, option);
				memset(databuffer, 0, datasize);
			}
		}
	}

	return flags;
}

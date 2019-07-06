#include "filesystem.h"

#include <mntent.h>
#include <string.h>
#include <sys/mount.h>
#include <err.h>

int
filesystem_tab_mount(const char *fstab) {
	int failed = 0;

	if(strcmp("none", fstab) != 0) {
		FILE *tab = setmntent(fstab, "r");

		if(tab != NULL) {
			struct mntent *entry;

			while((entry = getmntent(tab)) != NULL) {
				if(mount(entry->mnt_fsname, entry->mnt_dir, entry->mnt_type,
					filesystem_mount_flags(entry->mnt_opts), NULL) == -1) {
					warn("Unable to mount %s at %s as %s",
						entry->mnt_fsname, entry->mnt_dir, entry->mnt_type);
					failed++;
				}
			}

			endmntent(tab);
		} else {
			warn("Unable to open %s", fstab);
			failed = -1;
		}
	}

	return failed;
}

unsigned int
filesystem_mount_flags(const char *opts) {
	unsigned int mountflags = 0;

	if(strstr(opts, "defaults") == NULL) {
		if(strstr(opts, "ro") != NULL) {
			mountflags |= MS_RDONLY;
		}

		if(strstr(opts, "nodev") != NULL) {
			mountflags |= MS_NODEV;
		}

		if(strstr(opts, "nosuid") != NULL) {
			mountflags |= MS_NOSUID;
		}

		if(strstr(opts, "sync") != NULL) {
			mountflags |= MS_SYNCHRONOUS;
		}
	}

	return mountflags;
}


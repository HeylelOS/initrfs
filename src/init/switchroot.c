/* SPDX-License-Identifier: BSD-3-Clause */
#include "switchroot.h"

#include <string.h>
#include <dirent.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

static void
obliterate_directory(int fd) {
	DIR *dirp = fdopendir(fd);
	struct dirent *entry;

	if(dirp == NULL) {
		err(1, "fdopendir");
	}

	while(errno = 0, entry = readdir(dirp), entry != NULL) {
		if(strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0) {
			int flags = 0;

			if(entry->d_type == DT_DIR) {
				int entryfd = openat(dirfd(dirp), entry->d_name, O_RDONLY);

				if(entryfd < 0) {
					err(1, "openat");
				}

				obliterate_directory(entryfd);

				flags = AT_REMOVEDIR;
			}

			if(unlinkat(dirfd(dirp), entry->d_name, flags) != 0) {
				err(1, "unlinkat");
			}
		}
	}

	if(errno != 0) {
		err(1, "readdir");
	}

	if(closedir(dirp) != 0) {
		err(1, "closedir");
	}
}

void
switch_root(const char *rootmnt) {
	int oldrootfd = open("/", O_RDONLY);

	if(oldrootfd == -1) {
		errx(1, "Unable to open /");
	}

	if(chdir(rootmnt) == -1) {
		err(1, "Unable to chdir to %s", rootmnt);
	}

	if(mount(rootmnt, "/", NULL, MS_MOVE, NULL) == -1) {
		err(1, "Unable to move %s to /", rootmnt);
	}

	if(chroot(".") == -1) {
		err(1, "Unable to chroot to new root");
	}

	obliterate_directory(oldrootfd);
}

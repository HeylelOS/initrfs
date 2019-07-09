#include "candidate.h"
#include "filesystem.h"

#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <err.h>

static void
dir_remove_entries(int fd) {
	DIR *dirp = fdopendir(fd);
	struct dirent *entry;

	while((entry = readdir(dirp)) != NULL) {
		if(strcmp(".", entry->d_name) != 0
			&& strcmp("..", entry->d_name) != 0) {
			int flags = 0;

			if(entry->d_type == DT_DIR) {
				int entryfd = openat(dirfd(dirp), entry->d_name, O_RDONLY);

				dir_remove_entries(entryfd);

				close(entryfd);

				flags = AT_REMOVEDIR;
			}

			unlinkat(dirfd(dirp), entry->d_name, flags);
		}
	}
}

static void
switch_root(struct candidate *root) {
	int oldrootfd = open("/", O_RDONLY);
	if(oldrootfd == -1) {
		err(1, "Unable to open /");
	}

	if(mount("/mnt", "/", NULL, MS_MOVE, NULL) == -1) {
		err(1, "Unable to move /mnt to / for new root %s", root->device);
	}

	if(chroot(".") == -1) {
		err(1, "Unable to chroot to new root");
	}

	dir_remove_entries(oldrootfd);
	close(oldrootfd);

	const char *init = "/sbin/init";
	char * const arguments[] = { "init", NULL };
	if(filesystem_tab_mount(root->fstab) == 0) {
		if(execv(init, arguments) == -1) {
			warn("Unable to execute \"%s\" for %s", init, root->device);
		}
	} else {
		warnx("Unable to correctly mount fstab, using recovery init");
	}

	if(execv(root->recover, arguments) == -1) {
		err(1, "Unable to execute recover init \"%s\" for %s", root->recover, root->device);
	}
}

int
main(void) {
	struct candidate *list;

	if(candidate_list_create(&list) == -1) {
		goto main_err0;
	}

	if(mount("", "/dev", "devtmpfs", 0, NULL) == -1) {
		warn("Unable to mount /dev");
		goto main_err1;
	}

	struct candidate *root = list;
	while(root != NULL) {
		if(candidate_prepare(root) == 0) {
			switch_root(root);
			/* No return */
		} else {
			warnx("Candidate %s as %s failed", root->device, root->fstype);
		}

		root = root->next;
	}

	warnx("Unable to mount any candidate");

main_err1:
	candidate_list_destroy(list);
main_err0:
	return 1;
}


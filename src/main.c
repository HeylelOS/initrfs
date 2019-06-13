#include "candidate.h"

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <err.h>
#include <sys/mount.h>

static void
dir_remove_entries(int fd) {
	DIR *dirp = fdopendir(fd);
	struct dirent *entry;

	while((entry = readdir(dirp)) != NULL) {
		int flags = 0;

		if(entry->d_type == DT_DIR
			&& strcmp(".", entry->d_name) != 0
			&& strcmp("..", entry->d_name) != 0) {
			int entryfd = openat(dirfd(dirp), entry->d_name, O_RDONLY);

			dir_remove_entries(entryfd);

			close(entryfd);

			flags = AT_REMOVEDIR;
		}

		unlinkat(dirfd(dirp), entry->d_name, flags);
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

	char * const arguments[] = { "init", NULL };
	if(execv("/init", arguments) == -1) {
		err(1, "Unable to execute init for profile %s of %s", root->profile, root->device);
	}
}

int
prepare_candidate(struct candidate const *candidate) {
	char source[PATH_MAX];
	snprintf(source, sizeof(source), "/dev/%s", candidate->device);

	/* Mounting candidate */
	if(mount(source, "/mnt", candidate->fstype, 0, NULL) == -1) {
		warn("Unable to mount candidate %s as %s at /mnt", candidate->device, candidate->fstype);
		goto init_candidate_err0;
	}

	/* Moving /dev */
	if(mount("/dev", "/mnt/dev", NULL, MS_MOVE, NULL) == -1) {
		warn("Unable to mount /dev for candidate %s", candidate->device);
		goto init_candidate_err1;
	}

	/* Better move before next mount */
	if(chdir("/mnt") == -1) {
		warn("Unable to chdir to /mnt for candidate %s", candidate->device);
		goto init_candidate_err2;
	}

	return 0;

init_candidate_err2:
	if(mount("/mnt/dev", "/dev", NULL, MS_MOVE, NULL) == -1) {
		warn("Unable to reset /mnt/dev to /dev for %s", candidate->device);
	}

init_candidate_err1:
	if(umount("/mnt") == -1) {
		warn("Unable to unmount failing candidate");
	}

init_candidate_err0:
	return -1;
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
		if(prepare_candidate(root) == 0) {
			switch_root(root);
			/* No return */
		}

		root = root->next;
	}

	warnx("Unable to mount any candidate");

main_err1:
	candidate_list_destroy(list);
main_err0:
	return 1;
}


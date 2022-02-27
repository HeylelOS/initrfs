/* SPDX-License-Identifier: BSD-3-Clause */
#include <dirent.h>

#include <stdlib.h> /* malloc, free */
#include <unistd.h> /* close */

#include <_/NULL.h>
#include <_/ssize_t.h>

_Static_assert(sizeof(long) == 8, "long is not a 64-bits type");

ssize_t getdents64(int, struct dirent *, unsigned int);

DIR *
fdopendir(int fd) {
	DIR * const dirp = malloc(sizeof(*dirp));

	if(dirp != NULL) {
		dirp->_fd = fd;
	}

	return dirp;
}

int
closedir(DIR *dirp) {
	const int fd = dirp->_fd;

	free(dirp);

	return close(fd);
}

struct dirent *
readdir(DIR *dirp) {
	static struct dirent *buffer;
	static size_t capacity;

	if(buffer == NULL) {
		capacity = 500;
		buffer = malloc(capacity);
	}

	struct dirent *entry = buffer;
	if(entry != NULL) {
		const ssize_t readval = getdents64(dirp->_fd, entry, capacity);

		if(readval <= 0) {
			entry = NULL;
		}
	}

	return entry;
}

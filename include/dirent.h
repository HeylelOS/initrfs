/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _DIRENT_H
#define _DIRENT_H

struct _DIR {
	int _fd;
};

/* The following is aligned with linux's
 * struct linux_dirent64, beware size types */
struct dirent {
	long d_ino;
	long d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[];
};

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14

#define dirfd(dirp) ((dirp)->_fd)

typedef struct _DIR DIR;

DIR *fdopendir(int);
int closedir(DIR *);

struct dirent *readdir(DIR *);

/* _DIRENT_H */
#endif

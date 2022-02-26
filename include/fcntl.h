/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _FCNTL_H
#define _FCNTL_H

#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR   02

#define AT_FDCWD -100

#define AT_REMOVEDIR 0x200

typedef unsigned int mode_t;

int open(const char *, int, ...);
int openat(int, const char *, int, ...);

/* _FCNTL_H */
#endif

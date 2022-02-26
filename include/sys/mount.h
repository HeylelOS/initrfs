/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#define MS_RDONLY 1
#define MS_NOSUID 2
#define MS_NODEV 4
#define MS_NOEXEC 8
#define MS_SYNCHRONOUS 16
#define MS_MANDLOCK 64
#define MS_DIRSYNC 128
#define MS_NOSYMFOLLOW 256
#define MS_NOATIME 1024
#define MS_NODIRATIME 2048
#define MS_MOVE 8192
#define MS_SILENT 32768
#define MS_RELATIME (1 << 21)
#define MS_I_VERSION (1 << 23)
#define MS_STRICTATIME (1 << 24)
#define MS_LAZYTIME (1 << 25)

int mount(const char *, const char *, const char *, unsigned long, const void *);
int umount(const char *);
int umount2(const char *, int);

/* _SYS_MOUNT_H */
#endif

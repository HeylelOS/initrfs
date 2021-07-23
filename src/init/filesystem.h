#ifndef FILESYSTEM_H
#define FILESYSTEM_H

int
filesystem_tab_mount(const char *fstab);

unsigned int
filesystem_mount_flags(const char *opts);

/* FILESYSTEM_H */
#endif

#ifndef KERNELCMDLINE_H
#define KERNELCMDLINE_H

struct kernel_cmdline {
	char *root, *rootfstype;
	unsigned long rootflags, rootdelay;
};

void
kernel_cmdline_parse(struct kernel_cmdline *cmdline);

void
kernel_cmdline_mount_root(const struct kernel_cmdline *cmdline, const char *rootmnt);

/* KERNELCMDLINE_H */
#endif

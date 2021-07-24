#ifndef KERNELCMDLINE_H
#define KERNELCMDLINE_H

struct kernel_cmdline {
	const char *init;
	const char *root;
	const char *rootfstype;
	unsigned long rootflags;
	unsigned long rootdelay;
};

void
kernel_cmdline_parse(struct kernel_cmdline *cmdline);

void
kernel_cmdline_mount_root(const struct kernel_cmdline *cmdline, const char *rootmnt);

/* KERNELCMDLINE_H */
#endif

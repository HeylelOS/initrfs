#ifndef KERNELCMDLINE_H
#define KERNELCMDLINE_H

struct kernel_cmdline {
	char *init;
	char *root;
	char *rootfstype;
	char *rootdata;
	unsigned long rootflags;
	unsigned long rootdelay;
};

void
kernel_cmdline_parse(struct kernel_cmdline *cmdline);

void
kernel_cmdline_mount_root(const struct kernel_cmdline *cmdline, const char *rootmnt);

/* KERNELCMDLINE_H */
#endif

#ifndef KERNELCMDLINE_H
#define KERNELCMDLINE_H

struct kernel_cmdline {
	const char *init;
	const char *root;
	const char *rootfstype;
	const char *rootdata;
	unsigned long rootflags;
	unsigned long rootdelay;
};

const char *
kernel_cmdline_init(const char *init);

void
kernel_cmdline_parse(struct kernel_cmdline *cmdline);

void
kernel_cmdline_mount_root(const struct kernel_cmdline *cmdline, const char *rootmnt);

/* KERNELCMDLINE_H */
#endif

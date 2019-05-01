#ifndef PARTITION_H
#define PARTITION_H

#include <sys/types.h>

struct partition {
	struct partition_device {
		int major;
		int minor;
	} device;

	blkcnt_t blocks;
	char name[32];

	struct partition *next;
};

int
partition_list_create(struct partition **listp);

void
partition_list_destroy(struct partition *list);

/* PARTITION_H */
#endif

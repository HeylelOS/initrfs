#include "partition.h"
#include "early_io.h"

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int
partition_list_create(struct partition **listp) {
	FILE *partitionsfile;
	ssize_t linelen;
	size_t linecap = 0;
	char *line = NULL;
	int retval = -1;

	*listp = NULL;

	if((partitionsfile = fopen("/proc/partitions", "r")) == NULL) {
		goto partition_list_create_err0;
	}

	while((linelen = getline(&line, &linecap, partitionsfile)) != -1
		&& *line != '\n');

	while((linelen = getline(&line, &linecap, partitionsfile)) != -1) {
		struct partition *partition = calloc(1, sizeof(*partition));

		if(line[linelen - 1] == '\n') {
			line[linelen - 1] = '\0';
		}

		if(sscanf(line, "%d %d %ld %31s",
			&partition->device.major, &partition->device.minor,
			&partition->blocks, partition->name) != 4) {
			partition_list_destroy(*listp);
			*listp = NULL;
			goto partition_list_create_err1;
		}

		partition->next = *listp;
		*listp = partition;
	}

	retval = 0;

partition_list_create_err1:
	free(line);
	fclose(partitionsfile);

partition_list_create_err0:
	return retval;
}

void
partition_list_destroy(struct partition *list) {

	while(list != NULL) {
		struct partition *next = list->next;
		free(list);
		list = next;
	}
}


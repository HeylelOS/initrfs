#include "candidate.h"

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int
candidate_list_create(struct candidate **listp) {
	FILE *candidatesfile = fopen("/candidates", "r");

	if(candidatesfile == NULL) {
		warn("Unable to open candidates list file\n");
		return -1;
	}

	int scanval;
	struct candidate candidate;
	memset(&candidate, 0, sizeof(candidate));
	*listp = NULL;
	while((scanval = fscanf(candidatesfile, "%31s %15s %63s",
		candidate.device, candidate.fstype, candidate.profile)) != EOF) {

		if(scanval == 3) {
			candidate.next = *listp;
			*listp = malloc(sizeof(**listp));
			**listp = candidate;
		}
	}

	if(ferror(candidatesfile) != 0) {
		warn("Error when fetching a candidate");
	}

	return 0;
}

void
candidate_list_destroy(struct candidate *list) {

	while(list != NULL) {
		struct candidate *next = list->next;
		free(list);
		list = next;
	}
}


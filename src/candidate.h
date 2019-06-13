#ifndef CANDIDATE_H
#define CANDIDATE_H

struct candidate {
	char profile[64];
	char device[32];
	char fstype[16];

	struct candidate *next;
};

int
candidate_list_create(struct candidate **listp);

void
candidate_list_destroy(struct candidate *list);

/* CANDIDATE_H */
#endif

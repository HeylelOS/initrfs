#ifndef CANDIDATE_H
#define CANDIDATE_H

struct candidate {
	char recover[32];
	char device[32];
	char options[32];
	char fstab[32];
	char fstype[16];

	struct candidate *next;
};

int
candidate_list_create(struct candidate **listp);

void
candidate_list_destroy(struct candidate *list);

int
candidate_prepare(struct candidate *candidate);

/* CANDIDATE_H */
#endif

/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef TRIMSTR_H
#define TRIMSTR_H

#include <stddef.h>
#include <ctype.h>

static inline char *
trimstr(char *trimmed, size_t length) {

	/* Trim the beginning */
	while(isspace(*trimmed)) {
		length--;
		trimmed++;
	}

	/* Trim the end */
	while(isspace(trimmed[length - 1])) {
		length--;
	}
	/* If there wasn't anything to end-trim, no problem,
	we will just be replacing the previous nul delimiter */
	trimmed[length] = '\0';

	return trimmed;
}

/* TRIMSTR_H */
#endif

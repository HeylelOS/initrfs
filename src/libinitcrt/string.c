/* SPDX-License-Identifier: BSD-3-Clause */
#include <string.h>

#include <stdlib.h> /* malloc */
#include <stdbool.h> /* bool, false */
#include <_/uint8_t.h>

void *
memchr(const void *pointer, int value, size_t count) {
	const uint8_t * const end = (const uint8_t *)pointer + count;
	const uint8_t *current = pointer;
	const uint8_t byte = value;

	while(current != end && *current != byte) {
		current++;
	}

	return current != end ? (void *)current : NULL;
}

void *
memcpy(void *destination, const void *source, size_t count) {
	const uint8_t * const end = (const uint8_t *)source + count;
	const uint8_t *current = source;
	uint8_t *buffer = destination;

	while(current != end) {
		*buffer = *current;
		current++;
		buffer++;
	}

	return destination;
}

void *
memmove(void *destination, const void *source, size_t count) {
	const void * const upper = (const uint8_t *)source + count;

	if(source > destination || (source < destination && upper < destination)) {
		/* memcpy copies byte by byte from the beginning, this way is safe */
		memcpy(destination, source, count);
	} else if(source != destination) {
		const uint8_t * const end = (const uint8_t *)source - 1;
		const uint8_t *current = (const uint8_t *)upper - 1;
		uint8_t *buffer = (uint8_t *)destination + count - 1;

		while(current != end) {
			*buffer = *current;
			current--;
			buffer--;
		}
	} /* else source == destination, no copy required */

	return destination;
}

void *
memset(void *destination, int value, size_t count) {
	const uint8_t * const end = (const uint8_t *)destination + count;
	uint8_t *current = destination;
	const uint8_t byte = value;

	while(current != end) {
		*current = byte;
		current++;
	}

	return destination;
}

int
memcmp(const void *lhs, const void *rhs, size_t count) {
	const char *left = lhs, *right = rhs;
	size_t i = 0;
	int diff;

	while(i != count && !(diff = left[i] - right[i], diff)) {
		i++;
	}

	return diff;
}

char *
strchr(const char *string, int value) {
	return memchr(string, value, strlen(string));
}

char *
strncpy(char *buffer, const char *string, size_t count) {
	const size_t length = strlen(string);

	if(length < count) {
		return memset((uint8_t *)memcpy(buffer, string, length) + length, 0, count - length);
	} else {
		return memcpy(buffer, string, count);
	}
}

int
strncmp(const char *lhs, const char *rhs, size_t count) {
	size_t lhslen = strlen(lhs), rhslen = strlen(rhs);

	if(lhslen < count) {
		count = lhslen;
	}

	if(rhslen < count) {
		count = rhslen;
	}

	return memcmp(lhs, rhs, count);
}

int
strcmp(const char *lhs, const char *rhs) {

	while(*lhs != '\0' && *lhs - *rhs == 0) {
		lhs++;
		rhs++;
	}

	return *lhs - *rhs;
}

size_t
strlen(const char *string) {
	return (const char *)memchr(string, 0, -1) - string;
}

char *
strndup(const char *string, size_t count) {
	const size_t length = strlen(string);

	if(length < count) {
		count = length;
	}

	char * const copy = malloc(count + 1);

	if(copy != NULL) {
		((char *)memcpy(copy, string, count))[count] = '\0';
	}

	return copy;
}

char *
strdup(const char *string) {
	const size_t length = strlen(string);
	char * const copy = malloc(length + 1);

	if(copy != NULL) {
		memcpy(copy, string, length + 1);
	}

	return copy;
}

char *
strsep(char **stringp, const char *delimiters) {
	char * const string = *stringp;

	if(string != NULL) {
		const char *delimiter;
		char *token = string;

		/* Iterating our string for a token */
		while(*token != '\0') {
			/* Reset delimiter iterator */
			delimiter = delimiters;

			/* Is the current character a delimiter? */
			while(*delimiter != '\0' && *token != *delimiter) {
				delimiter++;
			}

			/* If it is, break iteration now */
			if(*delimiter != '\0') {
				break;
			}

			token++;
		}

		/* Found a delimiter? Mark it, else set next to NULL */
		if(*token != '\0') {
			*token = '\0';
			*stringp = token + 1;
		} else {
			*stringp = NULL;
		}
	}

	return string;
}

static char * const errstrings[] = {
	"Unknown error",
	"Operation not permitted",
	"No such file or directory",
	"No such process",
	"Interrupted system call",
	"I/O error",
	"No such device or address",
	"Argument list too long",
	"Exec format error",
	"Bad file number",
	"No child processes",
	"Try again",
	"Out of memory",
	"Permission denied",
	"Bad address",
	"Block device required",
	"Device or resource busy",
	"File exists",
	"Cross-device link",
	"No such device",
	"Not a directory",
	"Is a directory",
	"Invalid argument",
	"File table overflow",
	"Too many open files",
	"Not a typewriter",
	"Text file busy",
	"File too large",
	"No space left on device",
	"Illegal seek",
	"Read-only file system",
	"Too many links",
	"Broken pipe",
	"Math argument out of domain of func",
	"Math result not representable",
};

char *
strerror(int errcode) {
	if(errcode < 0 || errcode > sizeof(errstrings) / sizeof(*errstrings)) {
		errcode = 0;
	}

	return errstrings[errcode];
}

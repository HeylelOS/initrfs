/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _STRING_H
#define _STRING_H

#include <_/NULL.h>
#include <_/size_t.h>

void *memchr(const void *, int, size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);
int memcmp(const void *, const void *, size_t);

char *strchr(const char *, int);
char *strncpy(char *, const char *, size_t);
int strncmp(const char *, const char *, size_t);
int strcmp(const char *, const char *);

size_t strlen(const char *);
char *strndup(const char *, size_t);
char *strdup(const char *);

char *strsep(char **, const char *);

char *strerror(int);

/* _STRING_H */
#endif

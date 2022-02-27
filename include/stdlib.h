/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdnoreturn.h>

#include <_/NULL.h>
#include <_/size_t.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void *malloc(size_t);
void *realloc(void *, size_t);
void free(void *);

void noreturn exit(int);

unsigned long strtoul(const char *, char **, int);

/* _STDLIB_H */
#endif

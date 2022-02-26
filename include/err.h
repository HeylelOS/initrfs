/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _ERR_H
#define _ERR_H

#include <stdnoreturn.h>

void noreturn err(int, const char *, ...);
void noreturn errx(int, const char *, ...);

void warn(const char *, ...);
void warnx(const char *, ...);

/* _ERR_H */
#endif

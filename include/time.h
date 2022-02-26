/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _TIME_H
#define _TIME_H

struct timespec {
	long tv_sec;
	long tv_nsec;
};

int nanosleep(const struct timespec *, struct timespec *);

/* _TIME_H */
#endif

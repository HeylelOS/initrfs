#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdnoreturn.h>

#include <_size_t.h>
#include <_ssize_t.h>

void noreturn _exit(int);

ssize_t read(int, void *, size_t);
ssize_t write(int, const void *, size_t);
int close(int);

int unlinkat(int, const char *, int);

int chdir(const char *);
int chroot(const char *);

int execv(const char *, char * const []);
int execve(const char *, char * const [], char * const []);

unsigned int sleep(unsigned int);

/* _UNISTD_H */
#endif

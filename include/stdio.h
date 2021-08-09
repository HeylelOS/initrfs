#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

#include <_size_t.h>
#include <_ssize_t.h>

#define stdin _stdin()
#define stdout _stdout()
#define stderr _stderr()

#define EOF -1

struct _FILE;

typedef struct _FILE FILE;

FILE *_stdin();
FILE *_stdout();
FILE *_stderr();

FILE *fopen(const char *, const char *);
int fclose(FILE *);

ssize_t getline(char **, size_t *, FILE *);
ssize_t getdelim(char **, size_t *, int, FILE *);

int fputc(int, FILE *);
int fputs(const char *, FILE *);
int puts(const char *);

int vfprintf(FILE *, const char *, va_list);

/* _STDIO_H */
#endif

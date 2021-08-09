#include <err.h>

#include <stdio.h> /* vfprintf */
#include <stdlib.h> /* exit */
#include <stdarg.h> /* va_start, va_end */
#include <string.h> /* strerror */
#include <errno.h> /* errno */

void noreturn
err(int eval, const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputs(": ", stderr);
	fputs(strerror(errno), stderr);
	fputc('\n', stderr);

	exit(eval);
}

void noreturn
errx(int eval, const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fputc('\n', stderr);

	exit(eval);
}

void
warn(const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputs(": ", stderr);
	fputs(strerror(errno), stderr);
	fputc('\n', stderr);
}

void
warnx(const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fputc('\n', stderr);
}

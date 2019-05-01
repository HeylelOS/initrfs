#ifndef EARLY_IO
#define EARLY_IO

void
early_abort(const char *message,
	const char *error, ...);

void
early_print(const char *format, ...);

/* EARLY_IO */
#endif

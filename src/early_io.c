#include "early_io.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/reboot.h>

void
early_abort(const char *message,
	const char *error, ...) {
	static char fancy[] =
		"##################################################\n"
		"#              EARLY USERSPACE ABORT             #\n"
		"##################################################\n"
		"#                                                #\n"
		"# ->                                             #\n"
		"#      The system will reboot in 10 seconds      #\n"
		"##################################################";

	char *ptr = stpncpy(fancy + 155, message, 46);
	while(ptr < fancy + 201) {
		*ptr++ = ' ';
	}


	if(error == NULL) {
		ptr = stpncpy(fancy + 209, strerror(errno), 43);
		while(ptr < fancy + 252) {
			*ptr++ = ' ';
		}
	} else {
		va_list ap;
		ssize_t written;

		va_start(ap, error);
		written = vsnprintf(fancy + 209, 43, error, ap);
		va_end(ap);

		if(written >= 0 && written < 43) {
			fancy[209 + written] = ' ';
		}
	}

	puts(fancy);

	/* sleep(10); */

	reboot(RB_POWER_OFF);
}

void
early_print(const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}

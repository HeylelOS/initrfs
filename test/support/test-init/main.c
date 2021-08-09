#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <err.h>

int
main(void) {

	puts("Hello, world!");

	reboot(RB_AUTOBOOT);
	err(EXIT_FAILURE, "reboot");
}


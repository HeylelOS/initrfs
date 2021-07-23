#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <err.h>

int
main(void) {

	puts("Hello, world!");

	if(reboot(RB_AUTOBOOT) == -1) {
		err(EXIT_FAILURE, "reboot");
	}

	/* Never reached */
}


/*
	init.c
	Copyright (c) 2019, Valentin Debon

	This file is part of HeylelOS initramfs test
	subject the BSD 3-Clause License, see LICENSE
*/
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


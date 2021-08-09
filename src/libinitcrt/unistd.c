#include <unistd.h>

#include <time.h> /* nanosleep */

extern char **environ;

int
execv(const char *pathname, char * const argv[]) {
	return execve(pathname, argv, environ);
}

unsigned int
sleep(unsigned int seconds) {
	struct timespec rqt = { .tv_sec = seconds }, rmt;

	if(nanosleep(&rqt, &rmt) != 0) {
		return rmt.tv_sec;
	}

	return 0;
}

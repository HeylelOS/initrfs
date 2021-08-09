#include <errno.h>

int errno;

long
_propagate_errno(long returned) {
	if(returned >= 0) {
		return returned;
	} else {
		errno = -returned;
		return -1;
	}
}

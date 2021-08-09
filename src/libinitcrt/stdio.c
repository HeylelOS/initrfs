#include <stdio.h>

#include <stdlib.h> /* malloc, free */
#include <string.h> /* strlen */
#include <unistd.h> /* read, write, close */
#include <fcntl.h> /* open */

#include <_NULL.h>

struct _FILE {
	char *_ibuffer;
	size_t _icount;
	int _fd;
};

static int
_FILE_input_read(FILE *filep) {
	const size_t size = 500;

	filep->_ibuffer = realloc(filep->_ibuffer, filep->_icount + size);
	if(filep->_ibuffer != NULL) {
		char * const begin = filep->_ibuffer + filep->_icount;
		const ssize_t readval = read(filep->_fd, begin, size);

		if(readval > 0) {
			filep->_icount += readval;
			return 0;
		}
	}

	return -1;
}

FILE *
_stdin() {
	static FILE _file = { ._fd = 0 };
	return &_file;
}

FILE *
_stdout() {
	static FILE _file = { ._fd = 1 };
	return &_file;
}

FILE *
_stderr() {
	static FILE _file = { ._fd = 2 };
	return &_file;
}

FILE *
fopen(const char *pathname, const char *mode) {
	FILE *filep = malloc(sizeof(*filep));

	if(filep != NULL) {
		/* Runtime only supports file reading, mode is ignored */
		filep->_ibuffer = NULL;
		filep->_icount = 0;
		filep->_fd = open(pathname, O_RDONLY, 0666);

		if(filep->_fd < 0) {
			free(filep);
			filep = NULL;
		}
	}

	return filep;
}

int
fclose(FILE *filep) {
	const int fd = filep->_fd;

	free(filep);

	return close(fd);
}

ssize_t
getline(char **linep, size_t *capacityp, FILE *filep) {
	return getdelim(linep, capacityp, '\n', filep);
}

static int
_strredup(char **destp, size_t *capacityp, const char *src, size_t length) {
	size_t capacity = *capacityp;
	char *dest = *destp;

	if(length >= capacity) {
		capacity = length + 1;
		dest = realloc(dest, capacity);
	}

	if(dest != NULL) {

		((unsigned char *)memcpy(dest, src, length))[length] = '\0';
		*capacityp = capacity;
		*destp = dest;

		return 0;
	}

	return -1;
}

ssize_t
getdelim(char **linep, size_t *capacityp, int delimiter, FILE *filep) {
	char *eod; /* End Of Delimition, next character after delimiter if one is there */

	do {
		/* Look for a delimition, if none found, try to fill more of the input buffer */
		eod = memchr(filep->_ibuffer, delimiter, filep->_icount);
	} while(eod == NULL && _FILE_input_read(filep) == 0);

	/* If none is found at this point,
	 * we reached end of file, delimition is set to remainder of input.
	 * Else if we are delimited, we set the delimition end past the delimiter. */
	if(eod == NULL) {
		eod = filep->_ibuffer + filep->_icount;
	} else {
		eod++;
	}

	/* If the input buffer is empty, we reached end of file,
	 * whether eod is NULL or not. Else, if eod is not NULL,
	 * we have at least one delimition to eat. */
	if(filep->_icount != 0 && eod != NULL) {
		const size_t length = eod - filep->_ibuffer;

		if(_strredup(linep, capacityp, filep->_ibuffer, length) == 0) {
			const size_t moved = filep->_icount - length;

			memmove(filep->_ibuffer, eod, moved);
			filep->_icount = moved;

			return length;
		}
	}

	return -1;
}

int
fputc(int value, FILE *filep) {
	const unsigned char character = value;

	if(write(filep->_fd, &character, sizeof(character)) != sizeof(character)) {
		return EOF;
	}

	return character;
}

int
fputs(const char *string, FILE *filep) {
	return write(filep->_fd, string, strlen(string));
}

int
puts(const char *string) {
	const size_t length = strlen(string);

	if(write(stdout->_fd, string, length) == length && fputc('\n', stdout) != EOF) {
		return length + 1;
	} else {
		return EOF;
	}
}

enum _formatter_state {
	_FORMATTER_STATE_PRINT,
	_FORMATTER_STATE_PERCENT,
	_FORMATTER_STATE_PERCENT_DOT,
	_FORMATTER_STATE_PERCENT_DOT_STAR,
	_FORMATTER_STATE_END,
	_FORMATTER_STATE_ERROR_WRITE,
	_FORMATTER_STATE_ERROR_UNFINISHED_CONVERSION,
};

struct _formatter {
	enum _formatter_state state;
	unsigned int total;
	const char *location;
	const char *tag;
	int precision;
	int fd;
};

static void
_formatter_print(struct _formatter *formatter, const char *newtag, enum _formatter_state success) {
	const size_t count = formatter->location - formatter->tag;
	const ssize_t written = write(formatter->fd, formatter->tag, count);

	if(written == count) {
		formatter->tag = newtag;
		formatter->total += count;
		formatter->state = success;
	} else {
		formatter->state = _FORMATTER_STATE_ERROR_WRITE;
	}
}

static inline size_t
_formatter_convert_string_count(const char *string, int precision) {
	size_t count = strlen(string);

	if(precision > 0 && precision < count) {
		count = precision;
	}

	return count;
}

static void
_formatter_convert_string(struct _formatter *formatter, const char *string, enum _formatter_state success) {

	if(string == NULL) {
		string = "(nil)";
	}

	const size_t count = _formatter_convert_string_count(string, formatter->precision);
	const ssize_t written = write(formatter->fd, string, count);

	if(written == count) {
		formatter->tag = formatter->location + 1;
		formatter->total += count;
		formatter->state = success;
	} else {
		formatter->state = _FORMATTER_STATE_ERROR_WRITE;
	}
}

int
vfprintf(FILE *filep, const char *format, va_list ap) {
	/* Runtime only supports %s, %.s and %.*s formats */
	struct _formatter formatter = {
		.state = _FORMATTER_STATE_PRINT,
		.total = 0,
		.location = format,
		.tag = format,
		.fd = filep->_fd,
	};

	while(formatter.state < _FORMATTER_STATE_END) {
		switch(formatter.state) {
		case _FORMATTER_STATE_PRINT:
			switch(*formatter.location) {
			case '%': _formatter_print(&formatter, formatter.location, _FORMATTER_STATE_PERCENT); break;
			case '\0': _formatter_print(&formatter, formatter.location, _FORMATTER_STATE_END); break;
			default: break;
			}
			break;
		case _FORMATTER_STATE_PERCENT:
			switch(*formatter.location) {
			case '.':
				formatter.precision = 0;
				formatter.state = _FORMATTER_STATE_PERCENT_DOT;
				break;
			case 's':
				formatter.precision = -1;
				_formatter_convert_string(&formatter, va_arg(ap, const char *), _FORMATTER_STATE_PRINT);
				break;
			default: _formatter_print(&formatter, formatter.location + 1, _FORMATTER_STATE_PRINT); break;
			}
			break;
		case _FORMATTER_STATE_PERCENT_DOT:
			switch(*formatter.location) {
			case '*':
				formatter.precision = va_arg(ap, int);
				formatter.state = _FORMATTER_STATE_PERCENT_DOT_STAR;
				break;
			default: formatter.state = _FORMATTER_STATE_ERROR_UNFINISHED_CONVERSION; break;
			}
			break;
		case _FORMATTER_STATE_PERCENT_DOT_STAR:
			switch(*formatter.location) {
			case 's': _formatter_convert_string(&formatter, va_arg(ap, const char *), _FORMATTER_STATE_PRINT); break;
			default: formatter.state = _FORMATTER_STATE_ERROR_UNFINISHED_CONVERSION; break;
			}
			break;
		default:
			break;
		}
		formatter.location++;
	}

	return formatter.state == _FORMATTER_STATE_END ? formatter.total : -1;
}


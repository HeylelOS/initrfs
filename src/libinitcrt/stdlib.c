/* SPDX-License-Identifier: BSD-3-Clause */
#include <stdlib.h>

#include <ctype.h> /* isdigit */
#include <string.h> /* memcpy */
#include <unistd.h> /* _exit */
#include <sys/mman.h> /* mmap */

#include <_/uint8_t.h>

#define _clz(value) _Generic(value, \
	unsigned int: __builtin_clz, \
	unsigned long int: __builtin_clzl, \
	unsigned long long int: __builtin_clzll)(value)

#define MALLOC_OVERHEAD sizeof(size_t)

#define MALLOC_SMALLPOOL_MIN_ORDER 0
#define MALLOC_MEDIUMPOOL_MIN_ORDER 5
#define MALLOC_HUGEPOOL_MIN_ORDER 20

#define MALLOC_MEDIUMPOOL_SIZE (MALLOC_HUGEPOOL_MIN_ORDER - MALLOC_MEDIUMPOOL_MIN_ORDER)

struct malloc_arena {
	void *smallpool;
	void *mediumpool[MALLOC_MEDIUMPOOL_SIZE];

	size_t smallorder;
	size_t mediumorder;
};

static inline void *
malloc_allocation_mark(size_t *ptr, size_t size) {

	*ptr = size;

	return ptr + 1;
}

static inline size_t
malloc_allocation(size_t * restrict ptr, void ** restrict allocation) {

	if(ptr == NULL) {
		return 0;
	}

	ptr--;
	*allocation = ptr;

	return *ptr;
}

static size_t 
malloc_smallpool_extend(struct malloc_arena *arena) {
	const size_t size = 1 << (MALLOC_HUGEPOOL_MIN_ORDER + arena->smallorder);
	void * const mmaped = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	if(mmaped != MAP_FAILED) {
		void **iterator = mmaped, *allocationend = (uint8_t *)mmaped + size;

		while((*iterator = (uint8_t *)iterator + (1 << MALLOC_MEDIUMPOOL_MIN_ORDER)) < allocationend) {
			iterator = *iterator;
		}

		*iterator = NULL;

		arena->smallpool = mmaped;
		arena->smallorder++;

		return size;
	} else {
		return 0;
	}
}

static void *
malloc_small(struct malloc_arena *arena, size_t size) {
	void **allocation;

	if(arena->smallpool == NULL && malloc_smallpool_extend(arena) == 0) {
		return NULL;
	}

	allocation = arena->smallpool;
	arena->smallpool = *allocation;

	return malloc_allocation_mark((size_t *)allocation, size);
}

static void
free_small(struct malloc_arena *arena, void **allocation) {

	*allocation = arena->smallpool;
	arena->smallpool = allocation;
}

static inline size_t
malloc_medium_order(size_t value) {
	return 8 * sizeof(value) - _clz(value) - !(value & (value - 1));
}

static size_t 
malloc_mediumpool_extend(struct malloc_arena *arena) {
	const size_t size = 1 << (MALLOC_HUGEPOOL_MIN_ORDER + arena->mediumorder);
	void * const mmaped = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	if(mmaped != MAP_FAILED) {
		void **iterator = mmaped, *allocationend = (uint8_t *)mmaped + size;

		while((*iterator = (uint8_t *)iterator + (1 << (MALLOC_HUGEPOOL_MIN_ORDER - 1))) < allocationend) {
			iterator = *iterator;
		}

		*iterator = NULL;

		arena->mediumpool[MALLOC_MEDIUMPOOL_SIZE - 1] = mmaped;
		arena->mediumorder++;

		return size;
	} else {
		return 0;
	}
}

static void *
malloc_medium(struct malloc_arena *arena, size_t size) {
	const size_t order = malloc_medium_order(size) - MALLOC_MEDIUMPOOL_MIN_ORDER;
	size_t index = order;
	void **allocation;

	while(index < MALLOC_MEDIUMPOOL_SIZE && arena->mediumpool[index] == NULL) {
		index++;
	}

	if(index == MALLOC_MEDIUMPOOL_SIZE) {

		if(malloc_mediumpool_extend(arena) == 0) {
			return NULL;
		}

		index--;
	}

	while(index > order) {
		void **first = arena->mediumpool[index];
		void **buddy = (void **)((uint8_t *)first + (1 << (index - 1 + MALLOC_MEDIUMPOOL_MIN_ORDER)));

		arena->mediumpool[index] = *first;
		index--;

		*buddy = arena->mediumpool[index];
		*first = buddy;
		arena->mediumpool[index] = first;
	}

	allocation = arena->mediumpool[index];
	arena->mediumpool[index] = *allocation;

	return malloc_allocation_mark((size_t *)allocation, size);
}

static void
free_medium(struct malloc_arena *arena, void *allocation, size_t size) {
	size_t index = malloc_medium_order(size) - MALLOC_MEDIUMPOOL_MIN_ORDER;
	void **ptr = allocation;

	do {
		const long size = 1 << (index + MALLOC_MEDIUMPOOL_MIN_ORDER);
		void **lbuddy = (void **)((uint8_t *)ptr - size);
		void **rbuddy = (void **)((uint8_t *)ptr + size);
		void **iterator = arena->mediumpool + index;

		while(iterator != NULL && *iterator != lbuddy && *iterator != rbuddy) {
			iterator = *iterator;
		}

		if(iterator == NULL) {
			break;
		}

		if(*iterator == lbuddy) {
			*iterator = *lbuddy;
			ptr = lbuddy;
		} else { /* *iterator == rbuddy */
			*iterator = *rbuddy;
		}

		index++;
	} while(index < MALLOC_MEDIUMPOOL_SIZE);

	*ptr = arena->mediumpool[index];
	arena->mediumpool[index] = ptr;
}

static void *
malloc_huge(size_t size) {
	void * const mmaped = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	if(mmaped != MAP_FAILED) {
		return malloc_allocation_mark(mmaped, size);
	} else {
		return NULL;
	}
}

static void
free_huge(void *allocation, size_t size) {
	munmap(allocation, size);
}

static struct malloc_arena arena;

void *
malloc(size_t size) {

	size += MALLOC_OVERHEAD;

	if(size >= (1 << MALLOC_HUGEPOOL_MIN_ORDER)) {
		return malloc_huge(size);
	} else if(size >= (1 << MALLOC_MEDIUMPOOL_MIN_ORDER)) {
		return malloc_medium(&arena, size);
	} else if(size > MALLOC_OVERHEAD) {
		return malloc_small(&arena, size);
	}

	return NULL;
}

void *
realloc(void *ptr, size_t size) {

	if(ptr != NULL) {
		void *oldallocation;
		const size_t newsize = size + MALLOC_OVERHEAD;
		const size_t oldsize = malloc_allocation(ptr, &oldallocation);

		if(newsize <= oldsize) {
			return ptr;
		}

		void *newptr = malloc(newsize);

		if(newptr != NULL) {
			memcpy(newptr, ptr, oldsize - MALLOC_OVERHEAD);
			free(ptr);
		}

		return newptr;
	} else {
		return malloc(size);
	}
}

void
free(void *ptr) {

	if(ptr != NULL) {
		void *allocation;
		const size_t size = malloc_allocation(ptr, &allocation);

		if(size >= (1 << MALLOC_HUGEPOOL_MIN_ORDER)) {
			free_huge(allocation, size);
		} else if(size >= (1 << MALLOC_MEDIUMPOOL_MIN_ORDER)) {
			free_medium(&arena, allocation, size);
		} else if(size > MALLOC_OVERHEAD) {
			free_small(&arena, allocation);
		} /* else error, should we check? */
	}
}

void noreturn
exit(int status) {
	_exit(status);
}

unsigned long
strtoul(const char *string, char **endp, int base) {
	/* Runtime only supports base 10 */
	const char *current = string;
	unsigned long value = 0;

	while(isdigit(*current)) {
		value = 10 * value + (unsigned long)(*current - '0');
		current++;
	}

	if(endp != NULL) {
		*endp = (void *)current;
	}

	return value;
}


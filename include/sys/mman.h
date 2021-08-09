#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#define PROT_READ  0x01
#define PROT_WRITE 0x02

#define MAP_PRIVATE   0x02

#define MAP_ANONYMOUS 0x20

#define MAP_FAILED ((void *)-1)

void *mmap(void *, size_t, int, int, int, long);
int munmap(void *, size_t);

/* _SYS_MMAN_H */
#endif

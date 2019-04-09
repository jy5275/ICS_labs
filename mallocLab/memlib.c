/* memlib.c - a module that simulates the memory system. Needed because it 
 * allows us to interleave calls from the student's malloc package 
 * with the system's malloc package in libc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "memlib.h"
#include "config.h"

static char *heap;			/* First byte of heap */
static char *mem_brk;		/* top of heap plus one */
static char *mem_max_addr;  

/* mem_init - initialize the memory system model */
void mem_init(void){
	int dev_zero = open("/dev/zero", O_RDWR);
	heap = mmap((void *)0x800000000, /* suggested start*/
		MAX_HEAP,				/* length */
		PROT_WRITE,				/* permissions */
		MAP_PRIVATE,			/* private or shared? */
		dev_zero,				/* fd */
		0);						/* offset (dunno) */
	mem_max_addr = heap + MAX_HEAP;
	mem_brk = heap;				/* heap is empty initially */
}

/* Free the storage used by the memory system model */
void mem_deinit(void){
	munmap(heap, MAX_HEAP);
}

/* Reset the simulated brk pointer to make an empty heap */
void mem_reset_brk(){
	mem_brk = heap;
}

/* mem_sbrk - Extends the heap by [incr] bytes and returns the start addr
 * of the new area. In this model, heap cannot be shrunk.
 */
void *mem_sbrk(int incr) {
	char *old_brk = mem_brk;

    // call sbrk() in an attempt to have similar semantics as a real allocator.
	if (incr < 0 || (mem_brk + incr) > mem_max_addr || sbrk(incr) == (void *) -1) {
		errno = ENOMEM;
		fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
		return (void *)-1;
	}
	mem_brk += incr;
	return (void *)old_brk;
}

void *mem_heap_lo(){		/* addr of 1st heap byte */
	return (void *)heap;
}

void *mem_heap_hi(){		/* addr of last heap byte */
	return (void *)(mem_brk - 1);
}

size_t mem_heapsize() {		/* heap size (in byte) */
	return (size_t) ((void *)mem_brk - (void *)heap);
}

size_t mem_pagesize(){		/* page size of the system */
	return (size_t)getpagesize();
}

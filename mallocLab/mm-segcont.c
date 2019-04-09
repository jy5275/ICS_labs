/* mm-segcont.c: try to build a continuous list v3.0 [12:32, Sat, Dec 15th] 
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro. When you hand
 * in, remove the #define DEBUG line. */
#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

#define WSIZE       4       /* Word and header/footer size (bytes) */ 
#define DSIZE       8       /* Double word size (bytes) */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define PACK(size, alloc)	((size) | (alloc))
 
#define GET(p)			(*(unsigned*)(p))
#define GETL(p)			(*(char**)(p))		// p should be a ptr to char*, return that char* ptr
#define PUT(p, val)		(*(unsigned*)(p) = (val))
#define PUTL(p, valq)	(*(char**)(p) = (valp))		// p should be a ptr to char*, give that char* ptr a value
#define GET_SIZE(p)		(GET(p) & ~0x7)
#define GET_ALLOC(p)	(GET(p) & 0x1)

#define HDRP(bp)       ((char *)(bp) - WSIZE)
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) 
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) 
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) 
#define PREV_FREE_PTR(bp)	GETL(bp)
#define NEXT_FREE_PTR(bp)	GETL((char*)(bp) + DSIZE)

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */  
static char *pTail, *heap;
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)

int mm_init(void);
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
void *malloc(size_t size);
void free(void *bp);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);

/* Initialize: return -1 on error, 0 on success. */
int mm_init(void) {
    if ((heap_listp = mem_sbrk(24*DSIZE)) == (void *)-1) 
        return -1;
    heap = heap_listp;
    GETL(heap) = NULL;
    
    char *tmp1 = heap, *tmp2 = heap + 8*WSIZE;
    GETL(heap) = NULL;
    GETL(heap + DSIZE) = heap + 4*WSIZE;
    int i = 1;
    while (i < 10) {		// Continuous list
    	GETL(heap + 4*WSIZE*i) = tmp1;			// Set prev-ptr 
    	GETL(heap + 4*WSIZE*i + DSIZE) = tmp2;	// Set next-ptr
    	tmp1 += 4*WSIZE;
    	tmp2 += 4*WSIZE;
    	i++;
    }
    GETL(heap + 40*WSIZE) = heap + 36*WSIZE;
    GETL(heap + 42*WSIZE) = NULL;
    
    heap_listp = heap + 46*WSIZE;
    PUT(heap_listp - DSIZE, 0);					/* Alignment padding */
    PUT(heap_listp - WSIZE, PACK(DSIZE, 1));	/* Prologue header */ 
    PUT(heap_listp, PACK(DSIZE, 1));			/* Prologue footer */ 
    PUT(heap_listp + WSIZE, PACK(0, 1));		/* Epilogue header */
    
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) 
        return -1;
    return 0;
}

/* extend_heap - Extend heap with free block and return its block pointer */
static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE; 
    if ((long)(bp = mem_sbrk(size)) == -1)      // return NULL if mem_sbrk reaches maxlimit or input minus number
		return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */ 

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/* malloc */
void *malloc(size_t size) {   // size is payload, while asize is actual mem size to be alloc including HD&&FT
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;
    
    if (heap_listp == 0)
        mm_init();
    /* Ignore spurious requests */
    if (size == 0)	return NULL;

    /* Block should be no shorter than 6W to be useful. */
    if (size <= 2*DSIZE)
    	asize = 3*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); 

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {	// bp is the ptr that point to availiable free block
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory then place the block */
    extendsize = MAX(asize, CHUNKSIZE);                 
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)  
        return NULL;
    place(bp, asize);
    return bp;
} 

/* place - Place block of [asize] bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));  // csize - original size of this free block

    if ((csize - asize) >= (2*DSIZE)) { // left more than 4 words empty
        PUT(HDRP(bp), PACK(asize, 1));  // Set head of new block
        PUT(FTRP(bp), PACK(asize, 1));	// Set foot of new block
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));  // Set head of left free block
        PUT(FTRP(bp), PACK(csize-asize, 0));  // Set foot of left free block
    }
    else {								// left less than 2 free words - all include in new block
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/* find_fit - Find a fit for a block with asize bytes */
static void *find_fit (size_t asize) {
    /* First-fit search */
    size_t index = GetIndex(asize);
    char *pbase = heap + 4*index*WSIZE;
	char *it = *(char**)(pbase + DSIZE);	// it ptr to 1st free block on list[index]
	
	while (it) {		// Keep itenerating until meet NULL (tail)
		if (!GET_ALLOC(HDRP(it)) && GET_SIZE(HDRP(it)) >= asize)	// find!
			return it;
		it = GETL(it + DSIZE);		// Go to next free block on the whole list
	}
	
    return NULL; /* No fit */
}

/* free - Free a block */
void free(void *bp) {
    if (bp == NULL) 
        return;

    size_t size = GET_SIZE(HDRP(bp));
    if (heap_listp == 0)
        mm_init();

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);   // coalesce if previous or next block is free
}

size_t GetIndex (size_t payload) {
	if (payload <= 16) return 0;			// asize in [0, 16], insert into list0
	else if (payload <= 32) return 1;		// asize in (16, 32], insert into list1
	else if (payload <= 64) return 2;
	else if (payload <= 128) return 3;
	else if (payload <= 256) return 4;
	else if (payload <= 512) return 5;
	else if (payload <= 1024) return 6;
	else if (payload <= 2048) return 7;
	else if (payload <= 4096) return 8;
	else return 9;							// asize > 1<<12 Bytes, list9
} 

void *InsertBlock (char *p, size_t index) {	/* insert new block into beg list[index] */
	char *pbase = heap + 4*index*WSIZE;
	GETL(p) = pbase;			// Set prev-ptr on new block
	GETL(p + DSIZE) = GETL(pbase + DSIZE);	// Set next-ptr on new block
	GETL(GETL(pbase + DSIZE)) = p;			// Set prev-ptr on next block
	GETL(pbase + DSIZE) = p;	// Set next-ptr on head root 
	
	return (void*)p;			// return ptr to new block
}

void *DeleteBlock (char *p) {
	char *nextbp = GETL(p + DSIZE);
	char *prevbp = GETL(p); 
	GETL(prevbp + DSIZE) = nextbp;	// set next-ptr on prev block as next block
	GETL(nextbp) = prevbp;			// Set prev-ptr on next block as prev block
	
	return (void*)p;
}

/* coalesce - Boundary tag coalescing. Return ptr to coalesced block */
static void *coalesce (void *bp) {
	char *pante = PREV_BLKP(bp);
	char *ppost = NEXT_BLKP(bp);
	
    size_t prev_alloc = GET_ALLOC(FTRP(pante));
    size_t next_alloc = GET_ALLOC(HDRP(ppost));
    size_t size = GET_SIZE(HDRP(bp));
    size_t index;

    if (prev_alloc && next_alloc) {			/* Case 1 - both alloced */
    	index = GetIndex(size);
    	InsertBlock(bp, index);
        return bp;
    }

    else if (prev_alloc && !next_alloc) {	/* Case 2 - coalesce only post block */
    	DeleteBlock(ppost);				// post block is deleted from the list
		size += GET_SIZE(HDRP(ppost));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        
        index = GetIndex(size);
        InsertBlock(bp, index);
        return bp;
    }

    else if (!prev_alloc && next_alloc) {	/* Case 3 - coalesce only ante block */
    	DeleteBlock(pante);
    	size += GET_SIZE(HDRP(pante));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(pante), PACK(size, 0));
        
        index = GetIndex(size);
        InsertBlock(pante, index);
        return pante;
	}

    else {									/* Case 4 - coalesce all three blocks */
    	DeleteBlock(pante);
    	DeleteBlock(ppost);
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(pante), PACK(size, 0));
        PUT(FTRP(ppost), PACK(size, 0));
        
        index = GetIndex(size);
        InsertBlock(pante, index);
        return pante;
    }
    perror("WTF??? Why you reach here???");
    exit(0);
}

/* realloc - you may want to look at mm-naive.c */
void *realloc(void *ptr, size_t size) {
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if(ptr == NULL)
        return mm_malloc(size);

    newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if(newptr == NULL)
        return 0;

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(ptr));
    if(size < oldsize) 
    	oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* Free the old block. */
    mm_free(ptr);

    return newptr;
}


/* calloc - look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void *calloc (size_t nmemb, size_t size) {
    size_t bytes = nmemb*size;
    void *newptr;

    newptr = mm_malloc(bytes);
    memset(newptr, 0, bytes);
    return newptr;
}

/* Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void *p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/* Return whether the pointer is aligned. 
 * May be useful for debugging.
 */
static int aligned(const void *p) {
    return (size_t)ALIGN(p) == (size_t)p;
}

/* mm_checkheap */
void mm_checkheap(int lineno) {
}

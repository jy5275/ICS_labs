/* mm-seg.c, v1.0, 10 non-continuous segragated lists, shared pTail, best 81(42+39) [23:05, Sat]
 * mm-seg3.c, v1.3, 1.realloc extend on post block when asize > oldsize if possible
 * 					2.make 10 list accomadate larger blocks()
 *					(82 = 43 + 39, disappointing)
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
#define PUTL(p, valq)		(*(char**)(p) = (valp))	// p should be a ptr to char*, give that char* ptr a value
#define GET_SIZE(p)		(GET(p) & ~0x7)
#define GET_ALLOC(p)	(GET(p) & 0x1)

#define HDRP(bp)       ((char *)(bp) - WSIZE)
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) 
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) 
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) 

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */  
static char *pTail, *heap;
static int cheakheap = 1;
static int exhaustcheak = 0;
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)
//#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

int mm_init(void);
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
void *malloc(size_t size);
void free(void *bp);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
static size_t GetIndex (size_t payload);
static void *DeleteBlock (void *p);
static void *InsertBlock (void *p, size_t index);
void heap_error(int lineno, const char *str); 

/* Initialize: return -1 on error, 0 on success. */
int mm_init (void) {
    if ((heap_listp = mem_sbrk(24*DSIZE)) == (void *)-1) 
        return -1;
    heap = heap_listp;
    memset(heap, 0, 48*WSIZE);
    pTail = heap + 42*WSIZE;
    int i = 0;
    while (i < 10) {
    	GETL(heap + 4*WSIZE*i + DSIZE) = pTail;	// Set next-ptr of each head root
    	i++;
    }
    heap_listp = pTail;
    GETL(pTail + DSIZE) = NULL;
    PUT(pTail - WSIZE, PACK(24, 1));	/* Prologue header */ 
    PUT(pTail + 4*WSIZE, PACK(24, 1));	/* Prologue footer */ 
    PUT(pTail + 5*WSIZE, PACK(0, 1));	/* Epilogue header */
    
    if (cheakheap && exhaustcheak) {
		printf("----------------mm_init-----New Check begin--------------\n");
    	mm_checkheap(__LINE__);
    }
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) 
        return -1;
    return 0;
}

/* extend_heap - Extend heap with free block and return its block pointer */
static void *extend_heap(size_t words) {
    void *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE; 
    if ((long)(bp = mem_sbrk(size)) == -1)      // return NULL if mem_sbrk reaches maxlimit or input minus number
		return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */ 

    return coalesce(bp);
}

/* malloc */
void *malloc(size_t size) {   // size is payload, while asize is actual mem size to be alloc including HD&&FT
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    void *bp;
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
        if (cheakheap && exhaustcheak) {
        	printf("------------malloc(%ld)-----New Check begin--------------\n", size);
    		mm_checkheap(__LINE__);
    	}
        return bp;
    }

    /* No fit found. Get more memory then place the block */
    extendsize = MAX(asize, CHUNKSIZE);                 
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)  
        return NULL;
    place(bp, asize);
    
    if (cheakheap && exhaustcheak) {
		printf("------------malloc(%ld)-----New Check begin--------------\n", size);
    	mm_checkheap(__LINE__);
    }
    return bp;
} 

/* place - Place block of [asize] bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));  // csize - original size of this free block
    DeleteBlock(bp);
    if ((csize - asize) >= (3*DSIZE)) { // left >= 6 words empty ==> new free block
        PUT(HDRP(bp), PACK(asize, 1));  // Set head of new block
        PUT(FTRP(bp), PACK(asize, 1));	// Set foot of new block
        bp = NEXT_BLKP(bp);				// Add the left part into list (needn't coalesce)
        PUT(HDRP(bp), PACK(csize-asize, 0));  // Set head of left free block
        PUT(FTRP(bp), PACK(csize-asize, 0));  // Set foot of left free block
		size_t index = GetIndex(csize - asize);
		InsertBlock(bp, index);
	}
    else {								// left less than 6 free words - all include in new block
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/* find_fit - Find a fit for a block with asize bytes */
static void *find_fit (size_t asize) {
    /* First-fit search */
    size_t index = GetIndex(asize);
    char *pbase = heap + 4*index*WSIZE;
	char *it = GETL(pbase + DSIZE);		// it ptr to 1st free block on list[index]
	
	while (index < 10) {
		while (it) {		// Keep itenerating until meet NULL
			if (!GET_ALLOC(HDRP(it)) && GET_SIZE(HDRP(it)) >= asize)	// find!
				return (void*)it;
			it = GETL(it + DSIZE);		// Go to next free block
		}	// Search failed on list[index]. Go on next list!
		index++;
		pbase = heap + 4*index*WSIZE;
		it = GETL(pbase + DSIZE);
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
    if (cheakheap) {
    printf("------------free(%p)-----New Check begin--------------\n", bp);
    mm_checkheap(__LINE__);
    }
}

static size_t GetIndex (size_t payload) {
	if (payload <= 64) return 0;			// asize in [0, 32], insert into list0
	else if (payload <= 128) return 1;		// asize in (32, 64], insert into list1
	else if (payload <= 256) return 2;
	else if (payload <= 512) return 3;
	else if (payload <= 1024) return 4;
	else if (payload <= 2048) return 5;
	else if (payload <= 4096) return 6;
	else if (payload <= 8192) return 7;
	else if (payload <= 16384) return 8;
	else return 9;							// asize > 1<<13 Bytes, list9
} 

static void *InsertBlock (void *p, size_t index) {	/* insert new block into beg list[index] */
	char *pbase = heap + 4*index*WSIZE;
	GETL(p) = pbase;								// Set prev-ptr on new block
	GETL((char*)p + DSIZE) = GETL(pbase + DSIZE);	// Set next-ptr on new block
	GETL(GETL(pbase + DSIZE)) = p;					// Set prev-ptr on next block
	GETL(pbase + DSIZE) = p;						// Set next-ptr on head root 
	
	return p;			// return ptr to new block
}

static void *DeleteBlock (void *p) {
	char *nextbp = GETL((char*)p + DSIZE);
	char *prevbp = GETL(p); 
	GETL(prevbp + DSIZE) = nextbp;	// set next-ptr on prev block as next block
	GETL(nextbp) = prevbp;			// Set prev-ptr on next block as prev block
		
	return p;
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
    size_t oldsize, asize;
	void *newptr;

    if(size == 0) {		/* realloc(ptr, 0) == free(ptr), return NULL. */
        free(ptr);
        return 0;
    }

    if(ptr == NULL)		/* realloc(NULL, size) == malloc(size) */
        return malloc(size);
	
	/* New block should be no shorter than 6W to be useful. */
    if (size <= 2*DSIZE)	// asize is the total length of new block
    	asize = 3*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
         
    oldsize = GET_SIZE(HDRP(ptr));
	if (oldsize == asize) 
		newptr = ptr;
	
	else if (asize < oldsize) {				// Op within existing block
		if (oldsize - asize >= 3*DSIZE) {	// Add left part into list
			PUT(HDRP(ptr), PACK(asize, 1));	// Set head of new block
   		    PUT(FTRP(ptr), PACK(asize, 1));	// Set foot of new block
    	    ptr = NEXT_BLKP(ptr);			// Add the left part into list
    	    PUT(HDRP(ptr), PACK(oldsize-asize, 0));		// Set head of left free block
    	    PUT(FTRP(ptr), PACK(oldsize-asize, 0));		// Set foot of left free block
    	    coalesce(ptr);
			//size_t index = GetIndex(oldsize - asize);
			//InsertBlock(ptr, index);
			newptr = PREV_BLKP(ptr);
		}
		else newptr = ptr;
    	// Do nothing if (oldsize-asize) is too tiny to be replace
	}
	
	else {		// Need new mem space...
		void *ppost = (void*)NEXT_BLKP(ptr);
		long leftsize = GET_SIZE(HDRP(ppost)) + oldsize - asize;
		if (!GET_ALLOC(HDRP(ppost)) && leftsize >= 3*DSIZE) {	// Left a new free block
			DeleteBlock(ppost);
			PUT(HDRP(ptr), PACK(asize, 1));
			PUT(FTRP(ptr), PACK(asize, 1));
			ppost = NEXT_BLKP(ptr);
			PUT(HDRP(ppost), PACK((size_t)leftsize, 0));
			PUT(FTRP(ppost), PACK((size_t)leftsize, 0));
			coalesce(ppost);
			newptr = ptr;
		}
		else if (!GET_ALLOC(HDRP(ppost)) && leftsize >= 0) {	// Occupy post block
			DeleteBlock(ppost);
			PUT(HDRP(ptr), PACK(GET_SIZE(HDRP(ppost)) + oldsize, 1));
			PUT(FTRP(ptr), PACK(GET_SIZE(HDRP(ppost)) + oldsize, 1));
			newptr = ptr;
		}
		else {		// need new space when ppost was alloced or leftsize < 0
			newptr = malloc(size);
			if (newptr == NULL)
	        	return 0;
	    	memcpy(newptr, ptr, oldsize - 2*WSIZE);  // Copy old content into new block (oldsize - 2*W) Bytes
	    	free(ptr);
		}
	}
	if (cheakheap && exhaustcheak) {
		printf("-----realloc(%p, %ld):oldsize=%ld, asize=%ld---New Check begin----------\n", ptr, size, oldsize, asize);
		mm_checkheap(__LINE__);
	}
	return newptr;
}


/* calloc - look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void *calloc (size_t nmemb, size_t size) {    
    size = nmemb*size;
    size_t asize;
    void *newptr;
	/* New block should be no shorter than 6W to be useful. */
    if (size <= 2*DSIZE)
    	asize = 3*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
         
    newptr = malloc(asize);
    memset(newptr, 0, asize - 2*WSIZE);
    if (cheakheap && exhaustcheak) {
	printf("---------calloc(%ld, %ld)-----New Check begin-------------\n", nmemb, size);
    mm_checkheap(__LINE__);
    }
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
	char *bp = pTail;
	int isfree = 0;
	while (1) {
		if (!aligned((const void*)bp)) 
			heap_error(lineno, "Alignment error.");
		if (GET_SIZE(HDRP(bp)) == 0 && GET_ALLOC(HDRP(bp)))		// End of heap
			return;
		if (!in_heap((const void*)bp))
			heap_error(lineno, "Illegal mem ptr!");
		if ((GET_SIZE(HDRP(bp)) != GET_SIZE(FTRP(bp))) || GET_SIZE(HDRP(bp)) < 3*DSIZE)
			heap_error(lineno, "Block size error.");
		if (GET_ALLOC(HDRP(bp))) 
			isfree = 0;
		else {
			if (isfree)
				heap_error(lineno, "Two consecutive free blocks!");
			isfree = 1;
		}
		if (cheakheap) {
			printf("size=%d, alloc=%d, (0x%p : 0x%p)\n", GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)), HDRP(bp), FTRP(bp));
		}
		bp = NEXT_BLKP(bp);
	}
	//printf("malloclab not so hard. ");
}

void heap_error(int lineno, const char *str) {
	printf("line %d: %s\n", lineno, str);
	exit(0);
}








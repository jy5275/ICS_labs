/* mm-seg.c 	10 non-continuous segragated lists, shared pTail, (81 = 42+39) [23:05, Sat]
 * mm-seg3.c 	1.realloc extend on post block when asize > oldsize if possible
 * 				2.make 10 list accomadate larger blocks
 *				(82 = 43 + 39, expected)
 * mm-seg4.c	Cancel FT word on allocated blocks (83 = 45 + 39, disappointing)
 * mm-seg5.c	Sorted list, smallest in front (68 = 46 + 22, WTF) [Aborted]
 * mm-seg6.c	Compress ptr variable in heap into 4 Byte (87 = 48 + 39, well done)
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
/*
#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif
*/
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
#define PACK(size, pre, cur)	((size) | ((pre)<<1) | (cur))
/*
#define GET(p)				(*(unsigned*)(p))
#define GETL(p)				(*(char**)(p))		// p should be a ptr to char*, return that char* ptr
#define PUT(p, val)			(*(unsigned*)(p) = (val))
#define PUTL(p, valq)		(*(char**)(p) = (valp))	// p should be a ptr to char*, give that char* ptr a value
#define GET_SIZE(p)			(GET(p) & ~0x7)
#define GET_ALLOC(p)		(GET(p) & 0x1)
#define GET_PREV_ALLOC(p)	((GET(p) & 0x2)>>1)
#define NEXT_NODE(bp)		(GETL((char*)(bp) + DSIZE))

#define HDRP(bp)       ((char *)(bp) - WSIZE)
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) 
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(HDRP(bp))) 
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))	// valid only when pre is free (FT records length)
*/
#define _GET(_p)			(*(unsigned*)(heap+(_p)))	// get an unsigned integer
#define _PUT(_p, val)		(*(unsigned*)(heap+(_p)) = (val))
#define _GET_SIZE(_p)		(_GET(_p) & ~0x7)			// p is an unsigned integer
#define _GET_ALLOC(_p)		(_GET(_p) & 0x1)				// p
#define _GET_PREV_ALLOC(_p)	((_GET(_p) & 0x2)>>1)		//

#define _HDRP(_bp)       ((_bp) - WSIZE)
#define _FTRP(_bp)       ((_bp) + _GET_SIZE(_HDRP(_bp)) - DSIZE) 
#define _NEXT_BLKP(_bp)  ((_bp) + _GET_SIZE(_HDRP(_bp))) 
#define _PREV_BLKP(_bp)  ((_bp) - _GET_SIZE((_bp) - DSIZE))	// valid only when pre is free (FT records length)

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */
static unsigned _heap_listp = 0, _heap = 0, _pTail; 
static char *pTail, *heap;
static int checkheap = 0;
static int exhaustcheck = 1;
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)
//#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

int mm_init(void);
static unsigned _extend_heap(size_t words);
static void place(unsigned _bp, size_t asize);
static unsigned _find_fit(size_t asize);
static unsigned _coalesce(unsigned _bp);
void *malloc(size_t size);
void free(void *bp);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
static size_t GetIndex (size_t payload);
static unsigned _DeleteBlock (unsigned _p);
static unsigned _InsertBlock (unsigned _p, size_t index);
void heap_error(int lineno, const char *str); 

/* Initialize: return -1 on error, 0 on success. */
int mm_init (void) {
    if ((heap_listp = mem_sbrk(13*DSIZE)) == (void *)-1) 
        return -1;
    heap = heap_listp;
    memset(heap, 0, 24*WSIZE);
    pTail = heap + 11*DSIZE;
    _pTail = 11*DSIZE;
    int i = 0;
    while (i < 10) {
    	_GET(2*WSIZE*i + WSIZE) = _pTail;	// Set next-ptr of each head root
    	i++;
    }
    heap_listp = pTail;
    _heap_listp = _pTail;
    _GET(_pTail + WSIZE) = 0;
    _PUT(_pTail - WSIZE, PACK(16, 1, 1));	/* Prologue header */ 
    _PUT(_pTail + 2*WSIZE, PACK(16, 1, 1));	/* Prologue footer */ 
    _PUT(_pTail + 3*WSIZE, PACK(0, 1, 1));	/* Epilogue header */

    if (checkheap) {
    	if (exhaustcheck)
    		{printf("\n-------------mm_init-------New Check begin--------------\n");}
		mm_checkheap(__LINE__);
    }
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (_extend_heap(CHUNKSIZE/WSIZE) == 0) 
        return -1;
    return 0;
}

/* extend_heap - Extend heap with free block and return its block pointer */
inline static unsigned _extend_heap(size_t words) {
    unsigned _bp;
    void *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE; 
    if ((long)(bp = mem_sbrk(size)) == -1)      // return 0 if mem_sbrk reaches maxlimit or input minus number
		return 0;
	_bp = (char*)bp - heap;
    
    /* Initialize free block header/footer and the epilogue header */
	int preal = _GET_PREV_ALLOC(_HDRP(_bp));
    _PUT(_HDRP(_bp), PACK(size, preal, 0));		/* Free block header */
    _PUT(_FTRP(_bp), PACK(size, preal, 0));		/* Free block footer */
    _PUT(_HDRP(_NEXT_BLKP(_bp)), PACK(0, 0, 1));	/* New epilogue header */
    
    return _coalesce(_bp);
}

/* malloc */
void *malloc(size_t size) {   // size is payload, while asize is actual mem size to be alloc including HD&&FT
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    void *bp;
    unsigned _bp;
    if (heap_listp == 0)
        mm_init();
        
    /* Ignore spurious requests */
    if (size == 0)	return NULL;

    /* Block should be no shorter than 4W to be useful. */
    if (size <= 3*WSIZE)
    	asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + 11) / DSIZE); 

    /* Search the free list for a fit */
    if ((_bp = _find_fit(asize)) != 0) {	// bp is the ptr that point to availiable free block
        place(_bp, asize);
        if (checkheap) {
    		if (exhaustcheck)
    			{printf("\n---------malloc(%ld)-----New Check begin--------------\n", size);}
			mm_checkheap(__LINE__);
    	}
    	bp = (void*)(heap + _bp);
        return bp;
    }

    /* No fit found. Get more memory then place the block */
    extendsize = MAX(asize, CHUNKSIZE);                 
    if ((_bp = _extend_heap(extendsize/WSIZE)) == 0)  
        return NULL;
    
    place(_bp, asize);
    
    if (checkheap) {
    	if (exhaustcheck)
    		{printf("\n----------malloc(%ld)-----New Check begin--------------\n", size);}
		mm_checkheap(__LINE__);
    }
    bp = (void*)(heap + _bp);
    return bp;
} 

/* place - Place block of [asize] bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
inline static void place(unsigned _bp, size_t asize) {
    size_t csize = _GET_SIZE(_HDRP(_bp));		// csize - original size of this free block
    unsigned _ppost;
    _DeleteBlock(_bp);
    if ((csize - asize) >= (2*DSIZE)) { 		// left >= 4 words empty ==> new free block
        _PUT(_HDRP(_bp), PACK(asize, 1, 1));		// Set head of new block
        //PUT(FTRP(bp), PACK(asize, 1, 1));		// Set foot of new block
        _ppost = _NEXT_BLKP(_bp);						// Add the left part into list (needn't coalesce)
        _PUT(_HDRP(_ppost), PACK(csize-asize, 1, 0));	// Set head of left free block
        _PUT(_FTRP(_ppost), PACK(csize-asize, 1, 0));	// Set foot of left free block
		size_t index = GetIndex(csize - asize);
		_InsertBlock(_ppost, index);
	}
    else {								// left less than 6 free words - all include in new block
        _PUT(_HDRP(_bp), PACK(csize, 1, 1));
        //PUT(FTRP(bp), PACK(csize, 1, 1));
        _ppost = _NEXT_BLKP(_bp);
        _PUT(_HDRP(_ppost), _GET(_HDRP(_ppost)) | 2);		// Set prev-alloc bit on post block as alloc (|[10])
    }
}

/* find_fit - Find a fit for a block with asize bytes */
inline static unsigned _find_fit (size_t asize) {
    /* First-fit search */
    size_t index = GetIndex(asize);
    unsigned _pbase = _heap + 2*index*WSIZE;
	unsigned _it = _GET(_pbase + WSIZE);		// it ptr to 1st free block on list[index]
	
	while (index < 10) {
		while (_it) {		// Keep itenerating until meet NULL
			if (!_GET_ALLOC(_HDRP(_it)) && _GET_SIZE(_HDRP(_it)) >= asize)	// find!
				return _it;
			_it = _GET(_it + WSIZE);		// Go to next free block
		}	// Search failed on list[index]. Go on next list!
		index++;
		_pbase = _heap + 2*index*WSIZE;
		_it = _GET(_pbase + WSIZE);
	}
	
    return 0; /* No fit */
}

/* free - Free a block */
void free(void *bp) {
    if (bp == NULL) 
        return;
    unsigned _bp = (char*)bp - heap, _ppost;
    //void *ppost;
    size_t size = _GET_SIZE(_HDRP(_bp));
    if (heap_listp == 0)
        mm_init();
    _ppost = _NEXT_BLKP(_bp);    
	int preal = _GET_PREV_ALLOC(_HDRP(_bp));
    _PUT(_HDRP(_bp), PACK(size, preal, 0));
    _PUT(_FTRP(_bp), PACK(size, preal, 0));
    
    //printf("bp: %p ----- ppost: %p\n", bp, ppost);
    _PUT(_HDRP(_ppost), _GET(_HDRP(_ppost)) & (~2));	// Set prev-alloc bit on post block as free (&[11..1101])
    //printf("GET_PREV_ALLOC(%p)=%d\n", HDRP(ppost), GET_PREV_ALLOC(HDRP(ppost)));
    _coalesce(_bp);		// coalesce if previous or next block is free
    
    if (checkheap) {
    	if (exhaustcheck)
    		{printf("\n------------free(%p)-----New Check begin--------------\n", bp);}
		mm_checkheap(__LINE__);
    }
}

inline static size_t GetIndex (size_t payload) {
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

inline static unsigned _InsertBlock (unsigned _p, size_t index) {	/* insert new block into beg list[index] */
	unsigned _pbase = _heap + 2*index*WSIZE;
	_GET(_p) = _pbase;							// Set prev-ptr on new block
	_GET(_p + WSIZE) = _GET(_pbase + WSIZE);	// Set next-ptr on new block
	_GET(_GET(_pbase + WSIZE)) = _p;			// Set prev-ptr on next block
	_GET(_pbase + WSIZE) = _p;					// Set next-ptr on head root 
	
	return _p;		// return ptr to new block
}

inline static unsigned _DeleteBlock (unsigned _p) {
	unsigned _nextbp = _GET(_p + WSIZE);
	unsigned _prevbp = _GET(_p); 
	_GET(_prevbp + WSIZE) = _nextbp;	// set next-ptr on prev block as next block
	_GET(_nextbp) = _prevbp;			// Set prev-ptr on next block as prev block
		
	return _p;
}

/* coalesce - Boundary tag coalescing. Return ptr to coalesced block */
inline static unsigned _coalesce (unsigned _bp) {
	unsigned _pante;
	unsigned _ppost = _NEXT_BLKP(_bp);
	
    size_t prev_alloc = _GET_PREV_ALLOC(_HDRP(_bp));
    size_t next_alloc = _GET_ALLOC(_HDRP(_ppost));
    //printf("GET_PREV_ALLOC(%p)=%d\n", HDRP(bp), GET_PREV_ALLOC(HDRP(bp)));
    //printf("bp=%p, ppost=%p, prev_alloc=%ld, next_alloc=%ld\n", bp, ppost, prev_alloc, next_alloc);
    size_t size = _GET_SIZE(_HDRP(_bp));
    size_t index;

    if (prev_alloc && next_alloc) {			/* Case 1 - both alloced */
    	index = GetIndex(size);
    	_InsertBlock(_bp, index);
        //printf("bp1: %p\n", bp);
        return _bp;
    }

    else if (prev_alloc && !next_alloc) {	/* Case 2 - coalesce only post block */
    	_DeleteBlock(_ppost);				// post block is deleted from the list
		size += _GET_SIZE(_HDRP(_ppost));
        _PUT(_HDRP(_bp), PACK(size, 1, 0));
        _PUT(_FTRP(_bp), PACK(size, 1, 0));
        
        index = GetIndex(size);
        _InsertBlock(_bp, index);
        
        //printf("bp2: %p\n", bp);
        return _bp;
    }

    else if (!prev_alloc && next_alloc) {	/* Case 3 - coalesce only ante block */
    	_pante = _PREV_BLKP(_bp);
    	_DeleteBlock(_pante);
    	size += _GET_SIZE(_HDRP(_pante));
        _PUT(_FTRP(_bp), PACK(size, 1, 0));
        _PUT(_HDRP(_pante), PACK(size, 1, 0));
        
        index = GetIndex(size);
        _InsertBlock(_pante, index);
        //printf("ante1: %p\n", pante);
        return _pante;
	}

    else {									/* Case 4 - coalesce all three blocks */
    	_pante = _PREV_BLKP(_bp);
    	_DeleteBlock(_pante);
    	_DeleteBlock(_ppost);
        size += _GET_SIZE(_HDRP(_pante)) + _GET_SIZE(_HDRP(_ppost));
        _PUT(_HDRP(_pante), PACK(size, 1, 0));
        _PUT(_FTRP(_ppost), PACK(size, 1, 0));
        
        index = GetIndex(size);
        _InsertBlock(_pante, index);
        
        //printf("ante2: %p\n", pante);
        return _pante;
    }
    perror("WTF??? Why you reach here???");
    exit(0);
}

/* realloc - you may want to look at mm-naive.c */
void *realloc(void *ptr, size_t size) { 
	unsigned _ptr = (char*)ptr - heap, _newptr, _ppost, _pppost;
    size_t oldsize, asize;
	void *newptr;//, *ppost, *pppost;

    if(size == 0) {		/* realloc(ptr, 0) == free(ptr), return NULL. */
        free(ptr);
        return 0;
    }

    if(ptr == NULL)		/* realloc(NULL, size) == malloc(size) */
        return malloc(size);
	
	/* New block should be no shorter than 6W to be useful. */
    if (size <= 3*WSIZE)		// asize is the total length of new block
    	asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + 11) / DSIZE); 
    
    oldsize = _GET_SIZE(_HDRP(_ptr));
	if (oldsize == asize) { 
		_newptr = _ptr;
		newptr = heap + _newptr;
	}
	else if (asize < oldsize) {					// Op within existing block
		if (oldsize - asize >= 2*DSIZE) {		// Add left part into list
			int preal = _GET_PREV_ALLOC(_HDRP(_ptr));
			_PUT(_HDRP(_ptr), PACK(asize, preal, 1));		// Set head of new block
   		    //PUT(FTRP(ptr), PACK(asize, preal, 1));		// Set foot of new block
    	    _ppost = _NEXT_BLKP(_ptr);						// Add the left part into list
    	    _PUT(_HDRP(_ppost), PACK(oldsize-asize, 1, 0));	// Set head of left free block
    	    _PUT(_FTRP(_ppost), PACK(oldsize-asize, 1, 0));	// Set foot of left free block
    	    _pppost = _NEXT_BLKP(_ppost);
    	    _PUT(_HDRP(_pppost), _GET(_HDRP(_pppost)) & (~2));// Set prev-alloc bit on post block as free (&[1..1101])
    	    _coalesce(_ppost);
			_newptr = _ptr;
		}
		else _newptr = _ptr;
    	// Do nothing if (oldsize-asize) is too tiny to be replace
    	newptr = heap + _newptr;
	}
	
	else {		// Need new mem space...
		_ppost = _NEXT_BLKP(_ptr);
		long leftsize = _GET_SIZE(_HDRP(_ppost)) + oldsize - asize;
		if (!_GET_ALLOC(_HDRP(_ppost)) && leftsize >= 2*DSIZE) {	// Left a new free block
			_DeleteBlock(_ppost);
			int preal = _GET_PREV_ALLOC(_HDRP(_ptr));
			_PUT(_HDRP(_ptr), PACK(asize, preal, 1));		// Process valid block
			//PUT(FTRP(ptr), PACK(asize, preal, 1));
			_ppost = _NEXT_BLKP(_ptr);
			_PUT(_HDRP(_ppost), PACK((size_t)leftsize, 1, 0));
			_PUT(_FTRP(_ppost), PACK((size_t)leftsize, 1, 0));
			_coalesce(_ppost);
			_newptr = _ptr;
			newptr = heap + _newptr;
		}
		else if (!_GET_ALLOC(_HDRP(_ppost)) && leftsize >= 0) {	// Occupy post block
			_DeleteBlock(_ppost);
			int preal = _GET_PREV_ALLOC(_HDRP(_ptr));
			_PUT(_HDRP(_ptr), PACK(_GET_SIZE(_HDRP(_ppost)) + oldsize, preal, 1));
			//PUT(FTRP(ptr), PACK(GET_SIZE(HDRP(ppost)) + oldsize, preal, 1));
			_ppost = _NEXT_BLKP(_ptr);
			_PUT(_HDRP(_ppost), _GET(_HDRP(_ppost)) | 2);	// Set prev-alloc bit on post as alloc (|[10])
			_newptr = _ptr;
			newptr = heap + _newptr;
		}
		else {		// need new space when ppost was alloced or leftsize < 0
			newptr = malloc(size);
			if (newptr == NULL)
	        	return 0;
	    	memcpy(newptr, ptr, oldsize - WSIZE);  // Copy old content into new block (oldsize - 2*W) Bytes
	    	free(ptr);
		}
	}
	if (checkheap) {
    	if (exhaustcheck)
    		{printf("\n---------realloc(%p, %ld)-----New Check begin------------\n", ptr, size);}
		mm_checkheap(__LINE__);
    }
	return (void*)newptr;
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
    if (size <= 3*WSIZE)		// asize is the total length of new block
    	asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + 11) / DSIZE); 
         
    newptr = malloc(asize);
    memset(newptr, 0, asize - WSIZE);
    if (checkheap) {
    	if (exhaustcheck)
    		{printf("\n-----------calloc(%ld, %ld)-----New Check begin--------------\n", nmemb, size);}
		mm_checkheap(__LINE__);
    }
    return newptr;
}

/* Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
inline static int in_heap(const void *p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/* Return whether the pointer is aligned. 
 * May be useful for debugging.
 */
inline static int aligned(const void *p) {
    return (size_t)ALIGN(p) == (size_t)p;
}

/* mm_checkheap */
void mm_checkheap(int lineno) {
	unsigned _bp = _pTail;
	//char *bp = pTail;
	int isfree = 0;
	
	while (1) {
		if (!aligned((const void*)(heap + _bp))) 
			heap_error(lineno, "Alignment error.");
		if (_GET_SIZE(_HDRP(_bp)) == 0 && _GET_ALLOC(_HDRP(_bp))) {		// End of heap
			return;
		}
		if (!in_heap((const void*)(heap + _bp)))
			heap_error(lineno, "Illegal mem ptr!");
		if (_GET_SIZE(_HDRP(_bp)) < 2*DSIZE)
			heap_error(lineno, "Block size error.");
		if (_GET_ALLOC(_HDRP(_bp))) 
			isfree = 0;
		else {
			if (isfree)
				heap_error(lineno, "Two consecutive free blocks!");
			isfree = 1;
		}
		if (exhaustcheck) {
			printf("size=%d, alloc=%d, (%d : %d)\n", _GET_SIZE(_HDRP(_bp)), _GET_ALLOC(_HDRP(_bp)), _HDRP(_bp), _FTRP(_bp));
		}
		_bp = _NEXT_BLKP(_bp);
	}
	//printf("malloclab not so hard. ");
}

void heap_error(int lineno, const char *str) {
	printf("line %d: %s\n", lineno, str);
	exit(0);
}








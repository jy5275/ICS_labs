/* 蒋衍 1600013239 出品
 * 
 * mm-seg.c 	10 non-continuous segragated lists, shared pTail, (81 = 42+39) [23:05, Sat]
 * mm-seg3.c 	1.realloc extend on post block when asize > oldsize if possible
 * 				2.make 10 list accomadate larger blocks
 *				(82 = 43 + 39, expected)
 * mm-seg4.c	Cancel FT word on allocated blocks (83 = 45 + 39, disappointing)
 * mm-seg5.c	Sorted list, smallest in front (68 = 46 + 22, WTF) [Aborted]
 * mm-seg6.c	Compress ptr variable in heap into 4 Byte (87 = 48 + 39, well done)
 * mm-seg7.c	[compressed ptr] + [sorted list] (88 = 49 + 39, limit of seg list?) [22:52, Sun]
 * mm-seg8.c	Add some comment, replace pTail with heap_listp
 * 
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 * 
 *
 * Description of mm-seg8.c:
 * 	10 sorted segragated lists linked by 32-bit pseudo-pointer (unsigned int), best-fit search strategy
 *	Each list begin with a 2-word-size head node, 10 heads are put in the beginning of heap.
 *	All lists share a common tail node (prologue block).
 *  The last/second last bits of Head word of each block record whether this/anterior block is allocated.
 *
 *	In principle, Foot word of each block should be the same as Head, but the we only utilize size info,
 *	 so mess of the last 2 bits doesn't matter.
 *	Similarly, Head/tail of each list also has both pseudo-prev-ptr and pseudo-next-ptr, but we only 
 *	 utilize next-ptr of head and tail, so mess of prev-ptr also doesn't matter.
 *
 *
 * a free block = Head(4B) + pseudo-prev-ptr(4B) + pseudo-next-ptr(4B) + useless part + Foot(4B)
 *  ————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
 * |						|						|						|							|						|
 * |	 	Head (4B)		|  pseudo-prev-ptr (4B)	|  pseudo-next-ptr (4B)	|     useless...(0 ~ inf B)	|		Foot (4B)		|
 * |  size|prev-alloc|0		|						|						|							|    same with Head		|
 * |						|						|						|							|						|
 *  ————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
 *
 *
 * an allocated block = Head(4B) + content of the block
 *  ————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
 * |						|																									|
 * |	 	Head (4B)		|				content...(12 ~ inf B)																|
 * |   size|prev-alloc|1	|																									|
 * |						|																									|
 *  ————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
 *
 *
 *
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
#define PACK(size, pre, cur)	((size) | ((pre)<<1) | (cur))

#define _GET(_p)			(*(unsigned*)(heap+(_p)))	/* get an unsigned integer */
#define _PUT(_p, val)		(*(unsigned*)(heap+(_p)) = (val))
#define _GET_SIZE(_p)		(_GET(_p) & ~0x7)
#define _GET_ALLOC(_p)		(_GET(_p) & 0x1)
#define _GET_PREV_ALLOC(_p)	((_GET(_p) & 0x2)>>1)

#define _HDRP(_bp)       ((_bp) - WSIZE)
#define _FTRP(_bp)       ((_bp) + _GET_SIZE(_HDRP(_bp)) - DSIZE) 
#define _NEXT_BLKP(_bp)  ((_bp) + _GET_SIZE(_HDRP(_bp))) 

/* valid only when anterior block is free */
#define _PREV_BLKP(_bp)  ((_bp) - _GET_SIZE((_bp) - DSIZE))

/* Global variables */
static char *heap_listp = 0;	/* Pointer to prologue block */
static unsigned _heap_listp = 0, _heap = 0;		/* pseudo-ptr */
static char *heap;
static int checkheap = 0;		// Optionally call mm_checkheap at the end of each operation
static int exhaustcheck = 1;	// Optionally print info of each block

#define ALIGNMENT 8
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)

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
    heap_listp = heap + 11*DSIZE;
    _heap_listp = 11*DSIZE;
    
    /* next-ptr on each block ptr to prologue in the beginning */
    int i = 0;
    while (i < 10) {
    	_GET(2*WSIZE*i + WSIZE) = _heap_listp;
    	i++;
    }
    _GET(_heap_listp + WSIZE) = 0;					/* Prologue next-ptr*/
    _PUT(_heap_listp - WSIZE, PACK(16, 1, 1));		/* Prologue header */ 
    _PUT(_heap_listp + 2*WSIZE, PACK(16, 1, 1));	/* Prologue footer */ 
    _PUT(_heap_listp + 3*WSIZE, PACK(0, 1, 1));		/* Epilogue header */

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
static unsigned _extend_heap(size_t words) {
    unsigned _bp;
    void *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE; 
    
    /* return 0 if mem_sbrk reaches maxlimit or input is a minus number */
    if ((long)(bp = mem_sbrk(size)) == -1)
		return 0;
	_bp = (char*)bp - heap;
    
    /* Initialize free block header/footer and the epilogue header */
	int preal = _GET_PREV_ALLOC(_HDRP(_bp));
    _PUT(_HDRP(_bp), PACK(size, preal, 0));			/* Free block header */
    _PUT(_FTRP(_bp), PACK(size, preal, 0));			/* Free block footer */
    _PUT(_HDRP(_NEXT_BLKP(_bp)), PACK(0, 0, 1));	/* New epilogue header */
    
    return _coalesce(_bp);
}

/* malloc */
void *malloc(size_t size) {
    /* size is payload, while asize is actual mem size to be alloc */
    size_t asize;
    size_t extendsize;	/* Amount to extend heap if no fit */
    void *bp;
    unsigned _bp;
    if (heap_listp == 0)
        mm_init();
        
    /* Ignore spurious requests */
    if (size == 0)	return NULL;

    /* Block should be aligned 8B and no shorter than 4W to be useful */
    if (size <= 3*WSIZE)
    	asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + 11) / DSIZE); 

    /* Search the free list for a fit */
    if ((_bp = _find_fit(asize)) != 0) {
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

/* place - Place block of [asize] bytes at start of free block _bp 
 *         and split if the remained would be at least minimum block size
 */
static void place(unsigned _bp, size_t asize) {
	/* csize - original size of this free block */
    size_t csize = _GET_SIZE(_HDRP(_bp));
    unsigned _ppost;
    _DeleteBlock(_bp);
    
    /* left >= 4 words empty ==> new free block  */
    if ((csize - asize) >= (2*DSIZE)) {
        _PUT(_HDRP(_bp), PACK(asize, 1, 1));
        //PUT(FTRP(bp), PACK(asize, 1, 1));
        _ppost = _NEXT_BLKP(_bp);
        _PUT(_HDRP(_ppost), PACK(csize-asize, 1, 0));
        _PUT(_FTRP(_ppost), PACK(csize-asize, 1, 0));
		size_t index = GetIndex(csize - asize);
		
		/* Add the left part into list (needn't coalesce) */
		_InsertBlock(_ppost, index);
	}
    else {		// left less than 6 free words
        _PUT(_HDRP(_bp), PACK(csize, 1, 1));
        //PUT(FTRP(bp), PACK(csize, 1, 1));
        _ppost = _NEXT_BLKP(_bp);
        _PUT(_HDRP(_ppost), _GET(_HDRP(_ppost)) | 2);
    }
}

/* find_fit - Find a fit for a block with asize bytes */
static unsigned _find_fit (size_t asize) {
    size_t index = GetIndex(asize);				/* list num */
    unsigned _pbase = _heap + 2*index*WSIZE;
	unsigned _it = _GET(_pbase + WSIZE);
	
	/* best-fit strategy */
	while (index < 10) {
		/* Keep itenerating until meet NULL */
		while (_it) {
			if (!_GET_ALLOC(_HDRP(_it)) && _GET_SIZE(_HDRP(_it)) >= asize)
				return _it;
			_it = _GET(_it + WSIZE);
		}
		
		/* Search failed on current list. Go on searching in next list */
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
    size_t size = _GET_SIZE(_HDRP(_bp));
    if (heap_listp == 0)
        mm_init();
        
    _ppost = _NEXT_BLKP(_bp);
	int preal = _GET_PREV_ALLOC(_HDRP(_bp));
    _PUT(_HDRP(_bp), PACK(size, preal, 0));	
    _PUT(_FTRP(_bp), PACK(size, preal, 0));
    
    /* Set prev-alloc bit on post block as free (&[11..1101]) */
    _PUT(_HDRP(_ppost), _GET(_HDRP(_ppost)) & (~2));
    
    _coalesce(_bp);
    if (checkheap) {
    	if (exhaustcheck)
    		{printf("\n------------free(%p)-----New Check begin--------------\n", bp);}
		mm_checkheap(__LINE__);
    }
}

/* Get list num that block with size s fit into */
inline static size_t GetIndex (size_t s) {
	if (s <= 64) return 0;			// asize in [0, 32], insert into list0
	else if (s <= 128) return 1;	// asize in (32, 64], insert into list1
	else if (s <= 256) return 2;
	else if (s <= 512) return 3;
	else if (s <= 1024) return 4;
	else if (s <= 2048) return 5;
	else if (s <= 4096) return 6;
	else if (s <= 8192) return 7;
	else if (s <= 16384) return 8;
	else return 9;					// asize > 1<<13 Bytes, list9
} 

/* insert new block into proper pos of sorted list[index] */
static unsigned _InsertBlock (unsigned _p, size_t index) {
	unsigned _pbase = _heap + index*DSIZE;
	unsigned _it = _pbase;
	size_t len = _GET_SIZE(_HDRP(_p));
	
	/* find the proper pos for new block */
	while (_GET(_it + WSIZE) != _heap_listp && 
		_GET_SIZE(_HDRP(_GET(_it + WSIZE))) < len)
		_it = _GET(_it + WSIZE);
	 
	/* Then insert new block behind the block that _it ptr to */
	_GET(_p) = _it;							// Set prev-ptr on new block
	_GET(_p + WSIZE) = _GET(_it + WSIZE);	// Set next-ptr on new block
	_GET(_GET(_it + WSIZE)) = _p;			// Set prev-ptr on next block
	_GET(_it + WSIZE) = _p;					// Set next-ptr on head root 
	
	return _p;			// return ptr to new block
}

/* Delete block _p from the list */
inline static unsigned _DeleteBlock (unsigned _p) {
	unsigned _nextbp = _GET(_p + WSIZE);
	unsigned _prevbp = _GET(_p); 
	_GET(_prevbp + WSIZE) = _nextbp;	
	_GET(_nextbp) = _prevbp;
		
	return _p;
}

/* coalesce - Boundary tag coalescing. Return ptr to coalesced block */
static unsigned _coalesce (unsigned _bp) {
	unsigned _pante;
	unsigned _ppost = _NEXT_BLKP(_bp);
	
    size_t prev_alloc = _GET_PREV_ALLOC(_HDRP(_bp));
    size_t next_alloc = _GET_ALLOC(_HDRP(_ppost));
    size_t size = _GET_SIZE(_HDRP(_bp));
    size_t index;
	
	/* Case 1 - both alloced */
    if (prev_alloc && next_alloc) {
    	index = GetIndex(size);
    	_InsertBlock(_bp, index);
        //printf("bp1: %p\n", bp);
        return _bp;
    }
	
	/* Case 2 - coalesce only post block */
    else if (prev_alloc && !next_alloc) {
    	_DeleteBlock(_ppost);
		size += _GET_SIZE(_HDRP(_ppost));
        _PUT(_HDRP(_bp), PACK(size, 1, 0));
        _PUT(_FTRP(_bp), PACK(size, 1, 0));
        
        index = GetIndex(size);
        _InsertBlock(_bp, index);
        
        //printf("bp2: %p\n", bp);
        return _bp;
    }

	/* Case 3 - coalesce only ante block */
    else if (!prev_alloc && next_alloc) {
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
	
	/* Case 4 - coalesce all three blocks */
    else {
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
    if (size <= 3*WSIZE)	/* asize is the total length of new block */
    	asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + 11) / DSIZE); 
    
    oldsize = _GET_SIZE(_HDRP(_ptr));
	if (oldsize == asize) { 
		_newptr = _ptr;
		newptr = heap + _newptr;
	}
	
	/* Operation within existing block */
	else if (asize < oldsize) {
		/* New free block emerges */
		if (oldsize - asize >= 2*DSIZE) {
			int preal = _GET_PREV_ALLOC(_HDRP(_ptr));
			_PUT(_HDRP(_ptr), PACK(asize, preal, 1));
   		    //PUT(FTRP(ptr), PACK(asize, preal, 1));
    	    _ppost = _NEXT_BLKP(_ptr);	
    	    _PUT(_HDRP(_ppost), PACK(oldsize-asize, 1, 0));	
    	    _PUT(_FTRP(_ppost), PACK(oldsize-asize, 1, 0));
    	    _pppost = _NEXT_BLKP(_ppost);
    	    _PUT(_HDRP(_pppost), _GET(_HDRP(_pppost)) & (~2));
    	    _coalesce(_ppost);
			_newptr = _ptr;
		}
		/* Do nothing if (oldsize-asize) is too tiny to be emerge */
		else _newptr = _ptr;
    	
    	newptr = heap + _newptr;
	}
	
	/* Need some more mem space... */
	else {
		_ppost = _NEXT_BLKP(_ptr);
		long leftsize = _GET_SIZE(_HDRP(_ppost)) + oldsize - asize;
		
		/* in situ && new free block */
		if (!_GET_ALLOC(_HDRP(_ppost)) && leftsize >= 2*DSIZE) {
			_DeleteBlock(_ppost);
			int preal = _GET_PREV_ALLOC(_HDRP(_ptr));
			_PUT(_HDRP(_ptr), PACK(asize, preal, 1));
			//PUT(FTRP(ptr), PACK(asize, preal, 1));
			_ppost = _NEXT_BLKP(_ptr);
			_PUT(_HDRP(_ppost), PACK((size_t)leftsize, 1, 0));
			_PUT(_FTRP(_ppost), PACK((size_t)leftsize, 1, 0));
			_coalesce(_ppost);
			_newptr = _ptr;
			newptr = heap + _newptr;
		}
		
		/* in situ && no new free block */
		else if (!_GET_ALLOC(_HDRP(_ppost)) && leftsize >= 0) {
			_DeleteBlock(_ppost);
			int preal = _GET_PREV_ALLOC(_HDRP(_ptr));
			_PUT(_HDRP(_ptr), PACK(_GET_SIZE(_HDRP(_ppost)) + oldsize, preal, 1));
			//PUT(FTRP(ptr), PACK(GET_SIZE(HDRP(ppost)) + oldsize, preal, 1));
			_ppost = _NEXT_BLKP(_ptr);
			_PUT(_HDRP(_ppost), _GET(_HDRP(_ppost)) | 2);
			_newptr = _ptr;
			newptr = heap + _newptr;
		}
		
		/* ex situ */
		else {
			newptr = malloc(size);
			if (newptr == NULL)
	        	return 0;
	    	memcpy(newptr, ptr, oldsize - WSIZE);
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
    
    /* New block should be no shorter than 4W to be useful. */
    if (size <= 3*WSIZE)
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

/* Return whether the pointer is in the heap. */
inline static int in_heap(const void *p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/* Return whether the pointer is aligned. 
 * May be useful for debugging.
 */
inline static int aligned(const void *p) {
    return (size_t)ALIGN(p) == (size_t)p;
}

/* mm_checkheap, check if:
 *  all blocks are correctly aligned
 *  ptr on each block fall within boundary
 *  no two consecutive free blocks in the heap
 *  size of each block no shorter than mininum size (2*DSISE)
 *  all next/prev-ptr are consistent
 *  all blocks in lists are free
 *  all blocks in each list bucket fall within bucket size range
 * if (exhaustcheck), print block info of each visited blocks
 */
void mm_checkheap(int lineno) {
	unsigned _bp = _heap_listp, _it = _heap;
	int isfree = 0, i = 0;
	size_t len_min, len_max = 32;
	
	/* iterate through the heap */
	while (1) {
		if (!aligned((const void*)(heap + _bp))) 
			heap_error(lineno, "Alignment error.");
		if (_GET_SIZE(_HDRP(_bp)) == 0 && _GET_ALLOC(_HDRP(_bp))) 	// End of heap
			return;
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
			printf("size=%d, alloc=%d, (%d : %d)\n", _GET_SIZE(_HDRP(_bp)),
				 _GET_ALLOC(_HDRP(_bp)), _HDRP(_bp), _FTRP(_bp));
		}
		_bp = _NEXT_BLKP(_bp);	// Go on checking next block
	}
	
	/* Tranverse each list */
	for (i = 0; i < 10; i++) {
		len_min = (len_min << 1);
		if (i == 9)		len_max = 1 << 29, len_min = len_min << 1;
		else if (i == 1)	len_max = len_max << 1, len_min = 64;
		else 			len_max = len_max << 1, len_min = len_min << 1;
		
		_it = _heap + DSIZE*i;
		while (_GET(_it + WSIZE) != 0) {	// Break if _it ptr to prologue
			if (_GET_ALLOC(_HDRP(_it)))
				heap_error(lineno, "Allocted block in list!");
			if (_GET_SIZE(_HDRP(_it)) > len_max || _GET_SIZE(_HDRP(_it)) <= len_min)
				heap_error(lineno, "Put in a wrong list!");
			if (_GET(_it) != 0 && _GET(_GET(_it) + WSIZE) != _it)
				heap_error(lineno, "_prev-ptr wrong!");
			_it = _GET(_it + WSIZE);		// Go to next free block
		}
	}
}

void heap_error(int lineno, const char *str) {
	printf("line %d: %s\n", lineno, str);
	exit(0);
}








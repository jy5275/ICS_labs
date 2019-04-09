#include <stdio.h>

int src[8], dst[8];

/* $begin ncopy */
/*
 * ncopy - copy src to dst, returning number of positive ints
 * contained in src array.
 */
int ncopy(int *src, int *dst, int len)
{
    
    int eax = 0;
    int val;
    //*
    while (len > 0) {
	val = *src;
	*dst = val;
	if (val > 0) cnt++;
	len--; src++; dst++
    }   */
    //int cnt0 = 0, cnt1 = 0, val;
    //int *src0 = src, *src1 = src + 1, *dst0 = dst, *dst1 = dst + 1;
    int inc = 0;
    while (edx > 0) {
	esi = (ebx); 
	(ecx) = esi;
	if(esi > 0) eax++;
	
	esi = 4(src);
	4(dst) = esi;
	if(esi > 0) eax++;
	
	ebx += 8; ecx += 8; edx -= 2;
    }
    if(edx > 0){
        esi = (ebx);
        (ecx) = esi;
        if(esi > 0) eax++;
    }
    
    return eax;
}
/* $end ncopy */

int main()
{
    int i, count;

    for (i=0; i<8; i++)
	src[i]= i+1;
    count = ncopy(src, dst, 8);
    printf ("count=%d\n", count);
    exit(0);
}



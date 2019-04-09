/* 蒋衍 1600013239 出品 */
#include <stdio.h>
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
FILE *tracefile = NULL;
unsigned s = 5, b = 5, S = 32, E = 1, hit_count = 0, miss_count = 0, eviction_count = 0, nTime = 0, verbose = 0;
unsigned set_bit, tag_bit;
typedef struct {
	unsigned v, tag, touch;
} Line;
typedef struct {
	Line *lines;
} Set;
Set *L1 = NULL;
void Delete_Cache(){
	for(int i = 0; i < S; i++)
		free(L1[i].lines);
	free(L1);
}
void Init_Cache(){
	L1 = (Set*)malloc(S * sizeof(Set));
	for (int i = 0; i < S; i++){
		L1[i].lines = (Line*)malloc(E * sizeof(Line));
		for (int j = 0; j < E; j++){
			L1[i].lines[j].v = 0;
			L1[i].lines[j].touch = 1 << 30;
		}
	}
}
void GetCmd(int argc, char **argv){
	char ch;
	while ((ch = getopt(argc, argv, "vt:")) != -1) {
		switch (ch) {
			case 'v': verbose = 1; break;
			case 't': tracefile = fopen(optarg, "r"); break;
			default: break;
		}
	}
}
void SearchTag(Set *S, unsigned tag_bit){         // Return 1 for found a line with such tag in set S, 0 otherwise
	int cold = -1;
	unsigned minTouch = 1 << 30, replaceID = 0;
	for(int i = 0; i < E; i++) {
		if(!S->lines[i].v) { cold = i; break; }       // Set cold flag as the first empty line number
		if(S->lines[i].v && S->lines[i].tag == tag_bit) {
			S->lines[i].touch = nTime++;
			hit_count++;
			if(verbose) printf("hit in set[%d]. ", set_bit);
			return;
		}
		if(S->lines[i].touch < minTouch) {    // minTouch record the most remote Line which should be replaced
			minTouch = S->lines[i].touch;
			replaceID = i;
		}
	}  // A finished loop suggests a miss and possibly a eviction! 
	if(cold != -1) {       // Some lines are invalid, fetch the block from memory!
		(S->lines[cold]).v = 1;
		(S->lines[cold]).tag = tag_bit;
		S->lines[cold].touch = nTime++;
		if(verbose) printf("miss! set[%d] <- tag\"0x%x\". ", set_bit, tag_bit);
	}
	else {                 // All lines are occupied. You need to consider which line to evict...		
		if(verbose) printf("miss eviction! set[%d] tag\"0x%x\" <- tag\"0x%x\". ", set_bit, S->lines[replaceID].tag, tag_bit);
		S->lines[replaceID].tag = tag_bit;
		S->lines[replaceID].touch = nTime++;
		eviction_count++;
	}
	miss_count++;
}
int main(int argc, char **argv){
	GetCmd(argc, argv);
	Init_Cache();
	char op; unsigned long addr; unsigned size;
	while (fscanf(tracefile, "%c %lx,%d\n", &op, &addr, &size) != EOF) {
		if (op == 'I') continue;
		if (verbose) printf("%c %lx,%d ", op, addr, size);
		set_bit = (addr>>b) & ((1<<s) - 1), tag_bit = addr >> (b+s);
		if (op == 'L') 
			SearchTag(L1 + set_bit, tag_bit);
		else if (op == 'S') 
			SearchTag(L1 + set_bit, tag_bit);
		else if (op == 'M') {
			SearchTag(L1 + set_bit, tag_bit);
			SearchTag(L1 + set_bit, tag_bit);
		}
		if(verbose) printf("\n");
	}
	Delete_Cache();
	printSummary(hit_count, miss_count, eviction_count);	
	return 0;
}

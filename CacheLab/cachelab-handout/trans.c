// 蒋衍 1600013239 出品
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    int h, i, j, k, t1, t2, t3, t0, t5, t6, t7, t4;
    if (N == 32) {
    	for (i = 0; i < 4; i++) {
    		for (j = 0; j < 4; j++) {
    			for (h = 0; h < 8; h++) {
    				for (k = 0; k < 8; k++) {
    					if (h + 8*i == k + 8*j)		 // Skip ints on the diagonal
    						t0 = A[h + 8*i][h + 8*i];
    					else 						 // Normal
    						B[k + 8*j][h + 8*i] = A[h + 8*i][k + 8*j];
    				}
    				if (i == j)                      // B[8*i + h][8*i + h] is not in cache now, this store would evict the A, 
    					B[8*i + h][8*i + h] = t0;	 // ..However, thrush has been prevented
    			}									
    		}
    	}
    }
    else if (N == 64) {
    	for (i = 0; i < 64; i+=8) {
   			for (j = 0; j < 64; j+=8) {
   				for (h = 0; h < 4; h++) {      // Upper 2 blocks finished!
   					// (r2c) Mov A's upperleft to B's upperleft
   					t0 = A[i+h][j]; 
   					t1 = A[i+h][j+1];
   					t2 = A[i+h][j+2];
   					t3 = A[i+h][j+3];
   					t4 = A[i+h][j+4];
   					t5 = A[i+h][j+5];
   					t6 = A[i+h][j+6];
   					t7 = A[i+h][j+7];
   				
   					// (r2l) Mov A's upperight to B's upperight (reverse)
   					B[j][i+h] = t0;
   					B[j+1][i+h] = t1;
   					B[j+2][i+h] = t2;
   					B[j+3][i+h] = t3;
   					B[j+3][i+4+h] = t4;
   					B[j+2][i+4+h] = t5;
   					B[j+1][i+4+h] = t6;
   					B[j][i+4+h] = t7;
   				}
   			
   				for (k = 0; k < 4; k++) {
   					// (c2tmp) Mov A's lowerleft to tmp0~3
   					t0 = A[i+4][j+3-k];
   					t1 = A[i+5][j+3-k];
   					t2 = A[i+6][j+3-k];
   					t3 = A[i+7][j+3-k];
   					// (c2tmp) Mov A's loweright to tmp4~7
   					t4 = A[i+4][j+4+k];
   					t5 = A[i+5][j+4+k];
   					t6 = A[i+6][j+4+k];
   					t7 = A[i+7][j+4+k];
   					// A finished
   					
   					// (r2r) Mov B's upperleft to B's lowerright
   					B[j+4+k][i] = B[j+3-k][i+4];
   					B[j+4+k][i+1] = B[j+3-k][i+5];
   					B[j+4+k][i+2] = B[j+3-k][i+6];
   					B[j+4+k][i+3] = B[j+3-k][i+7];
   					
   					// (tmp2r) Mov tmp0~3 to B's upperight
   					B[j+3-k][i+4] = t0;
   					B[j+3-k][i+5] = t1;
   					B[j+3-k][i+6] = t2;
   					B[j+3-k][i+7] = t3;
   					// (tmp2r) Mov tmp4~7 to B's loweright
   					B[j+4+k][i+4] = t4;
   					B[j+4+k][i+5] = t5;
   					B[j+4+k][i+6] = t6;
   					B[j+4+k][i+7] = t7;
   				}
   				
   			}
   		}   
    }
    else {
    //============================================================================================================
    	for (i = 0; i < N; i+=16) 
    		for (j = 0; j < M; j+=16)
    			for (h = 0; h < 16 && h+i < N; h++)
    				for (k = 0; k < 16 && k+j < M; k++) 
    					B[j+k][i+h] = A[i+h][j+k];
    //============================================================================================================
    }
    
    ENSURES(is_transpose(M, N, A, B));
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
char trans_blocked_desc[] = "Blocked 64x64";
void trans_blocked(int M, int N, int A[N][M], int B[M][N]) {
	int h, i, j, k, t0;
   	for (i = 0; i < 67; i+=8) {
    	for (j = 0; j < 61; j+=8) {
    		for (h = 0; h < 8; h++) {
    			for (k = 0; k < 8; k++) {
    				if (h + 8*i == k + 8*j)		 // Skip ints on the diagonal
    					t0 = A[h + 8*i][h + 8*i];
    				else 						 // Normal
    					B[k + 8*j][h + 8*i] = A[h + 8*i][k + 8*j];
    			}
    			if (i == j)                      // B[8*i + h][8*i + h] is not in cache now, this store would evict the A, 
    				B[8*i + h][8*i + h] = t0;	 // ..However, thrush has been prevented
    		}									
    	}
   	}
    
    ENSURES(is_transpose(M, N, A, B));
}

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 
    //registerTransFunction(trans_blocked, trans_blocked_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


/* 
 * CS:APP Data Lab 
 * 
 * 蒋衍-30班-1600013239
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 8.0.0.  Version 8.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, plus Amendment 1 (published
   2015-05-15).  */
/* We do not support C11 <threads.h>.  */
/*
 * changeHigh : if low 16-bit of x == y, then let high 16-bit of z = x, otherwise let
 *     high 16-bit of z = y. return z.
 *   Example: changeHigh(0xaaaabbbb, 0xbbbbaaaa, 0xcccccccc) = 0xbbbbcccc,
 *   changeHigh(0xaaaabbbb, 0xddddbbbb, 0xcccccccc) = 0xaaaacccc
 *   Legal ops: ! ~ & ^ | << >>
 *   Max ops: 24
 *   Rating: 1
 */
int changeHigh(int x, int y, int z) {
  int f=!!((x << 16) ^ (y << 16));
  int minus = (~0);
  int Oxffff0000 = minus << 16, Ox0000ffff = (1 << 16) + minus;
  int tmp=(x & Oxffff0000) | ((y >> 16) & Ox0000ffff); //0xffff0000,0x0000ffff
  tmp = tmp << (f << 4);
  return (tmp & Oxffff0000) | (z & Ox0000ffff);
}
/* 
 * newOp - define a new bit-wise opertor PKU : 1PKU1 = 1, 1PKU0 = 1, 0PKU1 = 0, 0PKU0 = 1
 *   input int x, y return xPKUy(bit-wise)
 *   Legal ops: ! ~ & ^ |
 *   Max ops: 6
 *   Rating: 1
 */
int newOp(int x, int y) {
  return ~(~x & y);
}
/* 
 * onlyOne - input x, y return 1 if only one bit of the two numbers is different, return 0 otherwise
 *   Examples onlyOne(12, 14) = 1, onlyOne(12, 15) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int onlyOne(int x, int y) {
  int diffBit = x ^ y;  //diffBit records the different bit-pos.
  int ans = diffBit & (diffBit + (~0)); //large for multi, 0 for non or only-one
  ans = ans + !diffBit;  //add while diffBit is 0
  return !ans;
}
/*
 * truncAvg - Compute the average of the two numbers rounded toward 0
 *          without causing overflow
 *   Example: truncAvg(1, 10) = 5,
 *            truncAvg(-3, 6) = 1,
 *            truncAvg(-3, -4) = -3,
 *            truncAvg(0x7fffffff, 0x7fffffff) = 0x7fffffff
 *   Legal ops: ! ~ & | ^ + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int truncAvg(int x, int y) {  //unsolved!!
	int half_x = x >> 1;
	int half_y = y >> 1;
	int tmpSum = half_x + half_y;
	int f = tmpSum >> 31;   //-1 for negative, 0 for positive
	int left_x = x & 1;
	int left_y = y & 1;
	int plus_neg = !!f;
	int leftover = (left_x + left_y + plus_neg) >> 1;
	return tmpSum + leftover;
}
/* 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 2
 */
int absVal(int x) {
  int f = x >> 31;
  x = (x^f) + (f&1); 
  return x;
}

/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 2
 */
int subOK(int x, int y) {
  int y_is_TMIN = !(y ^ (1 << 31));//1 for TMIN, 0 for others
  int plus_y = (~y) + !y_is_TMIN;  //+0 for TMIN, +1 for others.Sign of plus_y don't change
  int allPos = ~((x | plus_y) >> 31); //-1 for possible positive overflow, 0 otherwise
  int allNeg = ((x & plus_y) >> 31);  //-1 for possible negative overflow, 0 otherwise
  int bitSum = x + plus_y + y_is_TMIN;
  int posOverflow = allPos & (bitSum >> 31);    //-1 for posOverflow, 0 for not
  int negOverflow = (~(bitSum >> 31)) & allNeg; //-1 for negOverflow, 0 for not 
  return !(posOverflow + negOverflow); //1 for OK, 0 for overflow
}
/*
 * fullAdd - 4-bits add using bit-wise operations only.
 *   (0 <= x, y < 16) 
 *   Example: fullAdd(12, 7) = 3,
 *            fullAdd(7, 8) = 15,
 *   Legal ops: ~ | ^ & << >>
 *   Max ops: 30
 *   Rating: 2
 */
int fullAdd(int x, int y) {
  int ans0, ans1, ans2, ans3, adv, inc, adv1, adv2, adv3;
  ans0 = x ^ y;  //indi-bit trust
  adv = (x & y) << 1;
  adv1 = adv & 2; // adv & 0010
  ans1 = ans0 ^ adv1; //ten-bit trust
  inc = (ans0 & adv1) << 1;  //More hundred-bit adv
  adv = adv | inc;
  adv2 = adv & 4;  //adv & 0100, hundred-bit ready to adv
  ans2 = adv2 ^ ans1;    //hundred-bit trust
  inc = (ans0 & adv2) << 1;  //More thousand-bit adv
  adv = adv | inc;
  adv3 = adv & 8;  //adv & 1000, thousand-bit ready to adv
  ans3 = adv3 ^ ans2;
  return ans3;
}
/* 
 * theMax - the same as max(x, y) 
 *   Example: max(4,5) = 5, max(6,3) = 6
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 28
 *   Rating: 3
 */

int theMax(int x, int y) {
  int f1 = x >> 31, f2 = y >> 31;
  int same = f1 ^ f2;    //0 for same, -1 for diff;
  int x_larger_set_f_0 = ~((~f1) & same);
  int y_larger_set_f_1111 = (~f2) & same;
  int f = (x + (~y) + 1) >> 31;  //suppose x,y are positive
  f = (f | y_larger_set_f_1111) & x_larger_set_f_0;
  return (y & f) + (x | f) + (!!f);  //x while x is larger, y while y is larger
}
/*
 * palindrome - return 1 if x is palindrome in binary form,
 *   return 0 otherwise
 *   A number is palindrome if it is the same when reversed
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   Example: palindrome(0xff0000ff) = 1,
 *            palindrome(0xff00ff00) = 0
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 40
 *   Rating: 4
 */
int palindrome(int x) {
	int rvs, rvs2;
	int rvs1 = (x & 0x8888) >> 3;     //000x 000x 000x 000x
	rvs1 = rvs1 | ((x & 0x4444) >> 1); //00xx 00xx 00xx 00xx 
	rvs1 = rvs1 | ((x & 0x2222) << 1);
	rvs1 = rvs1 | ((x & 0x1111) << 3); //(11 ops)
	rvs2 = rvs1 << 12;
	rvs2 = rvs2 | ((rvs1 & 0xf0) << 4); //
	rvs2 = rvs2 | ((rvs1 & 0xf00) >> 4); 
	rvs2 = rvs2 | ((rvs1 & 0xf000) >> 12);  //(21 ops) 
	
	rvs = rvs2 << 16;
	return !((x & 0xffff0000) ^ rvs);
}
/*
 * distance - calculate how many bits are different between x and y
 *   Example: distance(0xabcdabcd, 0xabcddcba) = 12,
 *            distance(0x0000ffff, 0xffff0000) = 32,
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 40
 *   Rating: 4
 */
int distance(int x, int y) {  //ops Exceeded!
	int dis = x ^ y, c4, t, ans;
	int collect = (17 << 8) | 17; //0001 0001 0001 0001
	collect = (collect << 16) | collect;//0001 0001 0001 0001...
	t = dis & collect;
	t = t + ((dis >> 1) & collect);
	t = t + ((dis >> 2) & collect);
	t = t + ((dis >> 3) & collect);
	c4 = 0xf;
	ans = t & c4;
	ans = ans + ((t >> 4) & c4);
	ans = ans + ((t >> 8) & c4);
	ans = ans + ((t >> 12) & c4);
	ans = ans + ((t >> 16) & c4);
	ans = ans + ((t >> 20) & c4);
	ans = ans + ((t >> 24) & c4);
	ans = ans + ((t >> 28) & c4);
	return ans;
}
/*
 * divSix - calculate x / 6 without using /
 *   Example: divSix(6) = 1,
 *            divSix(2147483647) = 357913941,
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 40
 *   Rating: 4
 */
int divSix(int x_o) {
	int w, is_TMIN = !(x_o ^ (1 << 31));  //1 for TMIN, 0 for others
	int f = x_o >> 31;  //0 for positive, 11111111 for negative
	int tot = 0;
	int x = (x_o^f) + (f & 1);  //x get the absVal

	int mod1 = (1 << 30) + (1 << 28) + (1 << 26) + (1 << 24) + (1 << 22) + (1 << 20) +
		(1 << 18) + (1 << 16) + (1 << 14) + (1 << 12) + (1 << 10) + (1 << 8) + (1 << 6) +
		(1 << 4) + (1 << 2) + 1;
	int mod2 = mod1 << 1;
	int rec1 = x & mod1, rec2 = x & mod2; //Record bit existence int 'rec'
	int collect, pos, ans;
	int cnt_mod1 = 0, cnt_mod2 = 0, mod = 0;   //max is 16
	int minus = ~0;      //-1[11111111]
						 //cout << hex << mod1 << ',' << mod2 << endl;
	w = (mod2 | (1 << 30)) + 1;
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 0
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 1
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 2
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 3
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 4
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0
	rec1 = rec1 & (rec1 + minus);  // 5
	cnt_mod1 = cnt_mod1 + !!rec1;  //add while rec1!= 0

	cnt_mod2 = cnt_mod2 + !!rec2;
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	rec2 = rec2 & (rec2 + minus);  // 5
	cnt_mod2 = cnt_mod2 + !!rec2;  //add while rec1!= 0
	mod = cnt_mod1 + (cnt_mod2 << 1);  //sum of total left

	//-----------mod很大,待会要从里边提取add_ans---------------
	
	//Begin at pos 2
	collect = 1, ans = 0, pos = 0x4;   //pos=[0000 0100]
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 4
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 5
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 6
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 7
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 8
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 9
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 10
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 11
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 17
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 21
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 22
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 23
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 24
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 25
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 26
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 27
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 28
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 29
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 30
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	//pos = pos << 1;    //Now you are at 31
	//collect = (collect << 1) + !!(pos & mod1); //renew collect
	//collect = ~collect + 1;  //31位权重变为负数
	//ans = ans + (collect & (~(((!(pos & x)) << 31) >> 31)));
	tot = tot + ans;

	//--------------------收集到一层mod,从中找额外的ans------------------
	//Begin at pos 2
	collect = 1, ans = 0, pos = 0x4;   //pos=[0000 0100]
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 4
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 5
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 6
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 7
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	tot = tot + ans;

	cnt_mod1 = !!(mod & 0x40) + !!(mod & 0x10) + !!(mod & 0x4) + !!(mod & 0x1);
	cnt_mod2 = !!(mod & 0x80) + !!(mod & 0x20) + !!(mod & 0x2) + !!(mod & 0x8);
	mod = cnt_mod1 + (cnt_mod2 << 1);  //max is 11

	//--------------------收集到二层mod,从中找额外的ans------------------
	 //Begin at pos 2
	collect = 1, ans = 0, pos = 0x4;   //pos=[0000 0100]
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	pos = pos << 1;    //Now you are at 3
	collect = (collect << 1) + !!(pos & mod1); //renew collect
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	tot = tot + ans;

	cnt_mod1 = !!(mod & 0x1) + !!(mod & 0x4);
	cnt_mod2 = !!(mod & 0x2) + !!(mod & 0x8);
	mod = cnt_mod1 + (cnt_mod2 << 1);  //max is 5

	//--------------------收集到三层mod,从中找额外的ans------------------
	//Begin at pos 2
	collect = 1, ans = 0, pos = 0x4;   //pos=[0000 0100]
	ans = ans + (collect & (~(((!(pos & mod)) << 31) >> 31)));
	tot = tot + ans;
	tot = tot + !(mod ^ 3);

	tot = tot >> 1;
	tot = ( (((tot ^ f) + !!f) << (is_TMIN << 4)) << (is_TMIN << 4)) |
		( (w << ((!is_TMIN) << 4)) << ((!is_TMIN) << 4));

	return tot;
}
/*
 * modThree - calculate x mod 3 without using %.
 *   Example: modThree(12) = 0,
 *            modThree(2147483647) = 1,
 *            modThree(-8) = -2,
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 60
 *   Rating: 4
 */

int modThree(int x_o) {
	int f = x_o >> 31;  //0 for positive, 11111111 for negative
	int sum1, sum2, sum3, sum4, plus1 = f & 1;
	int x = (x_o^f) + plus1;  //x get the absVal
	//cout << " x = " << x << ", ";
	int collect3 = 3 | (3 << 8);
	collect3 = (collect3 << 16) | collect3;  //0000 0011 0000 0011...
	//cout << "collect3 = " << hex << collect3 << endl;
	sum1 = x & collect3;
	sum1 = sum1 + ((x >> 2) & collect3);
	sum1 = sum1 + ((x >> 4) & collect3);
	sum1 = sum1 + ((x >> 6) & collect3);
	//cout << "sum1 = " << hex << sum1 << endl;
	sum1 = sum1 + (sum1 >> 16);
	//cout << "sum1 = " << hex << sum1 << endl;
	sum1 = sum1 + (sum1 >> 8); //sum1 record mod with max 48, 低8位有效
	//cout << "sum1 = " << hex << sum1 << endl;

	sum2 = sum1 & 0x33;
	sum2 = sum2 + ((sum1 >> 2) & 0x33);
	sum2 = sum2 + (sum2 >> 4); //sum2 record mod with max 11, 低4位有效(24 ops)
	//cout << "sum2 = " << hex << sum2 << endl;

	sum3 = (sum2 & 3) + ((sum2 >> 2) & 3); //max 5 (28 ops)

	sum4 = (sum3 & 3) + ((sum3 >> 2) & 1); //max 3 (32 ops)

	sum4 = sum4 & 3;   //清除高30位
	sum4 = sum4 >> (!(sum4 ^ 3) << 1);   //(37 ops)
	sum4 = (f ^ sum4) + plus1;
	return sum4;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 3
 */
int float_f2i(unsigned uf) {
	int minus = ~0;  //-1
	int TMIN = 1 << 31;  //1000..0
	int Bias_minus = (minus << 7) + 1;  //-127
	int exp_extractor = (1 << 8) + minus;  //0..0011111111
	int exp = exp_extractor & (uf >> 23);  //Exponent bit saved (8 ops)
	int E = exp + Bias_minus;   //ans = M*2^E
	int E_minus = ~E + 1;
	int displace = 31 + E_minus;
	unsigned waiting = (uf << 8) | TMIN;       //准备右移,头置1
	//cout << "waiting = " << hex << waiting << endl;
	unsigned tmp = waiting >> displace;  //右移到位,头已清零
	//cout << "tmp = " << hex << tmp << endl;
	if (uf >> 31) tmp = ~tmp + 1;         //(25 ops)
	if ((displace + minus) >> 31) return TMIN;  //if(E>=31) return TMIN;溢出！
	if (E >> 31) return 0;      //if(E<=0) return 0;
	if (!(exp^exp_extractor)) return TMIN;  //if(exp == 0..0011111111), break!
	return tmp;
}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 3
 */
unsigned float_half(unsigned uf) {
	int TMIN = 1 << 31, f, ans_except, plus1;
	unsigned minus = ~0;      //111..11
	int exp_extractor = minus >> 24;   //255[0..0011111111]
	int exp = (uf >> 23) & exp_extractor;
	int ans_exp = exp + minus;       //ans_exp>=0
	int replenished = uf & (~(exp_extractor << 23));  
	int ans = replenished | (ans_exp << 23);   //replenish exp
	if (!(exp ^ exp_extractor)) return uf;     //NaN/Inf, return argument
	if (!(exp & (minus << 1))) {       //exp==0||exp==1
		f = uf & TMIN;
		ans_except = (minus >> 1) & uf;  //ans_except waiting to shift and set signal
		plus1 = (ans_except & 1) && (ans_except & 2); //2 predicaments for round up
		ans_except = ans_except >> 1;    //shift right!
		ans = f | ans_except;            //set signal!
		if (ans_except) ans = ans + plus1;  //plus1 only if(ans_except!=0)
		return ans;
	}
	return ans;
}
/* 
 * float_construct - Return bit-level equivalent of expression float(x*2^y),
 *   you should treat the input as integers: if x = 2, y = 3, you should return a float value of 16  
 *   we ensure the constructed number can be represented in float: X[-65535, 65535], y[-160, 60]
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 90
 *   Rating: 4
 */

unsigned float_construct(int x_o, int y) {
	int op_bit, Round, Guard, ans_except, Sticky, replenished, exp_ans;
	int filled, exp_ans_minus;
	int f = x_o >> 31;  //0 for positive, 11111111 for negative
	int plus1 = f & 1, detect = 0, cnt = 0, exp, TMIN = 1 << 31;
	int x = (x_o^f) + plus1;  //x get the absVal
	unsigned minus = ~0, ux = x;
	unsigned test = TMIN;
	while (!detect) {   //只要detect还是0(test还没移到x最左边的1)
		test = test >> 1;
		detect = test & x;
		cnt += 1;  //到这儿移动了cnt步
		if (!test) return 0; //x=0
	}
	exp = 31 - cnt + 127;
	plus1 = 0;
	ux = (ux << (cnt + 1)) >> 9; 
	if (ux & (1 << 23)) exp = exp + 1;   //if(多米诺) exp++;
	replenished = ux & (minus >> 9); //清空ux的sign与exp区,此时f-exp-replenished忠实反映float
	exp_ans = exp + y;
	filled = replenished | (exp_ans << 23);  //set exp_ans!
	filled = filled | (f << 31);      //set signal!

	if ((exp_ans + minus) >> 31) {     //if(exp_ans<=0)
		exp_ans_minus = ~exp_ans + 1;
		op_bit = exp_ans_minus + 1;   //op_bit为待手动右移的bit数
		if (op_bit & (minus << 5)) op_bit = 31;  //if(op_bit>=32) 减为31
		ans_except = replenished | (!!exp << 23);  //if(exp非零)exp末位变1,方便右移
		Round = (ans_except >> (op_bit + minus)) & 1;
		Guard = (ans_except >> op_bit) & 1;
		plus1 = 0;
		Sticky = !!(ans_except & ((1 << (op_bit + minus)) + minus));
		if ((Round && Guard) || (Round && Sticky)) plus1 = 1;
		ans_except = ans_except >> op_bit;   //ans_except shifted!
		ans_except = (f<<31) | ans_except;    //ans_except set signal!
		ans_except += plus1;
		return ans_except;
	}
	return filled;
}

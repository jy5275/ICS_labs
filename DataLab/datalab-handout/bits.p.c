#line 176 "bits.c"
int changeHigh(int x, int y, int z) {
  int f=!!((x << 16) ^( y << 16));
  int minus=(  1 << 31) >> 31;
  int Oxffff0000=  minus << 16;int Ox0000ffff=(1<< 16) + minus;
  int tmp=(x & Oxffff0000) |(( y >> 16) & Ox0000ffff);
  tmp = tmp <<( f << 4);
  return (tmp & Oxffff0000) |( z & Ox0000ffff);
}
#line 191
int newOp(int x, int y) {
  return ~(~x & y);
}
#line 201
int onlyOne(int x, int y) {
  int diffBit=  x ^ y;
  int ans=  diffBit &( diffBit +(( 1 << 31) >> 31));
  ans = ans + !diffBit;
  return !ans;
}
#line 218
int truncAvg(int x, int y) {
    return 0;
}
#line 229
int absVal(int x) {
  return 2;
}
#line 240
int subOK(int x, int y) {
  return 2;
}
#line 252
int fullAdd(int x, int y) {
    return 2;
}
#line 262
int theMax(int x, int y) {
  return 2;
}
#line 278
int palindrome(int x) {
    return 2;
}
#line 289
int distance(int x, int y) {
  return 2;
}
#line 300
int divSix(int x) {
    return 2;
}
#line 312
int modThree(int x) {
    return 2;
}
#line 327
int float_f2i(unsigned uf) {
  return 2;
}
#line 341
unsigned float_half(unsigned uf) {
  return 2;
}
#line 355
unsigned float_construct(int x, int y) {
  return 2;
}

#include<stdio.h>
long f(long *p){
    long a = 0;
    scanf("%ld", &a);
    return *p + a;
}
int main(){
    long a = 1;
    long *p = &a;
    *p = f(p);
    printf("%ld",*p);
    return 0;
}

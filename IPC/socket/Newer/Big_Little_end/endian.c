#include <stdio.h>

int main()
{
    int n = 0x12345678;
    char *p = (char *)&n;

    printf("pointer p :%p = %#x\n", p, *p);
    printf("pointer p :%p = %#x\n", p+1, *(p + 1));
    printf("pointer p :%p = %#x\n", p+2, *(p + 2));
    printf("pointer p :%p = %#x\n", p+3, *(p + 3));
    return 0;

}
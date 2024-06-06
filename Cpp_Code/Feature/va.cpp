#include <stdarg.h>
#include <stdio.h>

int add_number(int n, ...);

int main()
{
    printf("hello world\n");
    printf("%d\n", add_number(1, 1));
    printf("%d\n", add_number(2, 1, 2));
    printf("%d\n", add_number(3, 1, 2, 3));
    printf("%d\n", add_number(4, 1, 2, 3, 4));
    return 0;
}

int add_number(int n, ...) 
{
    va_list va;
    va_start(va, n);

    int ret = 0;

    for (int i = 0; i < n; i++) 
    {
        ret += va_arg(va, int);
    }
    va_end(va);

    return ret;
}


// cc -o va va.cpp
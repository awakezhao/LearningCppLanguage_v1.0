

#include <setjmp.h>
#include <stdio.h>

jmp_buf env;
int count = 0;

void sub_func(int idx) {
    printf("sub_func --> idx:%d\n", idx);
    longjmp(env, idx);
}

int main() {

    int idx = 0;

    //第一次调用返回0；其他时候返回longjmp传递的值
    count = setjmp(env);
    if (count == 0) {
        printf("count:%d\n", count);
        sub_func(++idx);
    } else if (count == 1) {
        printf("count:%d\n", count);
        sub_func(++idx);
    } else if (count == 2) {
        printf("count:%d\n", count);
        sub_func(++idx);
    } else if (count == 3) {
        printf("count:%d\n", count);
        sub_func(++idx);
    } else {
        printf("other count\n");
    }
}
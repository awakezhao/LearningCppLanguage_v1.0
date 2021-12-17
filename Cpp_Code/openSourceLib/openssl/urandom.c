
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd=0;
    char *buff=NULL;
    unsigned long ulTest = 0;

    fd = open("/dev/random",O_RDONLY);

    read(fd,&ulTest,sizeof(ulTest));

    short s;
    read(fd, &s, sizeof(s));
    printf("%d\n", s);

    printf("%lu\n",ulTest);


    close(fd);
}
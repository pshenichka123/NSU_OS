#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
int main(void)
{
    write(1, "Hello World!\n", 13);
    return 0;
}
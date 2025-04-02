#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>




int my_write(int out_file_desctiptor, char* buffer, int buffer_size)
{
    return syscall(SYS_write, out_file_desctiptor, buffer, buffer_size);
}

int main(void)
{
    my_write(1, "Hello World\n", 12);
    return 0;
}

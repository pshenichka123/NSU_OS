#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

int get_file_size(int fd);
int open_files(int* fdSrc, int* fdDest, const char* src_file, const char* dest_file);
int reverse_file_content(const char* src_path, const char* dest_path);
int process_file(const char* dest_dir, const char* file_name, const char* src_path);
void reverse_buffer(char* buffer, int buffer_size);
int prepare_dest_path(const char* dest_dir, const char* file_name, char* dest_path);
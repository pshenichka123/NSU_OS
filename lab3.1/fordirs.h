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
#include "forfiles.h"
#define ERROR -1
#define DIR_ACCESS (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define FILE_ACCESS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) 


int go_inside_dir(const char* dest_dir, const char* cur_dir_name, const char* src_path);
int process_directory(const char* src_dir, const char* dest_dir);
int go_inside_dir(const char* dest_dir, const char* cur_dir_name, const char* src_path);

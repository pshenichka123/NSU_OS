#include"fordirs.h"


const char* get_last_dir(const char* path, char* dir_name) {
    if (path == NULL || *path == '\0') {
        return NULL;
    }
    const char* last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        strncpy(dir_name, path, PATH_MAX);
        return path;
    }
    strncpy(dir_name, last_slash + 1, PATH_MAX);
    return last_slash + 1;

}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_directory>\n", argv[0]);
        return 0;
    }
    char dir_name[PATH_MAX];
    get_last_dir(argv[1], dir_name);
    char reversed_dir[PATH_MAX];


    strcpy(reversed_dir, dir_name);
    reverse_buffer(reversed_dir, strlen(reversed_dir));

    int good_behaviour = mkdir(reversed_dir, DIR_ACCESS);
    if (good_behaviour != 0) {
        fprintf(stderr, "%s:%s\n", "Cant open dir:", reversed_dir);
        perror("Error");
        return 0;
    }

    int reversing_status = process_directory(argv[1], reversed_dir);
    if (reversing_status != 0)
    {
        puts("Errors during reversing, reversed directory is removed");
        rmdir(reversed_dir);
        return 0;
    }
    printf("Directory '%s' copied to '%s' with reversed file names and contents.\n", argv[1], reversed_dir);

    return 0;
}
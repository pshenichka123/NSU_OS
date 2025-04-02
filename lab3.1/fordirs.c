#include "fordirs.h"


int process_directory(const char* src_dir, const char* dest_dir)
{
    DIR* dir = opendir(src_dir);
    if (dir == NULL) {
        fprintf(stderr, "%s:%s", "Failed to open src dir:", src_dir);
        perror("Error");
        return ERROR;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {

        char src_path[PATH_MAX];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
        struct stat path_stat;
        if (lstat(src_path, &path_stat) != 0) {
            fprintf(stderr, "%s:%s", "Failed to get file stat:", src_dir);
            perror("Error");
            closedir(dir);
            return ERROR;
        }
        int behaviour_status = 0;

        if (S_ISDIR(path_stat.st_mode)) {
            behaviour_status = go_inside_dir(dest_dir, entry->d_name, src_path);
        }

        if (S_ISREG(path_stat.st_mode)) {
            behaviour_status = process_file(dest_dir, entry->d_name, src_path);
        }

        if (behaviour_status != 0) {
            closedir(dir);
            return ERROR;
        }

    }
    closedir(dir);
    return 0;
}


int go_inside_dir(const char* dest_dir, const char* cur_dir_name, const char* src_path)
{
    if (cur_dir_name[0] == '.') {
        return 0;
    }
    char dest_path[PATH_MAX];
    int behaviour_status = prepare_dest_path(dest_dir, cur_dir_name, dest_path);
    if (behaviour_status == ERROR)
    {
        fprintf(stderr, "Can't prepare destinanion path:%s/%s", dest_dir, cur_dir_name);
        perror("Error");
        return ERROR;
    }
    behaviour_status = mkdir(dest_path, DIR_ACCESS);
    if (behaviour_status != 0)
    {
        fprintf(stderr, "%s:%s", "Cant create directory:", dest_path);
        perror("Error");
        return ERROR;
    }

    behaviour_status = process_directory(src_path, dest_path);
    if (behaviour_status != 0)
    {
        fprintf(stderr, "%s:%s", "Can't process directory:", dest_path);
        perror("Error");
        return ERROR;
    }
    return 0;
}

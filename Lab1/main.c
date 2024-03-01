#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void print_usage() {
    printf("Usage: dirwalk [dir] [options]\n");
    printf("Options:\n");
    printf("  -l\tOnly symbolic links (-type l)\n");
    printf("  -d\tOnly directories (-type d)\n");
    printf("  -f\tOnly files (-type f)\n");
    printf("  -s\tSort output according to LC_COLLATE\n");
}

void process_directory(const char *path, int l_flag, int d_flag, int f_flag, int s_flag) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (lstat(full_path, &statbuf) == -1) {
            perror("lstat");
            continue;
        }

        if (!l_flag && !d_flag && !f_flag) {
            printf("%s\n", full_path);
        }

        if ((l_flag && S_ISLNK(statbuf.st_mode)) ||
            (d_flag && S_ISDIR(statbuf.st_mode)) ||
            (f_flag && S_ISREG(statbuf.st_mode))) {
            printf("%s\n", full_path);
        }

        if (S_ISDIR(statbuf.st_mode)) {
            process_directory(full_path, l_flag, d_flag, f_flag, s_flag);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    int opt;
    int l_flag = 0, d_flag = 0, f_flag = 0, s_flag = 0;
    char *dir_path = ".";

    while ((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch (opt) {
            case 'l':
                l_flag = 1;
                break;
            case 'd':
                d_flag = 1;
                break;
            case 'f':
                f_flag = 1;
                break;
            case 's':
                s_flag = 1;
                break;
            default:
                print_usage();
                return EXIT_FAILURE;
        }
    }

    if (optind < argc) {
        dir_path = argv[optind];
    }

    process_directory(dir_path, l_flag, d_flag, f_flag, s_flag);

    return EXIT_SUCCESS;
}

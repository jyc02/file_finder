#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

typedef struct Flags {
    int is1; // "S" Argument
    int is2; // "s" Argument
    int arg2; // Size in bytes for "s" argument
    int is3; // "f" Argument
    char *arg3; // Pattern for "f" argument
    int depth; // Depth for "f" argument
    int is4; // "t" Argument (unused)
    char *arg4; // Time argument for "t" (unused)
} Flags;

Flags* init_Flags(Flags *flags) {
    flags->is1 = FALSE;
    flags->is2 = FALSE;
    flags->arg2 = 0;
    flags->is3 = FALSE;
    flags->arg3 = NULL;
    flags->depth = 0;
    flags->is4 = FALSE;
    flags->arg4 = NULL;
    return flags;
} 

void traverseDir(int count, char *path, int indent, Flags flags, int currentDepth) {
    struct dirent *dirent;
    DIR *parentDir;
    struct stat fileStat;
    char filepath[500];
    parentDir = opendir(path);

    if (parentDir == NULL) {
        printf("Error opening directory '%s'\n", path);
        exit(-1);
    }
    while ((dirent = readdir(parentDir)) != NULL) {
        if (strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0 && strcmp(dirent->d_name, ".DS_Store") != 0) {
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, dirent->d_name);

            if (stat(filepath, &fileStat) < 0) {
                perror("Error getting file stats");
                continue;
            }

            int printFile = TRUE;
            if (flags.is2 && (fileStat.st_size > flags.arg2)) {
                printFile = FALSE;
            }
            if (flags.is3 && (strstr(dirent->d_name, flags.arg3) == NULL || currentDepth > flags.depth)) {
                printFile = FALSE;
            }

            if (printFile) {
                for (int i = indent; i > 0; i--) {
                    printf("    ");
                }
                printf("[%d] %s ", count, dirent->d_name);

                if (flags.is1) {
                    printf("%lld %o %s ",
                        (long long) ((dirent->d_type == DT_DIR) ? 0 : fileStat.st_size),
                        fileStat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO),
                        ctime(&fileStat.st_atime));
                }
                
                printf("\n");
                count++;

                if (dirent->d_type == DT_DIR) {
                    traverseDir(count, filepath, indent + 1, flags, currentDepth + 1);
                }
            }
        }
    }
    closedir(parentDir);
}

int main(int argc, char **argv) {
    int opt;
    Flags flags;
    init_Flags(&flags);

    while ((opt = getopt(argc, argv, "Ss:f:")) != -1) {
        switch (opt) {
            case 'S':
                flags.is1 = TRUE;
                break;
            case 's':
                flags.is2 = TRUE;
                flags.arg2 = atoi(optarg);
                break;
            case 'f':
                flags.is3 = TRUE;
                flags.arg3 = optarg;
                break;
        }
    }

    if (flags.is3 && argv[optind] != NULL && optind < argc) {
        flags.depth = atoi(argv[optind]);
        optind++; // Move past the depth argument
    }

    char* startDir = (optind < argc) ? argv[optind] : "."; // Start directory
    traverseDir(1, startDir, 0, flags, 0);

    return 0;
}
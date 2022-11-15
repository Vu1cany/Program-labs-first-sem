#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char **argv) {

    for (int i = 1; i < argc; ++i) {
        int err = mkdir(argv[i], 0777);
        if (!err) {
            printf("Directory %s was created successfully!\n", argv[i]);
        } else {
            printf("Directory %s is already exists!\n", argv[i]);
        }
    }

    FILE *file = fopen("Info.txt", "w");
    
    DIR *dir;
    struct dirent *current;
    dir = opendir(".");
    int count = 0;
    while ((current = readdir(dir)) != NULL) {
        if (current->d_type == DT_DIR) {
            count++;
            if (count < 3) continue;
            fputs(current->d_name, file);
            fputs("\n", file);
        }
    }

    fclose(file);
    closedir(dir);

    return 0;
}

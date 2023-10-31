#pragma warning(disable:4996)
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void cat_m(char** res) {
        char buf[512];
        int n, fd;

        if (res[1] == NULL) {
                perror("usage : cat_m filename");
                return;
        }

        const char* name = res[1];
        printf("%s", name);

        fd = open(name, O_RDONLY);

        if (fd == -1) {
                perror("cat_m error");
                return;
        }

        while( (n = read(fd, buf, sizeof(buf))) > 0) {
                printf("%s\n",buf);
        }

        return;
}

void cd_m(char** res) {

        char* name = res[1];
        if( chdir(name) == -1){
                perror("cd_m error");
        }
        return;
}

void mkdir_m(char** res) {
        char* name = res[1];

        mkdir(name, 0700);
        return;
}

void ls_m(char** res) {

        DIR* dirptr = NULL;
        struct dirent* entry = NULL;

        if( res[1] != NULL ) {
                perror("usage : ls only");
                return;
        }

        char name[100];
        getcwd(name, sizeof(name));

        dirptr = opendir(name);
        if( dirptr == NULL ){
                perror("ls_m error");
                return;
        }

        while( (entry = readdir(dirptr)) != NULL) {
                printf("[%ul] %s\n", entry->d_ino, entry->d_name);
        }
        return;
}

void vi_m(char ** res){
        struct stud buf;        
        int fd;

        fd = open(res[1], O_RDWR | O_CREAT, 0644);
        scanf("%d", &buf.no);
        getchar();

        while(buf.no >0){
                scanf("%d", &buf.no);
                getchar();
        }
        lseek(fd, 0, SEEK_SET); 
        write(1, res, 99);
}

int main(void) {
        char in[100], * res[20] = { 0 };
        char* inst[4] = { "cat", "cd", "mkdir", "ls" };
        void (*f[4])(char**) = { cat_m, cd_m, mkdir_m, ls_m };
        int i;

        while (1) {

                getcwd(in, sizeof(in));
                printf("%s\n", in);

                gets(in);

                if (in[0] == '\0') continue;
                i = 0;

        // get the command argument
                res[i] = strtok(in, " ");

                while (res[i]) {
                        i++;
                        res[i] = strtok(NULL, " ");
                }

                if (strcmp(res[0], "exit") == 0) exit(0);

                for (i = 0; i < 4; i++) {
                        if (!strcmp(res[0], inst[i])) {
                                f[i](res);
                                break;
                        }
                }
        }
        return 0;
}
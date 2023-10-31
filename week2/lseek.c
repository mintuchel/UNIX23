#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

int main(){
        int fd, p;

        fd = open("data", O_RDONLY);
        char buf[10];

        read(fd, buf, sizeof(char));
        write(1, buf, sizeof(char));
        printf("\n\n");

        p = lseek(fd, -1, SEEK_SET);
        printf("p : %d\n\n",p);

        p = lseek(fd, 0, SEEK_SET);
        printf("p : %d\n",p); // 0
        read(fd, buf, sizeof(char));
        write(1, buf, sizeof(char)); // a
        printf("\n\n");

        p = lseek(fd, +1, SEEK_SET);
        printf("p : %d\n",p); // 1
        read(fd, buf, sizeof(char));
        write(1, buf, sizeof(char)); // b
        printf("\n\n");

        p = lseek(fd, +2, SEEK_SET);
        printf("p : %d\n",p); // 2
        read(fd, buf, sizeof(char));
        write(1, buf, sizeof(char)); // c
        printf("\n\n");

        p = lseek(fd, 0, SEEK_END); 
        printf("p : %d\n",p); // 11
        read(fd, buf, sizeof(char));
        write(1, buf, sizeof(char));
        printf("\n\n");

        p = lseek(fd, -1, SEEK_END);
        printf("p : %d\n",p); // 10
        read(fd, buf, sizeof(char));
        write(1, buf, sizeof(char)); 
        printf("\n\n");

        p = lseek(fd, -2, SEEK_CUR);
        printf("p : %d\n",p); // 10
        read(fd, buf, sizeof(char));
        write(1, buf, sizeof(char)); 
        printf("\n\n");
        
        p = lseek(fd, +1, SEEK_END); // 12
        printf("p : %d\n",p);
        read(fd, buf, sizeof(char));
        write(1, buf, sizeof(char));
        printf("\n\n");

        exit(0);
}
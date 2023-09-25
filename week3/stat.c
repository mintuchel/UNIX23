#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
        // save information of data1 file in
        // "struct stat" type variable, buf
        struct stat buf;
        stat("data1", &buf);

        printf("%o %d %ld\n", buf.st_mode & 0777, buf.st_nlink, buf.st_size);

        return 0;
}
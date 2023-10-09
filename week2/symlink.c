#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    char linkname[50], realname[50];
    scanf("%s",linkname);

    struct stat buf;

    // get info of the real linkfile
    lstat(linkname, &buf);

    printf("file name : %s\n",linkname);
    printf("access permission : %o\n", buf.st_mode&0777);
    printf("size : %d\n\n", buf.st_size);

    // get real file's name
    readlink(linkname, realname, sizeof(realname));
    // get info of the real file
    stat(linkname, &buf);

    printf("file name : %s\n", realname);
    printf("access permission : %o\n", buf.st_mode&0777);
    printf("size : %d", buf.st_size);

    return 0;
}

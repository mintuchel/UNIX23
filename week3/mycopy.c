#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
        char n1[50], n2[50], buf[512], in;
        int i, fd1, fd2, n;

        scanf("%s",n1);
        scanf("%s",n2);

        fd1 = open(n1, O_RDONLY);
        if( fd1 == -1) {
                printf("%s : No such file...\n", n1);
                return 0;
        }

        // there have to be no file named n2
        // then we create the file with name n2
        fd2 = open(n2, O_WRONLY | O_CREAT | O_EXCL, 0644);

        if( fd2 == -1 ){
                printf("%s : File already exists... Do you want to overwrite it?\n", n2);
                getchar();
                scanf("%c",&in);
                if( in!='Y' ) return 0;

                fd2 = open(n2, O_WRONLY | O_TRUNC);
        }

        while( (n = read(fd1, buf, sizeof(buf))) > 0) {
                write(fd2, buf, n);
        }

        return 0;
}
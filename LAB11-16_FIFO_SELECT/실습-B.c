#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	char f[2][3] = {"f1", "f2"};
	int i, in, fd[2];

	// FIFO 2개 만들기
	mkfifo("f1", 0666);
	mkfifo("f2", 0666);

	// "f1" "f2" open
	fd[0] = open("f1", O_RDONLY);
	fd[1] = open("f2", O_WRONLY);

	for(;;) {
		// f1으로 정수 읽기
		read(fd[0], &in, sizeof(int));
		if(in==-1) exit(0);
		in+=8;
		// f2로 정수 보내기
		write(fd[1], &in, sizeof(int));
	}	
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

void do_child(int fd) {
	char buf[5] = {0};
	
	read(fd, buf, sizeof(char)*2);
	// can use write too.
	
    // buf[3] = '\n';
	// write(1, buf, sizeof(buf));	
	
    printf("%s pid = %d\n", buf, getpid());
	
    exit(0);
}

// this is getting 3 arguments
// ex) ./a.out data1 4

int main(int argc, char** argv) {
	int num, i, fd;
	pid_t pid;
	
	fd = open(argv[1],O_RDONLY);
	num = atoi(argv[2]);

	for(i=0;i<num;i++) {
		pid = fork();
		if(pid==0){
			do_child(fd);
		}
	}

	for(i=0;i<num;i++) {
		wait(0);
	}
	return 0;
}

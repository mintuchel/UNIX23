#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	int i, in, pid, p[2][2];
	int status;
	
	for(i=0;i<2;i++)
		pipe(p[i]);

	pid = fork();

	// child
	if(pid==0) {
		// close unusing pipes in child
		close(p[1][0]);
		close(p[0][1]);

		for(i=0;i<3;i++){
			// child read is BLOCKED until parent writes
			if(read(p[0][0], &in, sizeof(int))) {
				printf("child reads %d ...\n", in);
				in++;
				write(p[1][1], &in, sizeof(int));
				printf("child writes %d ...\n", in);
			}
		}

		exit(0);
	}

	// parent
	
	// close unusing pipes in parent
	close(p[0][0]);
	close(p[1][1]);

	// write 3 times and read 2 times	
	for(i=0;i<3;i++) {
		scanf("%d",&in);
		printf("parent writes %d ...\n", in);
		write(p[0][1], &in, sizeof(int));

		if(i<2) {
			read(p[1][0], &in, sizeof(int));
			printf("parent reads %d ... \n", in);
		}else{
			// closing read pipe
			close(p[1][0]);
			// the write pipe in child gets SIGPIPE SIGNAL
			// since the signal is not handled here,
			// process terminates right away
		}
	}

	wait(&status);
	if(WIFEXITED(status))
		printf("exits ... %d\n", WEXITSTATUS(status));
	else if(WIFSIGNALED(status))
		printf("terminated ... %d\n", WTERMSIG(status));
	exit(0);
}
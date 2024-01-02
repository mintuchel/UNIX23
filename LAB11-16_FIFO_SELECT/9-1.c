#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	int i, in, pid, p[2][2];
	
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
			}else{
			// when write pipe is closed,
			// the write pipe returns 0 for all the read pipes
			// that were waiting for it
				printf("child reads 0 ...\n");
			}
		}

		exit(0);
	}

	// parent
	
	// close unusing pipes in parent
	close(p[0][0]);
	close(p[1][1]);
	
	for(i=0;i<2;i++) {
		scanf("%d",&in);
		printf("parent writes %d ...\n", in);
		write(p[0][1], &in, sizeof(int));
		// pipe is automatically BLOCKED until child writes
		// UNIX PIPE PROVIDES AUTOMATIC BLOCKING
		read(p[1][0], &in, sizeof(int));
		printf("parent reads %d ...\n", in);
	}
	// close write pipe and send 0 to child's read pipe
	// which was waiting for this write pipe to write
	close(p[0][1]);

	wait(0);
	exit(0);
}
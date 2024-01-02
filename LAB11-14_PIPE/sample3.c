#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	int i, in ,pid, status, p[2][2];

	for(i=0;i<2;i++)
		pipe(p[i]);

	pid = fork();

	if(pid==0) {
		close(p[0][1]);
		close(p[1][0]);

		fcntl(p[0][0], F_SETFL, O_NONBLOCK);
		
		read(p[0][0], &in, sizeof(int));
		printf("child reads %d ... \n", in);
		in++;
		printf("child writes %d ... \n", in);
		write(p[1][1], &in, sizeof(int));
		
		sleep(5);
		exit(15);
	}

	close(p[0][0]);
	close(p[1][1]);

	scanf("%d", &in);
	printf("parent writes %d ...\n", in);
	write(p[0][1], &in, sizeof(int));
	read(p[1][0], &in, sizeof(int));
	printf("parent reads %d ...\n", in);

	wait(&status);
	if(WIFEXITED(status)) 
		printf("exits ... %d\n", WEXITSTATUS(status));
	else if(WIFSIGNALED(status)) 
		printf("terminated ... %d\n", WTERMSIG(status));

	exit(0);	
}
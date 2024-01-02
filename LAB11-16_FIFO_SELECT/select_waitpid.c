#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MSGSIZE 6

char *msg1 = "hello";
char *msg2 = "bye!!";
void parent(int[][2]);
int child(int[]);

int main(void){
	int pip[3][2];
	int i;

	for (i=0; i<3; i++) {
		pipe(pip[i]);
		if(fork()==0)
			child(pip[i]);
	}

	parent(pip);
	
	for (i=0; i<3; i++) {
		wait(0);
	}

	exit(0);
}

void parent(int p[3][2]){

	char buf[MSGSIZE], ch;
	fd_set set, master;
	int i, j, k;

	for (i=0; i<3; i++) close(p[i][1]);

	FD_ZERO(&master);

	// assign 1 to pipes that we want to check
	// use it like bitmask
	for (i=0; i<3; i++) FD_SET(p[i][0], &master);
	
	// now check "which pipes are on" by using select syscall
	// select returns which pipes are ready
	while (set=master, select(p[2][0]+1, &set, NULL, NULL, NULL) > 0) {
			for (i=0; i<3; i++){
				// if p[i][0] pipe is ready
				if (FD_ISSET(p[i][0], &set)){
					// read
					if (read(p[i][0], buf, MSGSIZE) >0)
						printf("MSG from %d=%s\n", i, buf);
				}
			}	

			// if every child is finished, end parent process
			// if there is a child running, start while again
			if(waitpid(-1,NULL,WNOHANG)==-1) return;
			else{
				printf("child is still running ... \n");
			}
	}
}

int child(int p[2]){
	int cnt;
	close(p[0]);

	for(cnt=0;cnt<2;cnt++) {
		write(p[1], msg1, MSGSIZE);
		sleep(getpid()%4);
	}
	write(p[1], msg2, MSGSIZE);
	exit(0);
}
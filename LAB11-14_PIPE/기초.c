#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void do_child(int id, int p[3][2]){
	int i, in ,pid = getpid();
	
	// 불필요한 pipe 닫기
	// 내 입력pipe 빼고 모두 닫기
	for(i=0;i<3;i++)
		close(p[i][1]);
		if(i!=id)
			close(p[i][0]);


	while(1) {
		// pipe에서 정수 읽기
		read(p[id][0], &in, sizeof(int));

		if(in==-1) 
			exit(0);
		else
			printf("%d %d\n", pid, in);
	}
}

int main(){
	int i, in, pid, p[3][2];

	// pipe 3개 생성
	for(i=0;i<3;i++) pipe(p[i]);

 	for (i=0; i<3; i++){
 		if (fork()==0){
 			do_child(i, p);
			exit(0);
 		}	
 	}

	// parent close all read pipes
	// it  only have to write to childs
	for(i=0;i<3;i++)
		close(p[i][0]);

 	for(i=0; i<12; i++){
 		scanf("%d", &in);
 		// pipe에 in 쓰기
 		write(p[i%3][1], &in, sizeof(int));
 	}

	in=-1;
 
	for(i=0;i<3;i++)
		// -1 전달
		write(p[i][1], &in, sizeof(int));

	for(i=0;i<3;i++) 
		wait(0);

 	exit(0);
}
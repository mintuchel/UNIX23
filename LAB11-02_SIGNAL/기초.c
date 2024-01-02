#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

void catchalarm(int);

int main(void) {

	int i, num, sum = 0;

	static struct sigaction act;
	act.sa_handler = catchalarm;
	sigaction(SIGALRM, &act, NULL);

	for(i=0;i<10;i++) {
		// 10초간 입력이 없으면, SIGALRM이 오도록 설정
		alarm(10);
ㅑ		// 입력받기
		scanf("%d",&num);
		// 입력을 받으면 알람 꺼두기	
		alarm(0);

		sum+=num;
		printf("sum=%d\n", sum);
	}

	exit(0);
}

void catchalarm(int signo) {
	printf("input input input\n");
}
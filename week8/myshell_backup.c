#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <dirent.h>
#include <string.h>

int main(void) {
	char in[50], *res[20]={0};
	int i, status;
	pid_t pid[2] = {0,};

	while (1)
	{
		printf("%s> ", getcwd(in,50));
		gets(in);
		if (in[0] == '\0')
			continue;
		i = 0;
		res[i] = strtok(in, " ");

		while (res[i])
		{
			res[++i] = strtok(NULL, " ");
		}

		if (strcmp(res[0], "exit") == 0)
		{
			// 안전한 종료를 위한 코드 A
			// backup 명령만 기다렸다 끝내기
			// 사실 waitpid 안써도 된다.
			wait(0);
			exit(0);
		}
		// 예외인 cd_m을 위한 코드 B	
		else if (strcmp(res[0], "cd_m") == 0)
		{
			chdir(res[1]);
		}
		else if (strcmp(res[0], "backup_m") == 0)
		{
			// backup 명령 실행중인지 확인 C
			if(pid[1]!=0) {
				// backup_m 명령이 현재 진행 중이 아니면 back_up 명령 실행D
				pid[1] = fork();
				if(pid[1]==0) {
                    // 백업은 사실 waitpid 안해도 된다
					execvp("backup_m", res);
					exit(0);
				}
				// backup은 사실 waitpid 안해줘도 병렬처리되어 돌아감
				// waitpid까지하면 불필요한 코드
			}	
		}
		else if (strcmp(res[0], "cd_m") == 0)
		{
			chdir(res[1]);
		}
		else
		{
			// 그 밖의 다른 명령 실행 E
			// 명령 실행을 위해 필요한 코드 F
			pid[0] = fork();
			if(pid[0]==0) {
               			execvp(res[0], res);
               			exit(0);
			}
			else{
				waitpid(pid[0], &status, 0);
			}
		}
		
		// backup_m을 위한 추가 코드
		// 만약 backup_m이 정상종료되었으면 
		if(waitpid(pid[1],&status,0)==pid[1]) {
			pid[1] = 0;
		}
	}

	return (0);
}

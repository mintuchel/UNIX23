#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <signal.h>

// pid[1] is for backup process which is WNOHANG
// pid[0] is for other process which is 0 
pid_t pid[2] = {0};

void catchalarm(int);

int main(void) {
   char in[50];
   char *res[20]={0};
   int i, status;

   static struct sigaction act;

   // sigignore(SIGINT);
   act.sa_handler = SIG_IGN;
   sigaction(SIGINT, &act, NULL);

   // set alarm for backup process
   act.sa_handler = catchalarm;
   sigaction(SIGALRM, &act, NULL);

   // execute backup_m 
   pid[1] = fork();
   if(pid[1] == 0) {
	execvp("backup_m", NULL);
	exit(0);
   }
              
   while (1)
   {
      printf("> ");
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
	 // (D)
	 if(pid[1] > 0) {
		// (E)
        	waitpid(pid[1], &status, 0);
		// 사실 상 wait(0); 도 가능함.
		// 모든 프로세스는 다 바로 끝내기 때문
		printf("BACKUP 종료 확인 ...\n");
	 }
         exit(0);
      }   
      else if (strcmp(res[0], "cd_m") == 0)
      {
         chdir(res[1]);
      }
      else{
	 // (F)
         pid[0] = fork();
         if(pid[0] == 0) {
			act.sa_handler = SIG_DFL;
			sigaction(SIGINT, &act, NULL);

                        execvp(res[0], res);
                        exit(0);
         }
	 // (G)
         else{
            waitpid(pid[0], &status, 0);
	    if(WIFSIGNALED(status)){
		printf("\n");
	    }
         }
      }

      // (D)
      // if backup has been executed
      if(pid[1] > 0) {
	// (H)
	// and if pid[1] is finished
	if(waitpid(pid[1], &status, WNOHANG)==pid[1]) {
		// (I) change pid[1] val
		pid[1] = 0;
		// and set alarm for the next automatic backup_m
		alarm(20);
      	}
      }
   }
   return 0;
}

// if alarm is called after alarm(20)
// this catches the alarm and execute backup_m automatically
void catchalarm(int signo) {
	pid[1] = fork();
	if(pid[1]==0) {
		char * const av[] = {"backup_m", (char*)0};
		execvp("backup_m", av);
		exit(0);
	}
	return;				
}
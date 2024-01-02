#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <fcntl.h>

union semun{
	int val;
	struct semid_ds *buf;
	ushort *array;
};

int main(void){
	ushort buf[2]= {1, 0};
	int i, j, pid, semid;

	key_t key;
	union semun arg; // used for semctl_SETALL
	struct sembuf p_buf; // used for semop

	key = ftok("key", 5);

	// make semaphore set that have 2 semaphores
	semid = semget(key, 2, 0600 | IPC_CREAT | IPC_EXCL); 
	if (semid==-1){
		 semid = semget(key, 2, 0600);
	}
	else{
 		arg.array = buf;
		semctl(semid, 0, SETALL, arg);
	}

 	pid=getpid();
 	for (i=0; i<3; i++){
		// semwait(A)
		// -1 on #0 semaphore
		// after this process A's semaphore(0) goes into blocking state	
		p_buf.sem_num = 0;
		p_buf.sem_op = -1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);
 
		for (j=0; j<3; j++){
 			printf("%d ... %ld\n", i, pid);
 			sleep(1);
		}

		// semsignal(B)
		// +1 on #1 semaphore
		// this works like a signal to process B
		// process B can now do semaphore operation since 1-1 >= 0
                p_buf.sem_num = 1;
                p_buf.sem_op = 1;
                p_buf.sem_flg = 0;
                semop(semid, &p_buf, 1);	 
 	}
 	return 0;
}
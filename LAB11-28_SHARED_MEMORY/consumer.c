#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

union semun{
        int val;
        struct semid_ds *buf;
        unsigned short * array;
};

struct buffer {
        int f;
        int r;
        char msg[10][512];
};

int main(){
	char in[10], temp[512] ="";
	
    // s = 1, n = 0, e = sizeofbuffer
	ushort buf[3] = { 1, 0, 10};

	int i, fd, n, semid, shmid;
	key_t key1, key2;
	union semun arg;
	struct buffer *msg_buf;
	struct sembuf p_buf;

	key1 = ftok("key", 5);
	semid = semget(key1, 3, 0600);
	arg.array = buf;
	semctl(semid,0 ,SETALL, arg);

	key2 = ftok("key", 2);
	shmid = shmget(key2, sizeof(struct buffer), 0600);
	msg_buf = (struct buffer*)shmat(shmid, 0, 0);

	while(1) {
		scanf("%s", in);

		//semwait(n)
		p_buf.sem_num = 1;
		p_buf.sem_op = -1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);

		// semwait(s)
		p_buf.sem_num = 0;
		p_buf.sem_op = -1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);

		printf("idx = %d : msg = %s\n", msg_buf->f, msg_buf->msg[msg_buf->f]);
		if(!strcmp(msg_buf->msg[msg_buf->f], "quit")) break;

		msg_buf->f = (msg_buf->f + 1) % 10;

		// semsignal(s)
		p_buf.sem_num = 0;
		p_buf.sem_op = 1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);

		// semsignal(e)
		p_buf.sem_num = 2;
		p_buf.sem_op = 1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);
	}

	exit(0);
}

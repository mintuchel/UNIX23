#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct q_entry{
	long mtype;
	int data;
};

int main(){
	int i, qid;
 	key_t key;
 	struct q_entry msg;

 	key = ftok("key", 3);
	// create msgqueue
	qid = msgget(key, 0600 | IPC_CREAT);

	for(i=0;i<15;i++) {
		// 0 : recieve msg in order
		// -3 : recieve msg which id is <= 3
		msgrcv(qid, &msg, sizeof(int), -3, 0);
		printf("#%d recieved type=%d / data=%d\n", i, msg.mtype, msg.data);
		msg.mtype = msg.mtype + 3;
		msg.data = msg.data + 8;
		msgsnd(qid, &msg, sizeof(int), 0);
		printf("#%d send type=%d / data=%d\n", i, msg.mtype, msg.data);
	}
	printf("server done!\n");
	return 0;
}
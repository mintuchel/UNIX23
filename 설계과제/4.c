#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

struct q_entry{
    long mtype;
    int snum;
    int cnum;
    int s_id;
    char msg[512];
};

struct q_entry cmessage(int mtype, int snum, int cnum);
struct q_entry nmessage(int mtype, int s_id, char *str);

struct q_entry cmessage(int mtype, int snum, int cnum){
        struct q_entry msg;

        msg.mtype=mtype;
        msg.snum=snum;
        msg.cnum=cnum;
        msg.s_id=0;
        strcpy(msg.msg, "");

        return msg;
}

struct q_entry nmessage(int mtype, int s_id, char *str){
        struct q_entry msg;

        msg.mtype=mtype;
        msg.snum=0;
        msg.cnum=0;
        msg.s_id=s_id;
        strcpy(msg.msg, str);

        return msg;
}

void do_writer(int qid, int id){
    char temp[512];
    int i, cnum, index;
    struct q_entry msg1, msg2;

    while(1){
        scanf("%s", temp);

        /* (i) message 보내기 전 준비 */
        msgrcv(qid, &msg1, 524, 999, 0);
        cnum = msg1.cnum; // 몇 명한테 보낼지
        index = msg1.snum; // 몇번째 메시지인지
        msg1.snum+=1;
        msgsnd(qid, &msg1, 524, 0);

	msg2=nmessage(index, id, temp);
        for (i=0; i<cnum; i++){ 
            msgsnd(qid, &msg2, 524, 0);
	    sleep(1);
        }

	if (cnum==1) 
		printf("id=%d, talkers=%d, msg#=%d ...\n", id, msg1.cnum, msg1.snum);
        
	if (strcmp(temp, "talk_quit")==0)
                break;
    }

    exit(0);
}

// index 부터 다 받음
void do_reader(int qid, int id, int index){
    struct q_entry msg;

    while(1){
        /* (j) message 받기 */
        msgrcv(qid, &msg, 524, index, 0);
        if (msg.s_id!=id){
            printf("[sender=%d & msg#=%d] %s\n", msg.s_id, msg.mtype, msg.msg);
        }
	/* (c) message 받은 후 필요한 작업 */
        else if(msg.s_id==id){
            if(strcmp(msg.msg, "talk_quit")==0)
                break;
        }
        index++;
    }

    exit(0);
}

int main(int argc, char** argv){
    int i, qid, id, index;
    pid_t pid[2];
    key_t key;
    struct q_entry msg1, msg2;

    key=ftok("key", 5);
    /* (k) message queue 만들고 초기화 작업 */
    qid = msgget(key, 0600 | IPC_CREAT | IPC_EXCL);
    if(qid==-1){
        qid = msgget(key, 0600);
    }else{
	// cmessage(mtype, snum, cnum)
        msg1 = cmessage(999, 1, 0);
        msgsnd(qid, &msg1, 524, 0);
    }

    id=atoi(argv[1]);
    
    /* (l) 통신 전 필요한 작업 */
    // cmsg 최신화 해서 넣기
    msgrcv(qid, &msg1, 524, 999, 0);
    msg1 = cmessage(999, msg1.snum, msg1.cnum + 1);
    index = msg1.snum;
    msgsnd(qid, &msg1, 524, 0);
    printf("id=%d, talkers=%d, msg#=%d ...\n", id, msg1.cnum, msg1.snum);

    /* (d) 함수 호출해서 message 주고 받기 */
    pid[0] = fork();
    if(pid[0]==0){
        do_writer(qid, id);
        exit(0);
    }
    pid[1] = fork();
    if(pid[1]==0){
        do_reader(qid, id, index);
        exit(0);
    }

    for(i=0;i<2;i++) wait(0);

    // cnum 최신화
    msgrcv(qid, &msg1, 524, 999, 0);
    msg1.cnum-=1;
    msgsnd(qid, &msg1, 524, 0);

    /* (h) message 통신 완료 후 message queue 지우기 */
    if(msg1.cnum == 0)
        msgctl(qid, IPC_RMID, NULL);

    exit(0);
}
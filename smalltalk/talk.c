#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>

struct q_entry
{
    long mtype;
    int snum;
    int cnum;
    int s_id;
    char msg[512];
};

struct manage_buffer
{
    int cnum;
    int index;
};

struct q_entry nmessage(int mtype, int s_id, char *str);

void do_writer(int qid, int id, int semid, struct manage_buffer * shmptr)
{
    char buffer[512];
    int i, cnum, index;
    struct q_entry msg;
    struct sembuf p_buf;

    while (1)
    {
        // 문장 입력받기
        fgets(buffer, sizeof(buffer), stdin);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }

        /* (i) message 보내기 전 준비 */
        p_buf.sem_num = 0;
        p_buf.sem_op = -1;
        p_buf.sem_flg = 0;
        semop(semid, &p_buf, 1);

        cnum = shmptr->cnum;
        shmptr->index++;
        index = shmptr->index;

        p_buf.sem_num = 0;
        p_buf.sem_op = 1;
        p_buf.sem_flg = 0;
        semop(semid, &p_buf, 1);

        msg = nmessage(index, id, buffer);
        for (i = 0; i < cnum; i++)
        {
            msgsnd(qid, &msg, 524, 0);
            sleep(1);
        }

        if (cnum == 1)
            printf("id=%d, talkers=%d, msg#=%d ...\n", id, cnum, index);

        if (strcmp(buffer, "talk quit") == 0)
            break;
    }

    exit(0);
}

void do_reader(int qid, int id, int index)
{
    struct q_entry msg;

    while (1)
    {
        /* (j) message 받기 */
        msgrcv(qid, &msg, 524, index, 0);
        if (msg.s_id != id)
        {
            printf("[sender=%d & msg#=%d] %s\n", msg.s_id, msg.mtype, msg.msg);
        }
        /* (c) message 받은 후 필요한 작업 */
        else if (msg.s_id == id)
        {
            if (strcmp(msg.msg, "talk quit") == 0)
                break;
        }

        index++;
    }

    exit(0);
}

int main(int argc, char **argv)
{
    int i, qid, shmid, semid, id, cnum, index;
    pid_t pid[2];
    struct q_entry msg1, msg2;
    struct manage_buffer *shmptr;
    struct sembuf p_buf;

    key_t talk_key = ftok("token", 5);

    // 세마포어 생성/참조
    semid = semget(talk_key, 1, 0600 | IPC_CREAT | IPC_EXCL);
    if (semid == -1)
        semid = semget(talk_key, 1, 0600);
    else
        semctl(semid, 0, SETVAL, 1);

    // 공유메모리 생성/참조
    shmid = shmget(talk_key, sizeof(struct manage_buffer), IPC_CREAT | 0666);
    shmptr = (struct manage_buffer *)shmat(shmid, 0, 0);

    // 메시지큐 생성/참조
    qid = msgget(talk_key, 0600 | IPC_CREAT | IPC_EXCL);
    if (qid == -1) {
        qid = msgget(talk_key, 0600);
    }else{
        shmptr->cnum = 0;
        shmptr->index = 1;
    }

    id = atoi(argv[1]);

    /* (l) 통신 전 필요한 작업 */
    p_buf.sem_num = 0;
    p_buf.sem_op = -1;
    p_buf.sem_flg = 0;
    semop(semid, &p_buf, 1);

    shmptr->cnum += 1;
    cnum = shmptr->cnum;
    index = shmptr->index;

    p_buf.sem_num = 0;
    p_buf.sem_op = 1;
    p_buf.sem_flg = 0;
    semop(semid, &p_buf, 1);

    printf("id=%d, talkers=%d, msg#=%d ...\n", id, cnum, index);

    /* (d) 함수 호출해서 message 주고 받기 */
    pid[0] = fork();
    if (pid[0] == 0)
    {
        do_writer(qid, id, semid, shmptr);
        exit(0); // fork해서 자식이 부모 코드 그대로 물려받고 자식에서 실행됨
    }

    pid[1] = fork();
    if (pid[1] == 0)
    {
        do_reader(qid, id, index);
        exit(0);
    }

    // 자식 프로세스 종료될때까지 wait
    for (i = 0; i < 2; i++)
        wait(0);

    p_buf.sem_num = 0;
    p_buf.sem_op = -1;
    p_buf.sem_flg = 0;
    semop(semid, &p_buf, 1);

    shmptr->cnum -= 1;

    p_buf.sem_num = 0;
    p_buf.sem_op = 1;
    p_buf.sem_flg = 0;
    semop(semid, &p_buf, 1);

    // 채팅방에 아무도 없으면 IPC 객체 모두 삭제
    if (msg1.cnum == 0) {
        msgctl(qid, IPC_RMID, NULL);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, IPC_RMID, NULL);
    }

    // 공유메모리 떼어내기
    shmdt(shmptr);
    exit(0);
}

struct q_entry nmessage(int mtype, int s_id, char *str)
{
    struct q_entry msg;

    msg.mtype = mtype;
    msg.snum = 0;
    msg.cnum = 0;
    msg.s_id = s_id;
    strcpy(msg.msg, str);

    return msg;
}

// pid_t exec_log(int shmid, int semid)
// {
//     char shmid_str[16], semid_str[16];

//     // int를 문자열로 변환
//     snprintf(shmid_str, sizeof(shmid_str), "%d", shmid);
//     snprintf(semid_str, sizeof(semid_str), "%d", semid);

//     pid_t logger_pid = fork();

//     // 자식이면 여기서 진행
//     if (logger_pid == 0)
//     {
//         // execl을 사용하여 다른 프로세스 실행 (num1_str, num2_str 전달)
//         execl("./log", "log", shmid_str, semid_str, (char *)NULL);
//         exit(0);
//     }
//     // 부모면 생성된 pid를 main한테 보냄
//     else
//     {
//         return logger_pid;
//     }
// }
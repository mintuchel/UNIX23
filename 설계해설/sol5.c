#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ftw.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/errno.h>

#define BUFSIZE 512
#define N 5

void debug(struct manager *manager, struct message *message);

// 공유메모리에 대한 정보가 이 구조체로 형변환됨
struct manager
{
    // 유한 버퍼를 원형큐 구조로 사용하기 때문에 idx 정보 필요
    int frontIdx;
    int backIdx;

    int total_user; // 현재 접속인원수
    int id_buffer[4];
};

// 각 메시지에 대한 정보를 담는 구조체
struct message
{
    int read_counter;    // 몇 명이 읽어야하는지. 각 receiver가 읽을때마다 이 값을 -1 시킴
    int message_counter; // 내가 몇번째 메시지인지

    int sender_id;       // 보낸 사람 id
    char mtext[BUFSIZE]; // message 내용
};

void receiver(int id, int semid, int manage_shmid, int message_shmid)
{
    // 1번째에 -1 하여 읽을 메세지가 존재하는지 확인
    // 1번째가 0이면 모든 메세지를 읽었다는 뜻이므로 새 메세지가 들어올때까지 BLOCK

    // 2번째에 -1하여 공유자원에 대한 베타적 접근 여부 확인
    // 2번째가 0이면 누가 사용하고 있다는 것이므로 BLOCK
    struct sembuf wait[] = {{1, -1, 0}, {2, -1, 0}};

    // 0번째에 +1 하여 메세지를 읽었으니 공간 하나 비었다는 의미로 +1
    // 0번째가 0이면 모든 메세지를 읽었다는 뜻이므로 새 메세지가 들어올때까지 BLOCK

    // 2번째에 +1하여 공유자원에 대한 LOCK 반납
    // 2번째가 0이면 누가 사용하고 있다는 것이므로 BLOCK
    struct sembuf signal[] = {{0, 1, 0}, {2, 1, 0}};

    struct sembuf p_buf[] = {{1, 1, 0}, {2, 1, 0}};

    struct manager *manage;
    struct message *message;

    // receiver가 읽어야할 메시지 번호
    // receiver가 처리중인 messsage_counter 번호와 동일한 message 만 읽는다
    int message_counter = 1;

    manage = (struct manager *)shmat(manage_shmid, 0, 0);
    message = (struct message *)shmat(message_shmid, 0, 0);

    while (1)
    {
        // 읽을 메세지 있는지 확인
        // {1,-1,0} {2,-1,0}
        semop(semid, wait, 2);

        // 현재 receiver가 처리하려는 메시지(message_counter) 와 manage->backIdx 위치에 저장된 메시지 번호가 같은지 확인
        // 번호가 일치해야만 읽을 수 있음
        if (message_counter == message[manage->backIdx].message_counter)
        {
            printf("[receiver] %d : %s\n", message[manage->backIdx].sender_id, message[manage->backIdx].mtext);
            message[manage->backIdx].read_counter--; // receiver는 메시지를 읽으면 해당 메시지의 read_counter 값을 감소시킨다
            message_counter++;                       // 그리고 내가 다음에 읽어야할 message_counter를 +1 (읽어야할 메시지 번호 갱신)

            // 만약 내 턴에 read_counter가 0이 된다면 모두 읽었다는 뜻이므로 다음 메시지가 들어갈 idx를 바꿔준다
            // 유한 버퍼이므로 원형 큐처럼 동작
            if (message[manage->backIdx].read_counter == 0)
                manage->backIdx = (manage->backIdx + 1) % N;

            semop(semid, signal, 2);
        }
        // 아직 읽을게 없으면
        else
        {
            // 버퍼 접근 잠금을 해제하고 기다리기 (타 프로세스를 위한 자원 반환)
            semop(semid, p_buf, 2);
        }
    }
    return;
}

// idx0 : 버퍼의 빈 공간
// idx1 : 읽을 메세지 존재성
// idx2 : 공유 자원에 대한 동시 접근 방지 (Mutual Exclusion)
void sender(int id, int semid, int manage_shmid, int message_shmid)
{
    // 0번째에 -1 하여 메세지를 보낼 공간이 있는지 확인
    // 0번째가 0이면 모든 버퍼가 꽉 찼다는 소리이므로 BLOCK

    // 2번째에 -1하여 공유자원에 대한 베타적 접근 여부 확인
    // 2번째가 0이면 누가 사용하고 있다는 것이므로 BLOCK
    struct sembuf wait[] = {{0, -1, 0}, {2, -1, 0}};

    // 1번째에 +1하여 읽을 메시지가 추가되었다고 signal
    // 2번째에 +1하여 공유자원에 대한 배타적 접근 권한 반환
    struct sembuf signal[] = {{1, 1, 0}, {2, 1, 0}};

    struct manager *manage;
    struct message *message;

    manage = (struct manager *)shmat(manage_shmid, 0, 0);
    message = (struct message *)shmat(message_shmid, 0, 0);

    int message_counter = 0;
    char input[BUFSIZE];

    while (gets(input))
    {
        // 메시지를 쓸 빈 공간이 존재하고 + 배타적 접근 권한을 받았으면
        semop(semid, wait, 2);

        // message 구성하기
        message[manage->frontIdx].sender_id = id;
        strcpy(message[manage->frontIdx].mtext, input);
        message_counter += 1;
        message[manage->frontIdx].message_counter = message_counter;
        message[manage->frontIdx].read_counter = manage->total_user;

        // 내 메시지 저장했으니 다음 메시지 저장될 곳 갱신
        // 원형큐이므로 버퍼 내에서 계속 순환하며 저장
        manage->frontIdx = (manage->frontIdx + 1) % N;

        debug(manage, message);

        // 접근권한 반납
        semop(semid, signal, 2);
    }

    return;
}

// sempahore 3개
// shared memory 2개 사용
int main(int argc, char **argv)
{
    int i, id;
    int semid, manage_shmid, message_shmid;
    key_t semkey, manage_shmkey, message_shmkey;
    pid_t pid[2];

    // 세마포어 집합 초기화시 사용할 semun
    union semun arg;

    // 세마포어 초기화 시 사용할 SETALL을 위한 int 배열
    // 1 = prevents overflow (amount of free space left in buffer)
    // 2 = prevents underflow (number of items in the buffer that are available for consumption)
    // 3 = control buffer access (mutual exclusion)
    ushort buf[3] = {N, 0, 1};

    struct manager *manage;
    struct message *message;

    // 필요한 IPC 객체 키 가져오기
    semkey = ftok("semkey", 3);
    manage_shmkey = ftok("manage_shmkey", 3);
    message_shmkey = ftok("message_shmkey", 3);

    // sempahore 3개 생성
    semid = semget(semkey, 3, 0600 | IPC_CREAT | IPC_EXCL);
    // 이미 존재한다면 기존꺼 열기
    if (semid == -1)
    {
        semid = semget(semkey, 3, 0600);
    }
    // 생성에 성공했으면 없었다는 거임
    // semaphore 초기화 해주기
    else
    {
        // semun 과 buf 사용해서 SETALL로 사용할 세마포어 전부 초기화해주기
        arg.array = buf;
        semctl(semid, 0, SETALL, arg);
    }

    // manage_sharedmemory 생성/참조
    manage_shmid = shmget(manage_shmkey, sizeof(struct manager), 0600 | IPC_CREAT);
    manage = (struct manager *)shmat(manage_shmid, 0, 0);

    // N개의 message_buffer 공간을 확보한 shared_memory 생성/참조
    message_shmid = shmget(message_shmkey, N * sizeof(struct message), 0600 | IPC_CREAT);
    message = (struct message *)shmat(message_shmid, 0, 0);

    id = atoi(argv[1]);

    // manage_buffer의 총 사용자수 + 1
    manage->total_user += 1;

    debug(manage, message);

    printf("id : %d\n", id);

    // 톡 읽을 reciever sender process fork로 만들어주기
    // pid = fork();
    // if (pid == 0)
    //     receiver(id, semid, manage_shmid, message_shmid);
    // else
    //     sender(id, semid, manage_shmid, message_shmid);

    // while (waitpid(pid, 0, WNOHANG) == 0)
    // {
    //     sleep(1);
    //     printf("Wait receiver child ... \n");
    // }

    pid[0] = fork();
    if(pid[0]==0){
        receiver(id, semid, manage_shmid, message_shmid);
        exit(0);
    }

    pid[1] = fork();
    if(pid[1]==0){
        sender(id, semid, manage_shmid, message_shmid);
        exit(0);
    }

    for (int i = 0; i < 2;i++)
        wait(0);

    // 특정 사용자가 종료했으면
    // 공유메모리 총 사용자 수 1 감소
    manage->id_buffer[id - 1] = 0;
    manage->total_user -= 1;

    // 만약 한명도 없으면 IPC 객체 모두 삭제
    if (manage->total_user == 0)
    {
        semctl(semid, IPC_RMID, 0);
        shmctl(manage_shmid, IPC_RMID, 0);
        shmctl(message_shmid, IPC_RMID, 0);
    }

    // 정상 종료
    exit(0);
}

void debug(struct manager *manager, struct message *message)
{
    int i;
    printf("[manager] frontIdx : %d, backIdx : %d, users : %d\n", manager->frontIdx, manager->backIdx, manager->total_user);
    printf("[id_buffer] : %d %d %d %d\n", manager->id_buffer[0], manager->id_buffer[1], manager->id_buffer[2],
           manager->id_buffer[3]);

    for (i = 0; i < N; i++)
    {
        printf("[%d : %s] ", (message + i)->sender_id, (message + i)->mtext);
    }
    printf("\n-----------------------------\n");
}
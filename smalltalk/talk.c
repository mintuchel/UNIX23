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
#define N 100

// Producer-Consumer Problem with 3 semaphore(1 mutex)
// N : spaces left in buffer
// 0 : messages that can be read
// 1 : semaphore for mutual exclusion(mutex)

struct manager
{
    int frontIdx;
    int rearIdx;
    int nextId;
    int total_user;
};

struct message
{
    int read_counter;
    int message_id;
    int sender_id;
    char mtext[BUFSIZE];
};

void receiver(int id, int message_id, int semid, int manager_shmid, int message_arr_shmid)
{
    struct sembuf wait[] = {{1, -1, 0}, {2, -1, 0}};
    struct sembuf signal1[] = {{0, 1, 0}, {2, 1, 0}};
    struct sembuf signal2[] = {{1, 1, 0}, {2, 1, 0}};
    struct manager *manager_shm;
    struct message *message_arr_shm;

    manager_shm = (struct manager *)shmat(manager_shmid, 0, 0);
    message_arr_shm = (struct message *)shmat(message_arr_shmid, 0, 0);

    while (1)
    {
        /** critical section **/
        semop(semid, wait, 2);

        if (message_id == message_arr_shm[manager_shm->frontIdx].message_id)
        {
            if (message_arr_shm[manager_shm->frontIdx].sender_id != id)
            {
                printf("[receiver] %d : %s\n", message_arr_shm[manager_shm->frontIdx].sender_id, message_arr_shm[manager_shm->frontIdx].mtext);
            }

            message_arr_shm[manager_shm->frontIdx].read_counter--;
            message_id++;

            if (message_arr_shm[manager_shm->frontIdx].read_counter == 0)
            {
                manager_shm->frontIdx = (manager_shm->frontIdx + 1) % N;
                semop(semid, signal1, 2);
                continue;
            }
        }

        semop(semid, signal2, 2);
        /** critical section **/
    }
    return;
}

void sender(int id, int semid, int manager_shmid, int message_arr_shmid)
{
    struct sembuf wait[] = {{0, -1, 0}, {2, -1, 0}};
    struct sembuf signal[] = {{1, 1, 0}, {2, 1, 0}};
    struct manager *manager_shm;
    struct message *message_arr_shm;

    manager_shm = (struct manager *)shmat(manager_shmid, 0, 0);
    message_arr_shm = (struct message *)shmat(message_arr_shmid, 0, 0);

    char input[BUFSIZE];

    while (gets(input))
    {
        /** critical section **/
        semop(semid, wait, 2);

        message_arr_shm[manager_shm->rearIdx].sender_id = id;
        strcpy(message_arr_shm[manager_shm->rearIdx].mtext, input);
        message_arr_shm[manager_shm->rearIdx].message_id = manager_shm->nextId;
        message_arr_shm[manager_shm->rearIdx].read_counter = manager_shm->total_user;
        manager_shm->nextId++;
        manager_shm->rearIdx = (manager_shm->rearIdx + 1) % N;

        semop(semid, signal, 2);
        /** critical section **/
    }
    return;
}

int main(int argc, char **argv)
{
    int i, id;
    int semid, manager_shmid, message_arr_shmid;
    key_t semkey, manager_shmkey, message_arr_shmkey;
    pid_t pid[2];

    union semun arg;
    ushort buf[3] = {N, 0, 1};
    struct manager *manage;
    struct message *message;

    semkey = ftok("semkey", 3);
    manager_shmkey = ftok("manage_shmkey", 3);
    message_arr_shmkey = ftok("message_shmkey", 3);

    semid = semget(semkey, 3, 0600 | IPC_CREAT | IPC_EXCL);
    if (semid == -1)
    {
        semid = semget(semkey, 3, 0600);
    }
    else
    {
        arg.array = buf;
        semctl(semid, 0, SETALL, arg);
    }

    manager_shmid = shmget(manager_shmkey, sizeof(struct manager), 0600 | IPC_CREAT);
    manage = (struct manager *)shmat(manager_shmid, 0, 0);

    if (manage->total_user == 0)
    {
        manage->frontIdx = 0;
        manage->rearIdx = 0;
        manage->nextId = 1;
        manage->total_user = 0;
    }

    message_arr_shmid = shmget(message_arr_shmkey, N * sizeof(struct message), 0600 | IPC_CREAT);
    message = (struct message *)shmat(message_arr_shmid, 0, 0);

    id = atoi(argv[1]);

    struct sembuf w_buf = {2, -1, 0};
    struct sembuf s_buf = {2, 1, 0};
    int message_id;

    /** critical section **/
    semop(semid, &w_buf, 1);
    manage->total_user += 1;
    message_id = manage->nextId;
    semop(semid, &s_buf, 1);
    /** critical section **/

    printf("id : %d\n", id);

    pid[0] = fork();
    if (pid[0] == 0)
    {
        receiver(id, message_id, semid, manager_shmid, message_arr_shmid);
        exit(0);
    }

    pid[1] = fork();
    if (pid[1] == 0)
    {
        sender(id, semid, manager_shmid, message_arr_shmid);
        exit(0);
    }

    for (int i = 0; i < 2; i++)
    {
        wait(0);
    }

    manage->total_user -= 1;

    if (manage->total_user == 0)
    {
        semctl(semid, IPC_RMID, 0);
        shmctl(manager_shmid, IPC_RMID, 0);
        shmctl(message_arr_shmid, IPC_RMID, 0);
    }

    exit(0);
}
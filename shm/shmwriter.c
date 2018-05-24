#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<signal.h>
#include<fcntl.h>

#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>

int msgid,shmid;
void *shared_Mem=(void *)0;
int *shmaddr;

struct data_str{
    long msg_type;
    pid_t pid;
};


void shardMem()
{    
    int i;
    shmid=shmget((key_t)1234, sizeof(int)*100, 0666 | IPC_CREAT);

    if(shmid==-1)
    {
        fprintf(stderr,"shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_Mem = shmat(shmid, (void *)0,0);
    if(shared_Mem == (void *)-1)
    {
        fprintf(stderr,"shmat failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached\n");
    shmaddr = (int *)shared_Mem;

    for(i=0;i<50;i++)
    {
        *(shmaddr+i)=i+1;
        printf("*shmaddr:%p, data:%d\n", shmaddr+i, *(shmaddr+i));
    }
    
    sleep(1);
}



void sigint(int sig)
{
    printf("relese\n");
}


int main()
{
    struct data_str data;
    pid_t shmr_pid;

    signal(SIGINT,sigint);
   
    msgid=msgget((key_t)1234, 0666 | IPC_CREAT);
    
    data.msg_type=1;
    data.pid=getpid();

// frist write start
// give write pid to reader
    msgsnd(msgid, (void *)&data, sizeof(data), 0);
    printf("shmw signal pid = %d\n",data.pid);
    
    pause();
    
    // reader 메세지 큐 읽기
    msgrcv(msgid, (void *)&data, sizeof(data),0,0);
    shmr_pid=data.pid;
    printf("shmr signal pid : %d\n",shmr_pid);
    msgctl(msgid, IPC_RMID, 0);


    shardMem();
    kill(shmr_pid,SIGUSR1);

    pause();

    shmdt(shared_Mem);

    shmctl(shmid, IPC_RMID,0);


    printf("The end\n");

    exit(EXIT_SUCCESS);

}

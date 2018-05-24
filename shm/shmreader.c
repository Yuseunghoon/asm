#include<stdio.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#define SHMSIZE 50

struct data_str{
    long msg_type;
    pid_t pid;
};


void sigusr1(int sig)
{
    void *shared_Mem=(void *)0;
    int shmid;
    int *shmaddr;
    int i;
  
    printf("i got sigusr1 signal\n");
    
    shmid=shmget((key_t)1234, sizeof(int)*SHMSIZE, 0666 | IPC_CREAT);

    shared_Mem = shmat(shmid,(void *)0,0);

    shmaddr = (int *)shared_Mem;

    sleep(1);

    for(i=0; i<SHMSIZE;i++)
    {
        printf("*shmaddr:%p, data:%d\n", shmaddr+i,*(shmaddr+i)); 
    }

}


int main()
{
    struct data_str data;
    pid_t shmw_pid;
    int msgid;

    signal(SIGUSR1,sigusr1);

    msgid=msgget((key_t)1234, 0666 | IPC_CREAT);
    
    msgrcv(msgid, (void *)&data, sizeof(data),0,0);
    
    shmw_pid=data.pid;
    printf("shmw signal pid = %d\n",data.pid);
    
    data.pid=getpid();
    msgsnd(msgid, (void *)&data, sizeof(data), 0);
    printf("throwing shmr siganl pid : %d\n",data.pid);
    
    kill(shmw_pid,SIGINT);

    pause();

    kill(shmw_pid,SIGINT);


    exit(EXIT_SUCCESS);
}

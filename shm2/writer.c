#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>

#define SHMSIZ 1024

int main()
{
    pid_t pid;
    pid_t pid_c;
    int shmid;
    int i;
    void *shared_Mem=(void *)0;
    int *shmaddr;
    
        // 공유메모리 만들기
        shmid=shmget((key_t)123, sizeof(int)*SHMSIZ, 0666 | IPC_CREAT);
        
        if(shmid==-1)
        {
            fprintf(stderr,"shmeid error why?????\n");
            exit(EXIT_FAILURE);
        }

        // 공유메모리 가져오기
        shared_Mem = shmat(shmid, (void *)0,0);
        if(shared_Mem == (void *)-1)
        {
            fprintf(stderr,"shared_Mem error\n");
            exit(EXIT_FAILURE);
        }

        printf("Memory activated\n");
        
        // 공유메모리 주소 시작지점 파악
        shmaddr = (int *)shared_Mem;

        for(i=0;i<SHMSIZ;i++)
        {
            *(shmaddr+i)=i;
            //printf("shmaddr %p = %d\n",shmaddr+i, *(shmaddr+i));
        }
        printf("Memory writing is done\n");

    pid_c=fork();
    //printf("mine : %d\n",getpid());
    //printf("child : %d\n",pid_c);
    
    if(pid_c==-1)
    {
        fprintf(stderr,"fork error\n");
        exit(EXIT_FAILURE);
    }
    
    //자식프로세서에서 작동
    else if(pid_c==0)
    {
        printf("fork done\n");
        execlp("./reader","reader","",(char *)NULL);
    } 


    else
    {
        //pause();
        //wait(&pid);
        
        while(*(shmaddr+0)==0);

        shmdt(shared_Mem);
        shmctl(shmid, IPC_RMID,0);

    }


    exit(EXIT_SUCCESS);
}


#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<fcntl.h>

#include<sys/types.h>
#include<sys/shm.h>

#define SHMSIZ 1024

void sigint(int sig)
{
    printf("Shared Memory is already");
}

int main()
{
    int shmid;
    int i,fd;
    char buffer[5];
    void *shared_Mem=(void *)0;
    int *shmaddr;
    int a;

    pid_t pid_p=getppid();
    
    signal(SIGINT, sigint);
    

    shmid=shmget((key_t)123, sizeof(int)*SHMSIZ, 0666| IPC_CREAT);
    

    if(shmid==-1)
    {
        fprintf(stderr,"shmid error\n");
        exit(EXIT_FAILURE);
    }

    shared_Mem = shmat(shmid, (void *)0,0);
    if(shared_Mem== (void *)-1)
    {
        fprintf(stderr,"shared_Mem error\n");
        exit(EXIT_FAILURE);
    }

    shmaddr = (int *)shared_Mem;

    fd=open("./a.txt",O_RDWR|O_CREAT | O_TRUNC,S_IRWXU| S_IRWXG| S_IRWXO);

    for(i=0;i<SHMSIZ;i++)
    {
        sprintf(buffer,"%d\n",*(shmaddr+i));
        //printf("buffer input %s",buffer);
        write(fd,buffer,strlen(buffer));
    }
    
    printf("write is finished\n");
    *(shmaddr+0) = 1;//flag로 공유메모리0번지가 1이면 부모 프로세서 가동

    shmdt(shared_Mem);

    close(fd);
    
    //kill(pid_p,SIGINT);

    exit(EXIT_SUCCESS);
}

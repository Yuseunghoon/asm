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
    char buf[SHMSIZ+1];
    void *shared_Mem=(void *)0;
    char *shmaddr;

    pid_t pid_p=getppid();
    
    signal(SIGINT, sigint);
    

    shmid=shmget((key_t)0x1234, sizeof(SHMSIZ), 0666| IPC_CREAT);
    

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

    //shmaddr=(char *)malloc(sizeof(buf));
    shmaddr = shared_Mem;

    printf("open\n");
    fd=open("./b.txt",O_RDWR|O_CREAT | O_TRUNC,S_IRWXU| S_IRWXG| S_IRWXO);
    
    
    //memset(buf,0,sizeof(buf));
    
    while(strcmp((char *)(shmaddr+0),"1"));
    
    while(!strcmp((char *)(shmaddr+2),"0"))
    {
        memset(buf,0,sizeof(buf));

        
        strcpy(buf, (char *)(shmaddr+6));//데이터 읽기
        
        printf("reader shm 0 : %s\n",(char *)(shmaddr+0));
        printf("reader shm 2 : %s\n",(char *)shmaddr+2);
        printf("reader shm 4 : %s\n",(char *)shmaddr+4);
        //printf("%p data : %s\n",shmaddr+6,buf);
        printf("data is being wrote to file\n");

        write(fd,buf,sizeof(buf));
        
        memset(buf,0,sizeof(buf));
        strcpy((char *)(shmaddr+4),"1");//reader flag 읽기완료

       
        if(!strcmp((char *)(shmaddr+2),"1"))// a.txt 파일을 다 읽었으면 종료
            break;

        printf("-----------------reader return\n");
    }       

    printf("write is finished\n");

    //free(shmaddr);   

    shmdt(shared_Mem);


    close(fd);
    

    exit(EXIT_SUCCESS);
}

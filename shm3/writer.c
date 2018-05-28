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
    int i=0,fd;
    char buf[SHMSIZ];
    void *shared_Mem;
    char *shmaddr;

    
    fd=open("./a.txt",O_RDONLY);
    

    
        // 0x1234 아이디의 공유메모리 만들기
        shmid=shmget((key_t)0x1234, sizeof(SHMSIZ)*2, 0666 | IPC_CREAT);
        
        if(shmid==-1)
        {
            fprintf(stderr,"shmeid error why?????\n");
            exit(EXIT_FAILURE);
        }

        // 공유메모리 가져오기
        shared_Mem = shmat(shmid, NULL,0);
        if(shared_Mem == (void *)-1)
        {
            fprintf(stderr,"shared_Mem error\n");
            exit(EXIT_FAILURE);
        }

        printf("Memory is activated\n");
      
    pid_c=fork();
    
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
        shmaddr = shared_Mem;
        //shmaddr = (char *)malloc(sizeof(buf));
        
        strcpy((char *)(shmaddr+0),"0");
        strcpy((char *)(shmaddr+2),"0");
        strcpy((char *)(shmaddr+4),"0");
            
        printf("shm 0 : %s\n",(char *)(shmaddr+0));
        printf("shm 2 : %s\n",(char *)(shmaddr+2));
        printf("shm 4 : %s\n",(char *)(shmaddr+4));

        memset(buf,0,sizeof(buf));

        while(read(fd,buf,sizeof(buf))>0)
        {            
            sprintf((char*)(shmaddr+6),"%s", buf);

            //printf("shmaddr %p = %s\n",shmaddr+6,(char*)(shmaddr+6));
            printf("data is updated\n");

            
            strcpy((char *)(shmaddr+0),"1");//메모리 입력완료

            printf("shm 0 : %s\n",(char *)shmaddr+0);
            printf("shm 2 : %s\n",(char *)shmaddr+2);
            printf("shm 4 : %s\n",(char *)shmaddr+4);
            
            memset(buf,0,sizeof(buf));

            while(!strcmp((char *)shmaddr+4,"0"));//reader에서 메모리확인할 때까지 멈춤

            strcpy((char *)(shmaddr+4),"0");//reader 읽기flag 초기화

            
            printf("\nwriter return!!!!!!!!!!!!!!!!!!!!!!!!!\n");

        }
        printf("Memory writing is done\n");

        strcpy((char *)(shmaddr+2),"1");//파일 읽기 끝
        
        close(fd);
        
        wait(&pid);
        
        //free(shmaddr);

        shmdt(shared_Mem);
        shmctl(shmid, IPC_RMID,0);

    }


    exit(EXIT_SUCCESS);
}


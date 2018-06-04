#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<wait.h>
#include<signal.h>

void sigint(int sig)
{
    printf("blackbox end\n");
    exit(0);
}


int main()
{
    pid_t pid;
    int status;

    FILE *fp;
    
    signal(SIGINT,sigint);

    while(1)
    {
    //make folder with date
    fp = popen("./makefile.sh", "r");
    if(fp<0)
    {
        perror("makefile popen error\n");
        exit(0);
    }
    fclose(fp);
    
    //mem
    
    pid=fork();
    if(pid<0)
    {
        perror("mem fork error\n");
        exit(0);
    }

    if(pid==0)
    {
        execlp("./mem","mem",NULL);
    }

    wait(&status);

    //video
    
    pid=fork();
    if(pid<0)
    {
        perror("video fork error\n");
        exit(0);
    }

    if(pid==0)
    {
        execlp("./video_rec","video",NULL);
    }

    wait(&status);
    }
    

    exit(1);
}

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>

int fd;

void sigHandler(int sig)
{
    static int count=0;
    char buf[10];

    printf("I got signal %d.\n",count++);
        
    switch(sig)
    {
        case SIGUSR1:
            strcpy(buf,"SIGUER1");
            break;

        case SIGUSR2:
            strcpy(buf,"SIGUER2");
            break;

        case SIGINT:
            strcpy(buf,"SIGINT");
            break;

        default: ;
    }

    write(fd,buf,10);
}


int main(int argc, int *argv[])
{
    int i=10;


    fd=open("./test2.txt",O_RDWR | O_CREAT | O_TRUNC, \
                        S_IRWXU | S_IWGRP | S_IRGRP | S_IROTH);
   
    signal(SIGINT, sigHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);
    printf("%d\n",getpid());

    while(i--)
    {
        pause();
    }

    close(fd);

    exit(0);
}

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>

int fd;

void sigHandler(int sig)
{
    static int count=0;
    char buf[100];
    time_t UTCtime;
    struct tm *tm;
    struct timeval UTCtime_u;

    time(&UTCtime);
    tm = localtime(&UTCtime);
    strftime(buf, sizeof(buf),"%A %m %e %h: %y",tm);
   // strcpy(buf,ctime(&UTCtime));

    printf("I got signal %d.\n",count++);
        
    switch(sig)
    {
        case SIGUSR1:
            //strcpy(buf,ctime(&UTCtime));
            //strcat(buf,"SIGUER1 ");
            strcat(buf," SIGUER1\n");
            break;

        case SIGUSR2:
            //strcpy(buf,"SIGUER2 ");
            //strcat(buf,ctime(&UTCtime));
            strcat(buf," SIGUER2\n");
            break;

        case SIGINT:
            //strcpy(buf,"SIGINT ");
            //strcat(buf,ctime(&UTCtime));
            strcat(buf," SIGINT\n");
            break;

        default: ;
    }

    write(fd,buf,strlen(buf));

    buf[0]='\0';
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


void sigHandler(int signo)
{
    static int counter=0;
    counter++;
    printf("signo : %d, counter : %d\n",signo, counter);

}
int main(int argc, char *argv[])
{
	pid_t pid;
    int fd, byteCount;
    char buffer[10];
    char buf[10];
    int i=5;

	// 시그널 핸들러 등록
    signal(SIGINT, sigHandler);

	// pid값을 읽어와서 test.txt파일에 쓰기
    pid=getpid();
    printf("siggen1 pid : %d\n",pid);

    sprintf(buffer,"%d",pid);
    fd = open("./test.txt", O_RDWR | O_CREAT,\
		           S_IRWXU | S_IWGRP | S_IRGRP | S_IROTH);

    read(fd,buf,10);
    close(fd);

    fd = open("./test.txt", O_RDWR | O_CREAT | O_TRUNC, \
		           S_IRWXU | S_IWGRP | S_IRGRP | S_IROTH);
    byteCount = write(fd,buffer,strlen(buffer));
    close(fd);
    
    //pid = atoi(argv[1]);
    pid=atoi(buf);
    printf("siggen1 read_pid : %d\n",pid);
    while(i--)
    {
    printf("send signal proc : %d\n",pid);
    sleep(1);
    kill(pid, SIGINT);

    if(i==0)
        exit(0);

    pause();

    }
    return 0;
}

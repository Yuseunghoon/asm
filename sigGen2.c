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
    int i=5,cnt=0;
	// 시그널 핸들러 등록
    signal(SIGINT, sigHandler);

    if(cnt==0)
    {
        pid=getpid();
        printf("sigGen2 pid : %d\n",pid);

        sprintf(buffer,"%d",pid);
        fd = open("./test.txt", O_RDWR);
        write(fd,buffer,strlen(buffer));
        close(fd);
        cnt=1;
    }
    pause();
	// test.txt파일에 pid값 읽어오기
    fd = open("./test.txt", O_RDWR);
    byteCount = read(fd,buffer,10);
    if(byteCount==0)
		printf("Can't read test.txt file.\n");

    pid = atoi(buffer);
    //printf("pid : %d\n", pid);
    close(fd);
    
    while(i--)
    {
    sleep(1);
    kill(pid, SIGINT);
    
    if(i==0)
        exit(0);

    pause();
    }
    
    return 0;
}

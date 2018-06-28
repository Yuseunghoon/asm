#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define buf_size 10

void error_handling(char *message);

int main(int argc, char **argv)
{
    int fd;
    double T;
    int flag;
    int count;
    char buf[buf_size];
    int i;
    double temp;
    int sock;
    struct sockaddr_in serv_addr;
    int str_len;
    char * testaddr;

    if(argc!=3){
        printf("usage : %s <IP port>\n", argv[0]);
    }
    
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))== -1)
        error_handling("connect() error!");


    while(1)
    {
	memset(buf,0,buf_size);
	// RaspberryPi CPU Temp
        fd = open("/sys/class/thermal/thermal_zone0/temp",O_RDONLY);
        if(fd<2)
            printf("Can not open File\n");
 
        count = read(fd, buf, 5);	   
        //fscanf(fd,"%3f", &T);
        temp = atoi(buf);
        temp /= 1000;
        printf("The temperature is %6.3f C.\n", temp);

        write(sock, &buf, sizeof(buf));
        close(fd);

        sleep(1);
    }

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

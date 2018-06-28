#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <termios.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int getch(void)
{
    int ch;
    struct termios buf, save;
    tcgetattr(0,&save);
    buf=save;
    buf.c_lflag &=~(ICANON|ECHO);
    buf.c_cc[VMIN]=1;
    buf.c_cc[VTIME]=0;
    tcsetattr(0,TCSAFLUSH,&buf);
    ch=getchar();
    tcsetattr(0,TCSAFLUSH,&save);
    return ch;
}

int main(int argc, char *argv[])
{
	int sock;
	//char message[BUF_SIZE];
	char ch;
	int str_len;
	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	
	while(1) 
	{
		//fputs("Input message(Q to quit): ", stdout);
		//fgets(message, BUF_SIZE, stdin);
		
		//if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
		//	break;
		
		ch=getch();
		
		printf("ch %c\n",ch);
		write(sock, &ch, sizeof(ch));
		
		//str_len=read(sock, message, BUF_SIZE-1);
		//message[str_len]=0;
		//printf("Message from server: %s", message);
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
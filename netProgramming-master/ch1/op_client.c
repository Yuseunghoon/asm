#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "cal.h"

void error_handling(char *message);

int main(int argc, char* argv[])
{
	struct cal c1;
	int sock;
	int i;
	int temp;
	struct sockaddr_in serv_addr;
	char message[30];
	int str_len;
	char * testAddr;
	
	// 프로그램 실행시 IP번지와 PORT번호를 설정했는지 확인
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	// STEP 1. 소켓을 생성한다.
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	// 서버의 주소값과 포트 번화를 설정한다.
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
		
	testAddr = inet_ntoa(serv_addr.sin_addr);
	printf("serv_addr = %s\n",testAddr);

	// STEP 2. 서버에 연결을 요청한다.
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
		error_handling("connect() error!");
	
	printf("operand num:");
	scanf("%d",&temp);
	printf("operation:");
	scanf("%c",&c1.op);
	scanf("%c",&c1.op);
	c1.num = (char)temp;
	for(i=0; i<c1.num; i++)
	{
		printf("Operand %d: ", i+1);
		scanf("%d", &c1.operand[i]);
	}



	// STEP 3. 서버로 데이터를 송신한다.
	str_len = write(sock, &c1, sizeof(c1));
	if(str_len==-1)
		error_handling("read() error!");

	// STEP 4. 서버와의 연결을 닫는다.
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

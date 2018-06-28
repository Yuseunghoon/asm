#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "cal.h"


void error_handling(char *message);

int main(int argc, char *argv[])
{
	struct cal c1;
	int serv_sock;
	int clnt_sock;
	int result;
	int i;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;


	// 클라이언트로 전송할 메시지 문자열
	//char message[]="Hello World!";
	
	// 실행시 PORT번호가 설정되었는지 확인
	// PORT번호가 없다면 프로그램 종료
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	// STEP 1. 소켓을 생성한다.
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	
	// 서버 자신의 주소값과 포트 번호값을 serv_addr에 저장한다.
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	// STEP 2. 서버 소켓과 서버의 주소를 연결한다.
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )
		error_handling("bind() error"); 
	
	// 서버 소켓 대기열의 크기를 설정한다.
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	// 클라이언트 주소 구조체의 크기를 설정한다.
	clnt_addr_size=sizeof(clnt_addr);  

	// STEP 3. 클라이언트로부터 접속요청을 처리한다.
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
	if(clnt_sock==-1)
		error_handling("accept() error");  
	
	// STEP 4. 데이터 송수신
	// 접속된 클라이언트로부터 데이터(메시지 문자열)를 수신한다.
	read(clnt_sock, &c1, sizeof(c1));

	result=0;
	printf("op:%c\n",c1.op);
	for(i=0;i<c1.num;i++)
	{
		printf("operand1:%d\n",c1.operand[i]);

		if(i==0)
		{
			result=c1.operand[0];
		}
		else
		{
			switch(c1.op)
			{
				case '+':
					result+=c1.operand[i];
					break;
				case '-':
					result-=c1.operand[i];
					break;
				default:
					break;
			}
		}
	}

	printf("result=%d\n",result);

	// STEP 5. 소켓을 닫는다.
	// 클라이언트와 서버의 소켓을 순서대로 닫는다.
	close(clnt_sock);	
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

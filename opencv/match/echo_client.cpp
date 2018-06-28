#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <termios.h>
#include "opencv2/opencv.hpp"

#define BUF_SIZE 1024
void error_handling(char *message);

int port_all;
char *ip_all;
using namespace cv;

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

void *openthread(void *aa)
{
	    //--------------------------------------------------------
    //networking stuff: socket , connect
    //--------------------------------------------------------
    int         sokt;
    char*       serverIP;
    int         serverPort;

    serverIP   = ip_all;
    serverPort = port_all+1;

    struct  sockaddr_in serverAddr;
    socklen_t           addrLen = sizeof(struct sockaddr_in);

    if ((sokt = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket() failed" << std::endl;
    }

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);

    if (connect(sokt, (sockaddr*)&serverAddr, addrLen) < 0) {
        std::cerr << "connect() failed!" << std::endl;
    }



    //----------------------------------------------------------
    //OpenCV Code
    //----------------------------------------------------------

    Mat img;
    img = Mat::zeros(480 , 640, CV_8UC1);    
    int imgSize = img.total() * img.elemSize();
    uchar *iptr = img.data;
    int bytes = 0;
    int key;

    //make img continuos
    if ( ! img.isContinuous() ) { 
          img = img.clone();
    }
        
    std::cout << "Image Size:" << imgSize << std::endl;

    namedWindow("CV Video Client",1);

    while (key != 'q') {

        if ((bytes = recv(sokt, iptr, imgSize , MSG_WAITALL)) == -1) {
            std::cerr << "recv failed, received bytes = " << bytes << std::endl;
        }
        
        cv::imshow("CV Video Client", img); 
      
        if (key = cv::waitKey(10) >= 0) break;
    }   

    close(sokt);

    return 0;
}

int main(int argc, char *argv[])
{
	int sock;
	//char message[BUF_SIZE];
	char ch;
	int str_len;
	struct sockaddr_in serv_adr;
	pthread_t open;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	port_all=atoi(argv[2]);
	ip_all=argv[1];
	
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
	
	pthread_create(&open,NULL,openthread,(void *)NULL);

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

	pthread_join(open, NULL);
	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
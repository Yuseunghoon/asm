#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdint.h>
#include <termio.h>
#include <wiringPi.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include "opencv2/opencv.hpp"

#define I2C_DEV "/dev/i2c-1"
#define CLOCK_FREQ 25000000.0
#define PCA_ADDR 0x40
#define LED_STEP 50

// Register Addr
#define MODE1 0x00
#define MODE2 0x01
#define PRE_SCALE 0xFE
#define LED15_ON_L 0x42
#define LED15_ON_H 0x43
#define LED15_OFF_L 0x44
#define LED15_OFF_H 0x45

#define LED14_ON_L 0x3E
#define LED14_ON_H 0x3F
#define LED14_OFF_L 0x40
#define LED14_OFF_H 0x41

#define LED13_ON_L 0x3A
#define LED13_ON_H 0x3B
#define LED13_OFF_L 0x3C
#define LED13_OFF_H 0x4D

//5
#define SPEED_L_ON_L 0x1A
#define SPEED_L_ON_H 0x1B
#define SPEED_L_OFF_L 0x1C
#define SPEED_L_OFF_H 0x1D

//4
#define SPEED_R_ON_L 0x16
#define SPEED_R_ON_H 0x17
#define SPEED_R_OFF_L 0x18
#define SPEED_R_OFF_H 0x19

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;


char msg[BUF_SIZE];
int fd;
unsigned char buffer[3]={0};
char key;

char *ip_all;
int port_all;
using namespace cv;

void *display(void *);

int capDev = 0;
VideoCapture cap(capDev); 


int reg_read8(unsigned char addr)
{
    buffer[0]=addr;
    int length = 1;
    if(write(fd,buffer,length)!=length)
    {
        printf("Failed to write from the i2c bus\n");
    }

    if(read(fd,buffer,length)!=length)
    {
        printf("Failed to read from the i2c bus\n");
    }
    printf("addr[%d] = %d\n",addr,buffer[0]);

    return 0;
}

int reg_write8(unsigned char addr, unsigned short data)
{
    int length=2;
    
    buffer[0]=addr;
    buffer[1]=data;

    if(write(fd,buffer,length)!=length)
    {
        printf("Failed to write form the i2c bus\n");
        return -1;
    }
    return 0;
}

int pca9685_restart(void)
{
    int length;
    
    reg_write8(MODE1, 0x00);    
    reg_write8(MODE2, 0x04);

    return 0;

    /*
    buffer[0] = MODE1;
    buffer[1] = 0x00;
    length = 2;

    // [S][PCA_ADDR][W][ACK][MODE1][ACK][0x00][ACK][p] 
    // MODE1 레지스터에 0x00을 쓴다.

    if(write(fd,buffer,length) != length)
    {
        printf("Failed to write from the i2c bus\n");
        return 0;
    }
    else
    {
        printf("Data Mode1 %x\n",biffer[0]);
    }
               
    buffer[0] = MODE2;
    buffer[1] = 0x04;
    length = 2;

    // MODE2 레지스터에 0x04을 쓴다.

    if(write(fd,buffer,length) != length)
    {
        printf("Failed to write from the i2c bus\n");
        return -1;
    }
    else
    {
        printf("Data Mode2 %x\n",biffer[1]);
    }
    */
}

int pca9685_freq()
{
    int length = 2, freq = 50;
    uint8_t pre_val = (CLOCK_FREQ / 4096/ freq)-1;
    printf("pre_val = %d\n",pre_val);

    // OP : OSC OFF
    reg_write8(MODE1, 0x10);
    // OP : WRITE PRE_SCALE VALUE
    reg_write8(PRE_SCALE,pre_val);
    // OP : RESTART
    reg_write8(MODE1, 0x80);
    // OP : TOTEM POLE
    reg_write8(MODE2, 0x04);

    return 0;

    /*
    // OP : OSC OFF
    buffer[0]=MODE1;
    buffer[1]=0x10;
    if(write(fd,buffer,length)!=length)
    {
        printf("Failed to write form the i2c bus\n");
        return -1;
    }

    // OP : WRITE PRE_SCALE VALUE
    buffer[0]=PRE_SCALE;
    buffer[1]=prescale_val;
    if(write(fd,buffer,length)!=length)
    {
        printf("Failed to write form the i2c bus\n");
        return -1;
    }

    // OP : RESTART
    buffer[0]=MODE1;
    buffer[1]=0x80;
    if(write(fd,buffer,length)!=length)
    {
        printf("Failed to write form the i2c bus\n");
        return -1;
    }

    // OP : TOTEM POLE
    buffer[0]=MODE2;
    buffer[1]=0x04;
    if(write(fd,buffer,length)!=length)
    {
        printf("Failed to write form the i2c bus\n");
        return -1;
    }
    */
}


int reg_read16(unsigned char addr)
{
    unsigned short temp;
    reg_read8(addr);
    temp = 0xff & buffer[0];
    reg_read8(addr+1);
    temp |= (buffer[0]<<8);
    printf("addr=0x%x, data=0x%x\n",addr, temp);

    return 0;
}

int reg_write16(unsigned char addr, unsigned short data)
{
    int length =2;
    reg_write8(addr,(data & 0xff));
    reg_write8(addr+1,(data>>8) &0xff);

    return 0;
}


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

void *led_on(void)
{
    unsigned short time_val=4000;
    int i;
    unsigned short wheel,cam,reset,speed;
    // left 205, rigth 409
    unsigned short zero=307, left=150, right=750;
    //char key;
    char sw=1;
    wheel=zero, cam=zero, speed=zero;

    reg_write16(LED13_ON_L,0);
    reg_write16(LED13_OFF_L,zero);
    reg_write16(LED14_ON_L,0);
    reg_write16(LED14_OFF_L,zero);
    reg_write16(LED15_ON_L,0);
    reg_write16(LED15_OFF_L,zero);


    while(key != 'b')
    {
        //printf("key = %c, speed = %d, wheel = %d, sw = %d\n",msg[0],speed,wheel,sw);
        //printf("key insert : ");
        //key=getch();
        
        switch(msg[0])
        {
            // wheel 14
            case 'a':
                if(wheel>=left)
                {
                    wheel-=LED_STEP;
                    reg_write16(LED14_ON_L,0);
                    reg_write16(LED14_OFF_L,wheel);
                }
                else if(wheel<left)
                {
                    printf("wheel left value is Maximum\n");
                    wheel=left;
                }
                break;

            case 'd':
                if(wheel<=right)
                {
                    wheel+=LED_STEP;
                    reg_write16(LED14_ON_L,0);
                    reg_write16(LED14_OFF_L,wheel);
                }
                else if(wheel>right)
                {
                    printf("wheel right value is Maximum\n");
                    wheel=right;
                }

                break;
            
            // cam up
            case 65:
                if(cam<right)
                {
                    cam+=LED_STEP;
                    reg_write16(LED15_ON_L,0);
                    reg_write16(LED15_OFF_L,cam);
                }
                else if(cam>left)
                {
                    printf("Cam up value is Maximum\n");
                    cam-=LED_STEP*2;
                }

                break;
        
            // cam down
            case 66:
                if(cam>left)
                {
                    cam-=LED_STEP;
                    reg_write16(LED15_ON_L,0);
                    reg_write16(LED15_OFF_L,cam);
                }
                else if(cam<left)
                {
                    printf("Cam down value is Maximum\n");
                    cam+=LED_STEP;
                }
                break;

            //reset
            case 'r':
                cam=zero,wheel=zero,speed=zero;

                reg_write16(LED13_ON_L,0);
                reg_write16(LED13_OFF_L,zero);
                reg_write16(LED14_ON_L,0);
                reg_write16(LED14_OFF_L,zero);
                reg_write16(LED15_ON_L,0);
                reg_write16(LED15_OFF_L,zero);
                reg_write16(SPEED_R_ON_L,0);
                reg_write16(SPEED_R_OFF_L,zero);
                reg_write16(SPEED_L_ON_L,0);
                reg_write16(SPEED_L_OFF_L,zero);
                                
                break;

            case 'f':
                reg_write8(MODE1, 0x10);
                printf("sleep\n");
                break;

            case 'o':
                reg_write8(MODE1, 0x80);
                printf("wake up\n");
                break;
            
            case 'q':
                if(sw==0)
                {
                    sw=1;
                    digitalWrite(0,0);
                    digitalWrite(2,0);
                }
                else if(sw==1)
                {
                    sw=0;
                    digitalWrite(0,1);
                    digitalWrite(2,1);
                }

                break;
            // sw=1 forword, sw=0 back
            // speed up ^
            case 'w':
                if(sw==1)
                {
                    if(zero<=speed && speed<=time_val)
                    {
                        speed+=LED_STEP;
                    }
                
                    else if(speed>time_val)
                    {
                        printf("speed up value is Maximum\n");
                        speed=time_val;
                    }
                }
                else if(sw==0)
                {
                    if(zero<speed)
                    {
                        speed-=LED_STEP;
                    }
                    else
                    {
                        sw=1;
                        speed=zero;
                    }
                }

                break;
        
            // speed down
            case 's':
                if(sw == 1)
                {
                    if(speed>=zero)
                    {
                        speed-=LED_STEP;
                            if(speed<zero)
                                sw=0;
                    }
                }
                else if(sw==0)
                {
                    printf("go to back\n");
                    if(speed<=time_val)
                    {
                        speed+=LED_STEP;
                    }
                    else if(speed>time_val)
                    {
                        printf("speed up value is Maximum\n");
                        speed=time_val;
                    }
                }
                break;

           default: ;
        }
        
		memset(msg,0,sizeof(msg));
        //wheel
        //reg_write16(LED14_ON_L,0);
        //reg_write16(LED14_OFF_L,wheel);
        // speed
        reg_write16(SPEED_R_ON_L,0);
        reg_write16(SPEED_R_OFF_L,speed);
        reg_write16(SPEED_L_ON_L,0);
        reg_write16(SPEED_L_OFF_L,speed);
                
        if(sw==1)
        {
            digitalWrite(0,0);
            digitalWrite(2,0);
        }
        else if(sw==0)
        {
            digitalWrite(0,1);
            digitalWrite(2,1);
        }
    }
    
}

void *TCP(void *arg)
{
	char *port= (char *)arg;
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	pthread_t t_id;
  
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(port));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
}

void *openthread(void * aa)
{
//--------------------------------------------------------
    //networking stuff: socket, bind, listen
    //--------------------------------------------------------
    int localSocket, remoteSocket, port = 4097;                               
    struct  sockaddr_in localAddr, remoteAddr;
    pthread_t thread_id;
    
    int addrLen = sizeof(struct sockaddr_in);
/*   
    if ( (argc > 1) && (strcmp(argv[1],"-h") == 0) ) {
          std::cerr << "usage: ./cv_video_srv [port] [capture device]\n" <<
                       "port           : socket port (4097 default)\n" <<
                       "capture device : (0 default)\n" << std::endl;

          exit(1);
    }
*/
    port = port_all+1;

    localSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (localSocket == -1){
         perror("socket() call failed!!");
    }    

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons( port );

    if( bind(localSocket,(struct sockaddr *)&localAddr , sizeof(localAddr)) < 0) {
         perror("Can't bind() socket");
         exit(1);
    }
    
    //Listening
    listen(localSocket , 3);
    
    std::cout <<  "Waiting for connections...\n"
              <<  "Server Port:" << port << std::endl;

    //accept connection from an incoming client
    while(1){
    //if (remoteSocket < 0) {
    //    perror("accept failed!");
    //    exit(1);
    //}
       
     remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);  
      //std::cout << remoteSocket<< "32"<< std::endl;
    if (remoteSocket < 0) {
        perror("accept failed!");
        exit(1);
    } 
    std::cout << "Connection accepted" << std::endl;
     pthread_create(&thread_id,NULL,display,&remoteSocket);

     //pthread_join(thread_id,NULL);

    }
    //pthread_join(thread_id,NULL);
    //close(remoteSocket);

    return 0;
}

void *display(void *ptr){
    int socket = *(int *)ptr;
    //OpenCV Code
    //----------------------------------------------------------

    Mat img, imgGray;
    img = Mat::zeros(480 , 640, CV_8UC1);   
     //make it continuous
    if (!img.isContinuous()) {
        img = img.clone();
    }

    int imgSize = img.total() * img.elemSize();
    int bytes = 0;
    int key;
    

    //make img continuos
    if ( ! img.isContinuous() ) { 
          img = img.clone();
          imgGray = img.clone();
    }
        
    std::cout << "Image Size:" << imgSize << std::endl;

    while(1) {
                
            /* get a frame from camera */
                cap >> img;
            
                //do video processing here 
                cvtColor(img, imgGray, CV_BGR2GRAY);

                //send processed image
                if ((bytes = send(socket, imgGray.data, imgSize, 0)) < 0){
                     std::cerr << "bytes = " << bytes << std::endl;
                     break;
                } 
    }

}

int main(int argc, char *argv[])
{
	pthread_t id_key, id_TCP, id_open;
    
    wiringPiSetup();
    pinMode(0, OUTPUT);   
    pinMode(2, OUTPUT);

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

    if((fd=open(I2C_DEV,O_RDWR))<0)
    { 
        printf("Failed open i2c-1 bus\n");
        return -1;
    }       

    if(ioctl(fd,I2C_SLAVE,PCA_ADDR)<0)
    {
        printf("Failed to acquire bus access and/or talk to slave\n");
        return -1;
    }

	port_all=atoi(argv[1]);

	//pthread_create(&id_key,NULL,led_on,NULL);
    pthread_create(&id_TCP,NULL,TCP,(void *)argv[1]);
	pthread_create(&id_open,NULL,openthread,(void *)NULL);

    pca9685_restart();
    pca9685_freq();
    led_on();

	pthread_join(id_key,NULL);
	pthread_join(id_TCP,NULL);

    return 0;
    
}

void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	//char msg[BUF_SIZE];
	
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
	{
		//send_msg(msg, str_len);
		printf("key = %c\n",msg[0]);
	}
	
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}
void send_msg(char * msg, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
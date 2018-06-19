#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdint.h>
#include <termio.h>

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

//12
#define SPEED_L_ON_L 0x36
#define SPEED_L_ON_H 0x37
#define SPEED_L_OFF_L 0x38
#define SPEED_L_OFF_H 0x39

//11
#define SPEED_R_ON_L 0x32
#define SPEED_R_ON_H 0x33
#define SPEED_R_OFF_L 0x34
#define SPEED_R_OFF_H 0x35

int fd;
unsigned char buffer[3]={0};

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

int blinkLED(void)
{
    int i;
    unsigned short value;
    unsigned short max=4095;

    unsigned short zero=307, left=205, right=409;


    while(1)
    {
        for(i=0;i<max;i+=5)
        {
            if(i>1024)
                i+=15;
            
            reg_write16(LED15_ON_L,max-i);
            reg_read16(LED15_ON_L);
            reg_write16(LED15_OFF_L,max);
            reg_read16(LED15_OFF_L);
            
            reg_write16(LED14_ON_L,max-i);
            reg_read16(LED14_ON_L);
            reg_write16(LED14_OFF_L,max);
            reg_read16(LED14_OFF_L);
            
            reg_write16(LED13_ON_L,max-i);
            reg_read16(LED13_ON_L);
            reg_write16(LED13_OFF_L,max);
            reg_read16(LED13_OFF_L);
            usleep(200);
        }

        for(i=0;i<max;i+=5)
        {
            if(i>1024)
                i+=15;
            
            reg_write16(LED15_ON_L,i);
            reg_read16(LED15_ON_L);
            reg_write16(LED15_OFF_L,max);
            reg_read16(LED15_OFF_L);
            
            reg_write16(LED14_ON_L,i);
            reg_read16(LED14_ON_L);
            reg_write16(LED14_OFF_L,max);
            reg_read16(LED14_OFF_L);
            
            reg_write16(LED13_ON_L,i);
            reg_read16(LED13_ON_L);
            reg_write16(LED13_OFF_L,max);
            reg_read16(LED13_OFF_L);
            usleep(200);
        }
    }
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

int led_on(unsigned short value)
{
    unsigned short time_val=4095;
    unsigned short max=time_val;
    int i;
    unsigned short wheel,cam,reset,speed;
    // left 205, rigth 409
    unsigned short zero=307, left=150, right=750;
    char key;
    wheel=zero, cam=zero;

    reg_write16(LED13_ON_L,0);
    reg_write16(LED13_OFF_L,zero);
    reg_write16(LED14_ON_L,0);
    reg_write16(LED14_OFF_L,zero);
    reg_write16(LED15_ON_L,0);
    reg_write16(LED15_OFF_L,zero);


    while(key != 'b')
    {
        //printf("key insert : ");
        key=getch();
        
        switch(key)
        {
            case 'a':
                if(wheel<left)
                {
                    wheel-=LED_STEP;
                    reg_write16(LED14_ON_L,0);
                    reg_write16(LED14_OFF_L,wheel);
                }
                else if(wheel>left)
                {
                    printf("wheel left value is Maximum\n");
                    wheel+=LED_STEP*2;
                }
                break;

            case 'd':
                if(wheel<right)
                {
                    wheel+=LED_STEP;
                    reg_write16(LED14_ON_L,0);
                    reg_write16(LED14_OFF_L,wheel);
                }
                else if(wheel>right)
                {
                    printf("wheel right value is Maximum\n");
                    wheel-=LED_STEP*2;
                }

                break;
            
            // cam up
            case 'w':
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
            case 's':
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
                cam=zero,wheel,speed=zero;

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
                break;

            case 'o':
                reg_write8(MODE1, 0x80);
                break;
            
            // speed up ^
            case 65:
                if(speed<time_val)
                {
                    speed+=LED_STEP;
                    reg_write16(SPEED_R_ON_L,0);
                    reg_write16(SPEED_R_OFF_L,speed);
                    reg_write16(SPEED_L_ON_L,0);
                    reg_write16(SPEED_L_OFF_L,speed);
                }
                else if(speed>time_val)
                {
                    printf("speed up value is Maximum\n");
                    speed=time_val-LED_STEP;
                }

                break;
        
            // speed  down
            case 66:
                if(speed>0)
                {
                    speed-=LED_STEP;
                    reg_write16(SPEED_R_ON_L,0);
                    reg_write16(SPEED_R_OFF_L,speed);
                    reg_write16(SPEED_L_ON_L,0);
                    reg_write16(SPEED_L_OFF_L,speed);
                }
                else if(speed<0)
                {
                    printf("speed down value is Maximum\n");
                    speed+=LED_STEP;
                }
                break;

           default: ;
        }

    }
    
}

int main(void)
{
    int value = 2047;

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

    pca9685_restart();
    pca9685_freq();
    led_on(value);
    //blinkLED();

    return 0;
    
}

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdint.h>

#define I2C_DEV "/dev/i2c-1"
#define CLOCK_FREQ 25000000.0
#define PCA_ADDR 0x40
#define LED_STEP 100

// Register Addr
#define MODE1 0x00
#define MODE2 0x01
#define PRE_SCALE 0xFE
#define LED15_ON_L 0x42
#define LED15_ON_H 0x43
#define LED15_OFF_L 0x44
#define LED15_OFF_H 0x45

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
    int length = 2, freq = 10;
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
    while(1)
    {
        for(i=0;i<max;i+=5)
        {
            //if(i>1024)
            //    i+=15;
            reg_write16(LED15_ON_L,max-i);
            reg_read16(LED15_ON_L);
            reg_write16(LED15_OFF_L,max);
            reg_read16(LED15_OFF_L);
            usleep(20);
        }

        for(i=0;i<max;i+=5)
        {
            //if(i>1024)
            //    i+=15;
            reg_write16(LED15_ON_L,i);
            reg_read16(LED15_ON_L);
            reg_write16(LED15_OFF_L,max);
            reg_read16(LED15_OFF_L);
            usleep(20);
        }
    }
}

int led_on(unsigned short value)
{
    unsigned short time_val=4095;
    char key;
    while(key != 'b')
    {
        printf("key insert : ");
        key=getchar();
        if(key=='a')
        {
            if(value<3800)
            {
                value+=LED_STEP;
                reg_write16(LED15_ON_L,time_val-value);
                reg_read16(LED15_ON_L);
                reg_write16(LED15_OFF_L,value);
                reg_read16(LED15_OFF_L);
            }

            else
            {
                printf("Overflow\n");
            }
        }
        else if(key=='s')
        {            
            if(value>LED_STEP)
            {
                value-=LED_STEP;
                reg_write16(LED15_ON_L,0);
                reg_read16(LED15_ON_L);
                reg_write16(LED15_OFF_L,value);
                reg_read16(LED15_OFF_L);
            }

            else
            {
                printf("Underflow\n");
            }

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
    //led_on(value);
    blinkLED();

    return 0;
    
}

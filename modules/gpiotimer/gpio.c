#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#define BUF_SIZE 100
#define DEVICE_ROUTE "/dev/gpioled"
#define MODULE_CMD "sudo insmod /home/pi/asm/modules/gpiotimer/gpioirq_module.ko"
#define MKNOD_CMD "sudo mknod /dev/gpioled c 200 0"
#define CHMOD_CMD "sudo chmod 666 /dev/gpioled"


char buf[BUF_SIZE];
int fd = -1;

void signal_handler(int signum)
{
    int count;

    printf("user app : signal is catched\n");
    if(signum==SIGIO)
    {
        count = read(fd, buf, 20);
        printf("sig = %s\n",buf);
        
    }

    return;
}


int main(int argc, char **argv)
{
    int count;

    memset(buf, 0, BUF_SIZE);

    signal(SIGIO,signal_handler);

    printf("GPIO Set : %s\n", argv[1]);

    while(0 > (fd = open(DEVICE_ROUTE, O_RDWR)))
    {
        switch(errno)
        {
            case ENXIO://6
                system(MODULE_CMD);
                break;
            case ENOENT://2
                system(MKNOD_CMD);
                system(CHMOD_CMD);
                break;
            default:
                printf("Error : open failed %d \n", errno);
                return -1;
        }
    }
    
    sprintf(buf,"%s:%d",argv[1],getpid());
    count = write(fd, buf, strlen(buf));

    printf("device opened()\n");

    if(count < 0)
    {
        printf("Error : write failed %d \n", errno);
        return -1;
    }

    count = read(fd, buf, 20);
    printf("Read data : %s\n", buf);

    while(1);
    
    close(fd);

    printf("device closed()\n");

    return 0;
}

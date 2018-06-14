#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define BUF_SIZE 100
#define DEVICE_ROUTE "/dev/gpioled"
#define MODULE_CMD "sudo insmod /home/pi/asm/yj/gpio_module.ko"
#define MKNOD_CMD "sudo mknod /dev/gpioled c 200 0"
#define CHMOD_CMD "sudo chmod 666 /dev/gpioled"

int main(int argc, char **argv)
{
    char buf[BUF_SIZE];
    int fd = -1;
    int count;

    memset(buf, 0, BUF_SIZE);

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
    
    printf("device opened()\n");

    count = write(fd, argv[1], strlen(argv[1]));
    if(count < 0)
    {
        printf("Error : write failed %d \n", errno);
        return -1;
    }

    count = read(fd, buf, 20);
    printf("Read data : %s\n", buf);

    close(fd);

    printf("device closed()\n");

    sleep(1);

    return 0;
}

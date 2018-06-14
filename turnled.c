#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/mman.h>

#define SW 17
#define LED 27

//Raspi3 PHYSICAL I/O peri base addr
#define BCM_IO_BASE 0x3f000000

//Raspi3 GPIO BASE ADDR
#define GPIO_BASE (BCM_IO_BASE+0x200000)
#define GPFSEL0 *(gpio+1)
#define GPFSEL1 *(gpio+2)

//GPIO MODE SELECT
#define GPIO_IN(g) (*(gpio+((g)/10))&=~(7<<(((g)%10)*3)))
#define GPIO_OUT(g) (*(gpio+((g)/10))&=(1<<(((g)%10)*3)))

//#define GPIO_SET(g) (*gpio+7)=(1<<g)

//GPIO Pin Output SET / CLR

#define GPIO_SIZE 0xB4

volatile unsigned int *gpio;

int GPIO_GET(int gno)
{
	int value;

	if(gno>31)
		value=*(gpio+14)&(1<<(gno-32));	
	else
		value=*(gpio+13)=(1<<(gno));
	
	return value;
}

int GPIO_CLR(int gno)
{
	if(gno>31)
		*(gpio+11)=(1<<(gno-32));	
	else
		*(gpio+10)=(1<<(gno));
	
	return 0;
}

int GPIO_SET(int gno)
{
	if(gno>31)
		*(gpio+8)=(1<<(gno-32));	
	else
		*(gpio+7)=(1<<(gno));
	
	return 0;
}

int main(int argc, char **argv)
{
	int gno, i, mem_fd;
	int regValue;
	void *gpio_map;

	/*
	if(argc<2)
	{
		printf("Error : %s GPIO_NO\n",argv[0]);
		return -1;
	}
	*/

	//gno=atoi(argv[1]);

	//device open /dev/mem
	if((mem_fd=open("/dev/mem",O_RDWR |O_SYNC))<0)
	{
		printf("Error : open() /dev/mem\n");
		return -1;
	}

	gpio_map = mmap(NULL,GPIO_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,mem_fd,GPIO_BASE);

	if(gpio_map==MAP_FAILED)
	{
		printf("Error : mmap() %d\n",(int)gpio_map);
		return -1;
	}

	gpio = (volatile unsigned int *)gpio_map;
	
	/*
	regValue=*(gpio+1);
	printf("GPFSEL1 = 0x%x\n",regValue);
	regValue = regValue&0xFF1FFFFF;
	*(gpio+1)=regValue;
	printf("GPFSEL1 = 0x%x\n",regValue);
	*/

	regValue=GPIO_IN(SW);
	GPIO_OUT(LED);
	
	for(i=0;i<5;i++)
	{	
		GPIO_GET(SW);
		printf("SW Value = %d\n",regValue);
		GPIO_SET(LED);
		usleep(500000); //500ms
		GPIO_CLR(LED);
		usleep(500000); //500ms
	}

	munmap(gpio_map,GPIO_SIZE);

	close(mem_fd);
	
	return 0;

}

#include<stdio.h>
#include<unistd.h>
#include<wiringPi.h>
#include<stdlib.h>

int ledControl(int gpio1, int gpio2)
{
	int i;

	//gpio : pint number, OUTPUT : assign as Output
	pinMode(gpio1, OUTPUT);
	pinMode(gpio2, OUTPUT);

	for(i=0;i<5;i++)
	{
		digitalWrite(gpio1, HIGH);
		usleep(500000); //500ms
		digitalWrite(gpio1, LOW);
		
		digitalWrite(gpio2, HIGH);
		usleep(500000); //500ms
		digitalWrite(gpio2, LOW);
		usleep(500000); //500ms
		
	}

	return 0;
}

int main(int argc, char **argv)
{
	int gpioNum[2];
	int sw;

	wiringPiSetup();// the first write it!
	if(argc<2)
	{
		printf("Error : %s GPIONum\n",argv[0]);
		return -1;
	}
	
	
	//pinMode(24,INPUT);
	
	//digitalRead(24);

	/*
	while((sw=digitalRead(24))==0)
	{
		printf("%d\n",sw);
		sleep(1);
	}
	*/

	gpioNum[0] = atoi(argv[1]);
	gpioNum[1] = atoi(argv[2]);
	ledControl(gpioNum[0], gpioNum[1]);
	return 0;
}

#include<wiringPi.h>
#include<stdio.h>

#define SW 0 //p17 -> wiringPi 0
#define LED 2 //p27 -> wiringPi 2

// define Pin function
int MyPinSetup(void)
{
	pinMode(SW,INPUT);
	pinMode(LED,OUTPUT);
	return 0;
}

int switchcontrol(void)
{
	while(1)
	{
	int value=digitalRead(SW);
	printf("value = %d\n",value);

	if(value==HIGH)
		digitalWrite(LED,HIGH);
	else
		digitalWrite(LED,LOW);
	
	usleep(100000);
	}
}

int main(void)
{
	wiringPisetup();
	MypinSetup();
	switchcontrol();

	return 0;
}

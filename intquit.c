#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>

static void sigHandler(int sig)
{
	static int count = 0;
	{
		count++;
		printf("Caught SIGINT(%d)\n", count);
		return;
	}

	printf("Caught SIGOUT - that's all folks!\n");
	exit(0);
}


int main(int argc, char *argv[])
{
	if(signal( SIGINT, sigHandler) == SIG_ERR)
		exit(-1);
	
	if(signal( SIGQUIT, sigHandler) == SIG_ERR)
		exit(-1);

	for(;;)
		pause();

}

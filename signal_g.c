#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<time.h>
#include<string.h>

int main(int argc, char *argv[])
{
    int r;
    int sig;
    int i;
    pid_t pid=atoi(argv[1]);

    srand(time(NULL));

    for(i=0;i<10;i++)
    {
        r=rand()%3;

        printf("rand : %d\n",r);

        switch(r)
        {
            case 0:
                sig=SIGUSR1;
                break;

            case 1:
                sig=SIGUSR2;
                break;

            case 2:
                sig=SIGINT;
                break;

            default: ;

        }

        printf("sig: %d\n",sig);

        kill(pid,sig);

        sleep(1);
    }


}

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

static int count = 0;
static pthread_mutex_t countlock = PTHREAD_MUTEX_INITIALIZER;

void *increment(void*arg)
{
    int error;
    while(1)
    {
        pthread_mutex_lock(&countlock);
        printf("increment %d\n",count);
        count++;
        pthread_mutex_unlock(&countlock);
        sleep(1);
    }

    return 0;
}

void *decrement(void*arg)
{
    int error;

    while(1)
    {
        pthread_mutex_lock(&countlock);
        printf("decrement %d\n", count);
        count--;
        pthread_mutex_unlock(&countlock);
        sleep(2);
    }

    return 0;
}
void *getcount(void *countp)
{
    int error;
    while(1)
    {
        pthread_mutex_lock(&countlock);
        
        printf("getcount %d\n",count);
        countp=(void *)&count;
        pthread_mutex_unlock(&countlock);
        sleep(3);
    }

    return 0;
}

int main()
{
    pthread_t p_thread[3];
    int value[3];
    int err;
    int a;

    if((err=pthread_create(&p_thread[0],NULL,increment,(void *)NULL))<0)
    {
        perror("thread 0 error\n");
        exit(1);
    }

    if((err=pthread_create(&p_thread[1],NULL,decrement,(void *)NULL))<0)
    {
        perror("thread 1 error\n");
        exit(1);
    }

    if((err=pthread_create(&p_thread[2],NULL,getcount,(void *)NULL))<0)
    {
        perror("thread 2 error\n");
        exit(1);
    }


    pthread_detach(p_thread[0]);
    pthread_detach(p_thread[1]);
    pthread_detach(p_thread[2]);

    pause();
}

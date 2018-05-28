#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>

#include<sys/time.h>

struct thd{
    long long start;
    long long end;
};


int sum()
{
    int i,sum;

    for(i=0;i<200000;i++)
    {
        sum+=i;
    }
    
    return sum;
}


void *t_function(void * data)
{
    int id;
    int i=0;
    int a =1;
    int sum_t;
    pthread_t t_id;
    id =*((int *)data);
    t_id=pthread_self();
    
    sum_t=sum();


    return 0;
}


int main(int argc[], char *argv[])
{
    pthread_t p_thread[2];
    int err;
    int a,b;
    int status1, status2;
    struct thd thd_data;

    switch(atoi(argv[1]))
    {
        case 1:

            printf("1\n");
            if(err=pthread_create(&p_thread[0],NULL, t_function, (void *)&a)<0)
            {
                perror("thread create error 0\n");
                exit(1);
            }

            pthread_join(p_thread[0],(void **)&status);
            
            break;

        case 2:

            if(err=pthread_create(&p_thread[0],NULL, t_function, (void *)&a)<0)
            {
                perror("thread create error 0\n");
                exit(1);
            }

            if(err=pthread_create(&p_thread[1],NULL, t_function, (void *)&b)<0)
            {
                perror("thread create error 1\n");
                exit(1);
            }
            
            pthread_join(p_thread[0],(void **)&status);
            pthread_join(p_thread[1],(void **)&status2);
            
            break;

        default: ;
    }

    printf("thd sum%d\n",status);
       
   return 0;
}

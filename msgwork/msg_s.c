#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

struct msg_data{
    long int msg_type;
    char name[10];
    int age;
    int id;
};

int main()
{
    struct msg_data msg;
    int msgid;
    char buffer[10];
    char end[2];
    int running =1;

    msgid=msgget((key_t)1234, 0666 | IPC_CREAT);

    if(msgid == -1)
    {
        fprintf(stderr,"msgid error : %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while(running)
    {
        printf("put the name : ");
        msg.msg_type=1;
        scanf("%s",&msg.name);


        printf("put the age : ");
        scanf("%d",&msg.age);

        msg.id++;
        
        printf("conutine(y/n) : ");
        scanf("%s",&end); 
        
        if(strncmp(end,"n",1)==0)
        {
            running=0;
            msg.msg_type=2;
        }

        if(msgsnd(msgid, (void *)&msg, sizeof(msg), 0)==-1)
        {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);

}

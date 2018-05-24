#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

struct msg_data{
    long int msg_type;
    char name[10];
    int age;
    int id;
    char end[2];
};

int main()
{
    int running=1;
    int msgid;
    int fd;
    char buf[10];
    struct msg_data msg;
    long int msg_to_receive=0;

    msgid=msgget((key_t)1234, 0666 | IPC_CREAT);
    fd=open("./msgdata.txt",O_RDWR| O_CREAT| O_TRUNC,\
            S_IRWXU| S_IRWXG| S_IRWXO);

    if(msgid == -1)
    {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while(running)
    {
        if(msgrcv(msgid, (void *)&msg, sizeof(msg), msg_to_receive, 0)==-1)
        {
            fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            exit(EXIT_FAILURE);
        }
        printf("Name : %s\n",msg.name);
        write(fd,msg.name,sizeof(msg.name));
        printf("Age : %d\n",msg.age);
        sprintf(buf,"%d",msg.age);
        write(fd,buf,sizeof(buf));
        printf("Id : %d\n",msg.id);
        sprintf(buf,"%d",msg.id);
        write(fd,buf,sizeof(buf));
       


        if(msg.msg_type==2)
            running=0;

    }

    close(fd);

    if(msgctl(msgid, IPC_RMID, 0)==-1)
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

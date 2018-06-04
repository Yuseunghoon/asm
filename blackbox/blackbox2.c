#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<wait.h>
#include<signal.h>
#include<time.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/time.h>

void sigint(int sig)
{
    printf("blackbox end\n");
    exit(0);
}


int main()
{
    pid_t pid;
    int status;
    time_t UTCtime;
    struct tm *tm;

    FILE *fp;
    char cmd[BUFSIZ];   
    char time_num[BUFSIZ];
    char filename[BUFSIZ];
    char buf[BUFSIZ];
    char mkdirname[BUFSIZ];

while(1)
{
    time(&UTCtime);
    
    memset(cmd,'\0',sizeof(cmd));
    memset(time_num,'\0',sizeof(time_num));
    memset(buf,'\0',sizeof(time_num));

    time(&UTCtime); 
    tm = localtime(&UTCtime);
    strftime(time_num,sizeof(time_num),"%Y%m%d_%H%M%S", tm);
    strncpy(filename,time_num,11);


    //mem
    
    pid=fork();
    if(pid<0)
    {
        perror("mem fork error\n");
        exit(0);
    }

    if(pid==0)
    {
        execlp("./mem","mem",NULL);
    }

    wait(&status);


    //make folder 
   
    int makefile = mkdir(filename,0766);
    printf("filename %s\n",filename);
    
    if(makefile==0)
    {
        printf("%s file had made successful\n",filename);
    }
    else if(makefile==-1)
    {
        printf("%s file is already existing\n",filename);
    }

    //video
    
    
    sprintf(cmd,"raspivid -o /home/pi/asm/blackbox/%s/%s.h264 -w 1280 -h 720 -t 60000",filename,time_num);


    printf("cmd : %s\n",cmd);
    printf("time : %s\n",time_num);
    fp = popen(cmd,"r");
    if(fp<0)
    {
        perror("makefile popen error\n");
        exit(1);
    }
    fclose(fp);
    printf("==============================================\n\n");
}

    exit(1);
}

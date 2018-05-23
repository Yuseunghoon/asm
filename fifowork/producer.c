#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<limits.h>
#include<unistd.h>

#define BUFFER_SIZE PIPE_BUF
#define TEN_MEG 256
#define FIFO_NAME "/tmp/fifo"

int main()
{
    int pipe_fd;
    int bytes_sent=0;
    int res,fd,cnt;
    char buffer[BUFFER_SIZE +1];

    printf("Process %d opening FIFO O_WRONLY\n",getpid());
    if(access(FIFO_NAME, F_OK)== -1){
        res = mkfifo(FIFO_NAME, 0777);
        if(res !=0){
            fprintf(stderr, "Could not create fifo %s\n", FIFO_NAME);
            exit(EXIT_FAILURE);
        }
    }
    
    fd=open("./doc.txt", O_RDONLY, S_IRUSR, S_IRGRP, S_IROTH);
    pipe_fd = open(FIFO_NAME, O_WRONLY);

    printf("Process %d result %d\n",getpid(),pipe_fd);
        if(pipe_fd != -1){
            while(bytes_sent < TEN_MEG){

                cnt=read(fd,buffer,sizeof(buffer));
                printf("reading the txt\n%s\n",buffer);
                
                res=write(pipe_fd, buffer, sizeof(buffer));
                if(res==-1){
                    fprintf(stderr, "Wrtie error on pipe\n");
                    exit(EXIT_FAILURE);
                }

                bytes_sent += res;
            }
 
            close(fd);
            (void)close(pipe_fd);
        }

}

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<limits.h>
#include<unistd.h>

#define BUFFER_SIZE PIPE_BUF
#define FIFO_NAME "/tmp/fifo"
int main()
{
        int pipe_fd, res, fd;
        int doc1,doc2;
        int doc1_r, doc2_r;
        int open_mode = O_RDONLY;
        int cnt=0;
        char buffer[BUFFER_SIZE + 1];
        char buffer_doc1[BUFFER_SIZE + 1];
        char buffer_doc2[BUFFER_SIZE + 1];
        int bytes_read = 0;
        memset(buffer, '\0', sizeof(buffer));
        printf("Process %d opening FIFO O_RDONLY\n", getpid());
        if (access(FIFO_NAME, F_OK) == -1) 
        {
           res = mkfifo(FIFO_NAME, 0777);
           if (res != 0) 
           {
              fprintf(stderr, "Could not create fifo %s\n", FIFO_NAME);
              exit(EXIT_FAILURE);
           }
        }
   
        pipe_fd = open(FIFO_NAME, open_mode);
        fd=open("./doc2.txt",O_RDWR | O_CREAT | O_TRUNC,\
                            S_IRWXU | S_IWGRP | S_IWGRP | S_IROTH);


        printf("Process %d result %d\n", getpid(), pipe_fd);
        if (pipe_fd != -1) 
        {
            while((res = read(pipe_fd, buffer, sizeof(buffer)))>0);
            {
            
                printf("%s\n",buffer);
                write(fd, buffer, sizeof(buffer));

                bytes_read += res;
               
            }
      
            close(fd);
            (void)close(pipe_fd);
        }
        else 
        {
            exit(EXIT_FAILURE);
        }

         doc1=open("./doc.txt",O_RDONLY);
    
         doc2=open("./doc2.txt",O_RDONLY);

         while((doc1_r=read(doc1,buffer_doc1, sizeof(buffer_doc1)))>0)
         {
            doc2_r=read(doc2,buffer_doc2, sizeof(buffer_doc2));
            if(strcmp(buffer_doc1,buffer_doc2)==0)
                printf("same\n");
            else if(strcmp(buffer_doc1,buffer_doc2)!=0)
                printf("not same\n");
            
         }

}

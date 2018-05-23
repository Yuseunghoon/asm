#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define SIZE 1024

int main(int argc, char *argv[])
{
	char buf[SIZE];
	int inputFd, outputFd;
	int len;
    time_t UTCtime;
    struct timeval UTCtime_u;
    struct timeval UTCtime_b;
    clock_t st, et;

	if(argc < 3){
		write(2, "Usage : copy file1 file2\n", 25);//2는 에러에 작성
		//perror("Usage : copy file1 file2\n");//에러 번호용 출력함수
		return -1;
	}

    gettimeofday(&UTCtime_b, NULL);
        printf("gettimeofday : %ld/%f\n", UTCtime_b.tv_sec, UTCtime_b.tv_usec/1000000.0);
	st=clock();
    inputFd = open(argv[1], O_RDONLY);
	outputFd = open(argv[2], \
		O_WRONLY | O_TRUNC | O_CREAT, \
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	
	if( outputFd < 0 || inputFd < 0)
		printf("file open error!");

	while((len = read(inputFd, buf, sizeof(buf))) > 0)
    {
        write(outputFd, buf, len);
    }
	close(inputFd); 
	close(outputFd);
    et=clock();
	gettimeofday(&UTCtime_u, NULL);
        printf("gettimeofday : %ld/%f\n", UTCtime_u.tv_sec-UTCtime_b.tv_sec, (UTCtime_u.tv_usec-UTCtime_b.tv_usec)/1000000.0);
	
    printf("clock :%d(%d clock per second.)\n",et-st, CLOCKS_PER_SEC);
    printf("clock -> sec = %lf\n",(float)(et-st)/CLOCKS_PER_SEC);
        
        
    return 0;
}

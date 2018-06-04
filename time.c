#include<stdio.h>
#include<time.h>
#include<sys/time.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
    int i,j;
    time_t UTCtime;
    struct tm *tm;
    char buf[BUFSIZ];
    struct timeval UTCtime_u;

    time(&UTCtime);//UTC 현재 시간 구하기
    printf("time : %u\n", (unsigned)UTCtime);//UTC 현재 시간 출력

    gettimeofday(&UTCtime_u, NULL);//UTC 현재 시간 구하기 (마이크로초까지)
    printf("gettimeofday : %ld/%d\n", UTCtime_u.tv_sec, UTCtime_u.tv_usec);

    printf("ctime : %s", ctime(&UTCtime));


    //putenv("TZ=PST3PDT");//환경변수를 설정한다
    //tzset();TZ변수를 설정한다
    tm = localtime(&UTCtime);//tm = gmtime(&UTCtime)
    printf("asctime : %s", asctime(tm));//현재의 시간을 tm 구조체를 이용해서 출력
    strftime(buf, sizeof(buf), "%A %m %e %H:%M:%S %Y", tm);//사용자 정의 문자열 정지
    //날짜 일로 %e는 01은 \1, %d는 01로 나온다.
    printf("strftime : %s\n", buf);
    
    strftime(buf, sizeof(buf), "%Y%H%M%S", tm);//사용자 정의 문자열 정지
    printf("atoi : %d\n",atoi(buf));
    return 0;
    
}

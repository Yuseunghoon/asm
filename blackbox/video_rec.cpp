#include <time.h>
#include <iostream>  
#include "opencv2/opencv.hpp"  
#include <sys/time.h>
#include <stdio.h>
#include <signal.h>

using namespace cv;
using namespace std;

void signal(int sig)
{
    printf("blackbox end\n");
    exit(0);
}
 
int main()
{
    time_t UTCtime;
    struct tm *tm;
    char buf_s[BUFSIZ];
    char buf_e[BUFSIZ];
    char name[BUFSIZ];
    char pwd[BUFSIZ];

    time(&UTCtime);

    //웹캡으로 부터 데이터 읽어오기 위해 준비  
    VideoCapture video(0);
    if (!video.isOpened())
    {
        cout << "웹캠을 열수 없습니다." << endl;
        return 1;
    }
 
    Mat frame;
 
    namedWindow("input", 1);
 
 
    //웹캠에서 캡쳐되는 이미지 크기를 가져옴   
    Size size = Size((int)video.get(CAP_PROP_FRAME_WIDTH),
        (int)video.get(CAP_PROP_FRAME_HEIGHT));
    video.set(CAP_PROP_FPS, 30.0);
 
    //파일로 동영상을 저장하기 위한 준비

    VideoWriter outputVideo;

    tm= localtime(&UTCtime);
    strftime(buf_s,sizeof(buf_s),"%H%M%S", tm);
    strftime(name,sizeof(name),"%Y%m%e_%H%M%S", tm);
    strcat(name,".mpg");

    memset(pwd,'\0',sizeof(pwd));
    strncat(pwd,name,11);
    strcat(pwd,"/");
    strcat(pwd,name);
    
    printf("pwd : %s\n",pwd);
    printf("file name : %s\n",name);
    printf("buf_s : %s\n",buf_s);

    outputVideo.open(pwd, VideoWriter::fourcc('X', 'V', 'I', 'D'),
        30, size, true);
    if (!outputVideo.isOpened())
    {
        cout << "동영상을 저장하기 위한 초기화 작업 중 에러 발생" << endl;
        return 1;
    }

 

    while (1)
    {
        
        //웹캡으로부터 한 프레임을 읽어옴  
        video >> frame;
 
        //웹캠에서 캡처되는 속도를 가져옴
        int fps = video.get(CAP_PROP_FPS);
        int wait = int(1.0 / fps * 1000);
 
        cout << "fps : " << fps << endl;
 
        //화면에 영상을 보여줌
        imshow("input", frame);
 
        //동영상 파일에 한프레임을 저장함.  
        outputVideo << frame;
 
        //ESC키 누르면 종료
        if (waitKey(wait) == 27) break;
        
       time(&UTCtime);
       tm= localtime(&UTCtime);
       strftime(buf_e,sizeof(buf_e),"%H%M%S",tm);
       printf("end time = %s\n",buf_e);
       int start = atoi(buf_s);
       int end = atoi(buf_e);

       printf("strat %d\n",start);
       printf("end %d\n",end);
          
       if(end-start>=20)
        {
            printf("finished\n");
            break;
        }
        
    }
 
 
    return 0;
}

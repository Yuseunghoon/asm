#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <sys/types.h>

const char *MMOUNT = "/proc/mounts";

struct f_size
{
    long blocks;
    long avail; 
};

typedef struct _mountinfo 
{
    FILE *fp;                // 파일 스트림 포인터    
    char devname[80];        // 장치 이름
    char mountdir[80];        // 마운트 디렉토리 이름
    char fstype[12];        // 파일 시스템 타입
    struct f_size size;        // 파일 시스템의 총크기/사용율 
} MOUNTP;

MOUNTP *dfopen()
{
    MOUNTP *MP;

    // /proc/mounts 파일을 연다.
    MP = (MOUNTP *)malloc(sizeof(MOUNTP));
    if(!(MP->fp = fopen(MMOUNT, "r")))
    {
        return NULL;
    }
    else
        return MP;
}

MOUNTP *dfget(MOUNTP *MP)
{
    char buf[256];
    char *bname;
    char null[16];
    struct statfs lstatfs;
    struct stat lstat; 
    int is_root = 0;

    // /proc/mounts로 부터 마운트된 파티션의 정보를 얻어온다.
    while(fgets(buf, 255, MP->fp))
    {
        is_root = 0;
        sscanf(buf, "%s%s%s",MP->devname, MP->mountdir, MP->fstype);
         if (strcmp(MP->mountdir,"/") == 0) is_root=1;
        if (stat(MP->devname, &lstat) == 0 || is_root)
        {
            if (strstr(buf, MP->mountdir) && S_ISBLK(lstat.st_mode) || is_root)
            {
                // 파일시스템의 총 할당된 크기와 사용량을 구한다.        
                statfs(MP->mountdir, &lstatfs);
                MP->size.blocks = lstatfs.f_blocks * (lstatfs.f_bsize/1024); 
                MP->size.avail  = lstatfs.f_bavail * (lstatfs.f_bsize/1024); 
                return MP;
            }
        }
    }
    rewind(MP->fp);
    return NULL;
}

int dfclose(MOUNTP *MP)
{
    fclose(MP->fp);
}

int main()
{
    MOUNTP *MP;
    FILE *fp;
    char buf[BUFSIZ];
    char rm[BUFSIZ];
    float per;

    if ((MP=dfopen()) == NULL)
    {
        perror("error");
        return 1;
    }

   
        while(dfget(MP))
        {
            /*printf("%-14s%-20s%10lu%10lu\n", MP->mountdir, MP->devname, 
                                MP->size.blocks,
                                MP->size.avail);
            */
            if(strcmp(MP->devname,"/dev/root")==0)
            {   
                per = 100* (float)(MP->size.avail)/MP->size.blocks;
                printf("memory percent %f%\n",per);
                
                if(per<10)
                {
                    printf("###### memory waring\n");
                    fp=popen("ls -d */","r");
                    if(fp<0)
                    {
                        perror("ls error\n");
                        exit(1);
                    }

                    fgets(buf,12,fp);
                    fclose(fp);

                    sprintf(rm,"rm -r %s",buf);
                    printf("###### remove file %s\n",buf);

                    fp=popen(rm,"r");
                    if(fp<0)
                    {
                        perror("rm error\n");
                        exit(1);
                    }

                    break;

                }
                else
                {
                    printf("memory is enough\n");
                    break;
                }
            }
        }
        //printf("=========================\n\n");

        fclose(fp);
       
       
}

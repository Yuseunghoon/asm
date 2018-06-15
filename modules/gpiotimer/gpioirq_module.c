#include <linux/fs.h>           // open(), read(), write(), close()
#include <linux/cdev.h>         // register_chardev_region(), cdev_init()
#include <linux/module.h>       
#include <linux/io.h>           // ioremap(), iounmap()
#include <linux/uaccess.h>      // copy_from_user(), copy_to_user()
#include <linux/gpio.h>         // request_gpio(), gpio_set_value, gpio_get_value()
#include <linux/interrupt.h>    // gpio_to_irq(), request_riq()
#include <linux/timer.h>        // init_timer(), mod_timer(), del_timer(), add_timer()
//#include <asm/siginfo.h>        // siginfo 구조체를 사용하기위해
#include <linux/sched/signal.h>

#define GPIO_MAJOR 200
#define GPIO_MINOR 0
#define GPIO_DEVICE "gpioled"

//Raspi 0, 1 PHYSICAL I/O PERI BASE ADDR
//#define BCM_IO_BASE 0x20000000

//Raspi 3 PHYSICAL I/O PERI BASE ADDR
#define BCM_IO_BASE 0x3F000000
#define GPIO_BASE (BCM_IO_BASE + 0x200000)
#define GPIO_SIZE 0xB4

#define GPIO_IN(g) (*(gpio+((g)/10))&=~(7<<(((g)%10)*3)))
#define GPIO_OUT(g) (*(gpio+((g)/10))|=(1<<(((g)%10)*3)))

#define GPIO_SET(g) (*(gpio+7) = (1<<g))
#define GPIO_CLR(g) (*(gpio+10) = (1<<g))
#define GPIO_GET(g) (*(gpio+13)&(1<<g))
#define GPIO_LED 17
#define GPIO_LED2 27
#define GPIO_SW 13
#define GPIO_SW2 6
#define BUF_SIZE 100

static char msg[BUF_SIZE] = {0};
static int switch_irq;
static int switch_irq2;
static struct timer_list timer; // timer 구조체
static struct task_struct *task; // 태스크를 위한 구조체

pid_t pid;
char pid_valid;
int key_value;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("YSH");
MODULE_DESCRIPTION("Raspberry Pi First Device Driver");

struct cdev gpio_cdev;

static int gpio_open(struct inode *inod, struct file *fil);
static int gpio_close(struct inode *inod, struct file *fil);
static ssize_t gpio_write(struct file *inode, const char *buff, size_t len, loff_t *off);
static ssize_t gpio_read(struct file *inode, char *buff, size_t len, loff_t *off);

typedef struct Data_{
    char data;
}Data;

static struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .read = gpio_read,
    .write = gpio_write,
    .open = gpio_open,
    .release = gpio_close,
};

volatile unsigned int *gpio;

static void timer_func(unsigned long data)
{
    printk(KERN_INFO "timer_func:%ld\n",data);
    
    gpio_set_value(GPIO_LED,data);
    gpio_set_value(GPIO_LED2,!data);
    if(data)
        timer.data=0;
    else
        timer.data=1;
    timer.expires=jiffies+(1*HZ);
    add_timer(&timer);
}

static irqreturn_t isr_func(int irq, void *data)
{
    static int count;

    
    static struct siginfo sinfo;
    memset(&sinfo,0,sizeof(struct siginfo));
    sinfo.si_signo = SIGIO;
    sinfo.si_code = SI_USER;

    send_sig_info(SIGIO,&sinfo,task);

    if(irq==switch_irq)
    {
        key_value=10;
    }
    else if(irq==switch_irq2)
    {
        key_value=20;
    }

    /*
    // IRQ발생 && LED가 OFF일때
    if(irq==switch_irq && !gpio_get_value(GPIO_LED))
    {
        gpio_set_value(GPIO_LED,1);
        
        static struct siginfo sinfo;
        memset(&sinfo,0,sizeof(struct siginfo));
        sinfo.si_signo = SIGIO;
        sinfo.si_code = SI_USER;

        task = pid_task(find_vpid(pid),PIDTYPE_PID);
        if(task != NULL)
        {
            send_sig_info(SIGIO,&sinfo,task);
        }
        else
        {
            printk("Error: I don't know user pid\n");
        }

        
    }
        
    // IRQ발생 && LED ON일때
    else if(irq==switch_irq && gpio_get_value(GPIO_LED) && !gpio_get_value(GPIO_LED2)) 
        gpio_set_value(GPIO_LED2,1);
    else
    {
        gpio_set_value(GPIO_LED,0);
        gpio_set_value(GPIO_LED2,0);
    }
    */

    printk(KERN_INFO " called isr_func():%d\n",count);
    count++;
    return IRQ_HANDLED;
}


/*
int GPIO_SET(int g)
{
    if(g >31)
        *(gpio + 8) = (1 << (g - 31));
    else
        *(gpio + 7) = (1 << (g));

    return g;
}
*/
static int gpio_open(struct inode *inod, struct file *fil)
{
    try_module_get(THIS_MODULE);
    printk(KERN_INFO "GPIO Device opened()\n");
    return 0;
}

static int gpio_close(struct inode *inod, struct file *fil)
{
    module_put(THIS_MODULE);
    printk(KERN_INFO "GPIO Device Closed()\n");
    return 0;
}

static ssize_t gpio_write(struct file *inode, const char *buff, size_t len, loff_t *off)
{
    short count;
    char *cmd, *str;
    char *sep = ":";
    char *endptr, *pidstr;

    memset(msg, 0, BUF_SIZE);

    count = copy_from_user(msg, buff, len);

    str=kstrdup(msg, GFP_KERNEL);
    cmd = strsep(&str, sep);
    cmd[1]='\0';
    pidstr = strsep(&str, sep);
    printk("Command : %s, Pid : %s\n", cmd,pidstr);
/*
    if((!strcmp(msg,"0")))
    {
        del_timer_sync(&timer);
    }
    else
    {
        init_timer(&timer);
        timer.function=timer_func; // expire시 호출하는 함수
        timer.data = 1L; // timer_func으로 전달하는 인자값
        timer.expires = jiffies + (1*HZ);
        add_timer(&timer);
    }
*/
    if(!strcmp(cmd,"end"))
    {
        pid_valid=0;
    }

    //gpio_set_value(GPIO_LED, (!strcmp(msg,"0"))?0:1);
                

    printk(KERN_INFO "GPIO Device write : %s\n", msg);

    //시그널 발생시 보낼 PID값을 등록
    pid = simple_strtol(pidstr, &endptr, 10);
    printk("pid=%d\n",pid);
    if(endptr != NULL)
    {
        task = pid_task(find_vpid(pid),PIDTYPE_PID);
        if(task == NULL)
        {
            printk("Error: I don't know user pid\n");
        }
    }

    return count;
}

static ssize_t gpio_read(struct file *inode, char *buff, size_t len, loff_t *off)
{
    short count;

    sprintf(msg,"%d",key_value);
    count = copy_to_user(buff, msg, strlen(msg)+1);


    /*
    if(gpio_get_value(GPIO_LED))
        msg[0]='1';
    else
        msg[0]='0';

    strcat(msg," from kernel");
    count = copy_to_user(buff, msg, strlen(msg)+1);
    printk(KERN_INFO "GPIO Device read : %s\n", msg);
    */


/*
    if(buff[0] == '1')
    {
        GPIO_SET(GPIO_LED1);
        GPIO_SET(GPIO_LED2);
    }
    else
    {
        GPIO_CLR(GPIO_LED1);
        GPIO_CLR(GPIO_LED2);
    }
*/
    return count;
}

static int initModule(void)
{
    dev_t devno;
    unsigned int count;
    int re;

    int err;
    //함수 호출 유무를 확인하기 위해
	printk(KERN_INFO "Init gpio_module\n");

    //1. 문자 디바이스를 등록
    devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    printk(KERN_INFO "devno=0x%x\n", devno);
    register_chrdev_region(devno,1, GPIO_DEVICE);

    //2. 문자 디바이스의 번호와 이름을 등록
    cdev_init(&gpio_cdev, &gpio_fops);
    count = 1;

    //3. 문자 디바이스 추가
    err = cdev_add(&gpio_cdev, devno, count);
    if(err < 0)
    {
        printk(KERN_INFO "Error : cdev_add()\n");
        return -1;
    }

    printk(KERN_INFO "'mknod /dev/%s c %d 0'\n", GPIO_DEVICE, GPIO_MAJOR);
    printk(KERN_INFO "'chmod 666 /dev/%s'\n", GPIO_DEVICE);


    
    // gpio.h에 정의된 gpio_request함수의 사용
    err = gpio_request(GPIO_LED,"LED");
    
    // GPIO_SW를 IRQ로 설정하기
    err = gpio_request(GPIO_SW, "SW");
    switch_irq = gpio_to_irq(GPIO_SW);
    re=request_irq(switch_irq, isr_func, IRQF_TRIGGER_RISING, "switch", NULL);
    
    
    err = gpio_request(GPIO_SW2, "SW2");
    switch_irq2 = gpio_to_irq(GPIO_SW2);
    re=request_irq(switch_irq2, isr_func, IRQF_TRIGGER_RISING, "switch2", NULL);
    

    /*
    if(err==-EBUSY)
    {
        printk(KERN_INFO " Error gpio_request\n");
        return -1;
    }
*/
    //4. 물리메모리 번지를 인자로 전달하면 가상메모리 번지를 리턴한다.


    gpio_direction_output(GPIO_LED, 0);
    gpio_direction_output(GPIO_LED2, 0);
    //gpio_direction_input(GPIO_SW);
    //gpio_direction_input(GPIO_SW2);

    //GPIO_OUT(GPIO_LED1);
    //GPIO_OUT(GPIO_LED2);
    //GPIO_IN(GPIO_SW);


	return 0;
}

static void __exit cleanupModule(void)
{
    dev_t devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    del_timer_sync(&timer);// 타이머 삭제
    
    //1. 문자 디바이스의 등록(장치번호, 장치명)을 해제한다.
    unregister_chrdev_region(devno, 1);

    //2. 문자 디바이스의 구조체를 제거한다.
        gpio_set_value(GPIO_LED,1);
        gpio_set_value(GPIO_LED2,0);
    cdev_del(&gpio_cdev);

    free_irq(switch_irq,NULL);
    //gpio_direction_output(GPIO_LED,0);
    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);
    gpio_free(GPIO_LED2);
    gpio_free(GPIO_SW);
    gpio_free(GPIO_SW2);
    

    //3. 문자 디바이스의 가상번지를 삭제한다.
    if(gpio)
        iounmap(gpio);

	printk(KERN_INFO "Exit gpio_module : Good Bye\n");
}

//my initialize function
module_init(initModule);
//my exit function
module_exit(cleanupModule);

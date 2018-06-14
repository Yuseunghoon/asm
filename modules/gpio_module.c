#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>

#define GPIO_MAJOR 200
#define GPIO_MINOR 0
#define GPIO_DEVICE "gpioled"

// Raspi 0,1 PHYSICAL I/O PERI BASE ADDR
//#define BCM_IO_BASE 0x20000000

// Raspi 3 PHYSICAL I/O PERI BASE ADDR
#define BCM_IO_BASE 0x3F000000
#define GPIO_BASE  (BCM_IO_BASE + 0x200000)
#define GPIO_SIZE 0xB4

#define GPIO_IN(g) (*(gpio+((g)/10)) &= (1<<(((g)%10)*3)))
#define GPIO_OUT(g) (*(gpio+((g)/10)) |= (1<<(((g)%10)*3)))

//#define GPIO_SET(g) (*(gpio+7) = (1<<g))
#define GPIO_CLR(g) (*(gpio+10) = (1<<g))
#define GPIO_GET(g) (*(gpio+13)&(1<<g))
#define GPIO_LED1 17
#define GPIO_LED2 27
#define BUF_SIZE 100
#define GPIO_SW 22

static char msg[BUF_SIZE]={0};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SH YU");
MODULE_DESCRIPTION("Raspberry Pi First Device Driver");


volatile unsigned int *gpio;
struct cdev gpio_cdev;

int GPIO_SET(int g)
{
    if(g>31)
        *(gpio+8)=(1<<(g-32));
    else
        *(gpio+7)=(1<<(g));
    return 0;
}

static int gpio_open(struct inode *inod, struct file *fil);
static int gpio_close(struct inode *inod, struct file *fil);

static ssize_t gpio_write(struct file *inode, const char *buff, size_t len, loff_t *off);
static ssize_t gpio_read(struct file *inode, char *buff, size_t len, loff_t *off);

static struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .read = gpio_read,
    .write = gpio_write,
    .open = gpio_open,
    .release = gpio_close,
};


static int gpio_open(struct inode *inod, struct file *fil)
{
    try_module_get(THIS_MODULE);
    printk(KERN_INFO "GPIO DEVICE opened()\n");
    return 0;
}

static int gpio_close(struct inode *inod, struct file *fil)
{
    module_put(THIS_MODULE);
    printk(KERN_INFO "GPIO DEVICE closed()\n");
    return 0;
}

static ssize_t gpio_write(struct file *inode, const char *buff, size_t len, loff_t *off)
{
    short count;
    memset(msg, 0, BUF_SIZE);
    count = copy_from_user(msg, buff, len);
    printk(KERN_INFO "GPIO DEVICE Write : %s\n", msg);
    return count;
}

static ssize_t gpio_read(struct file *inode, char *buff, size_t len, loff_t *off)
{
    int count;
    strcat(msg, "from kernel");

    count = copy_to_user(buff,msg,strlen(msg)+1);
    printk(KERN_INFO "GPIO DEVICE read : %s\n", msg);

    GPIO_SET(17);
    GPIO_SET(27);

    return count;
}

static int __inint_initModule(void)
{
    dev_t devno;
    unsigned int count;
    static void *map;

    int err;
    //함수 호출 유무를 확인하기
    printk(KERN_INFO "Init gpio_module\n");

    //1. 문자 디바이스를 이름 등록
    devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    printk(KERN_INFO "devno=0x%x\n",devno);
    register_chrdev_region(devno,1,GPIO_DEVICE);

    //2. 문자 디바이스를 위한 구조체를 초기화 한다.
    cdev_init(&gpio_cdev, &gpio_fops);
    count = 1;
    
    //3. 문자 디바이스 추
    err = cdev_add(&gpio_cdev, devno, count);
    if(err<0)
    {
        printk(KERN_INFO "Error : cdev_add()\n");
        return -1;
    }

    printk(KERN_INFO "'mknod /dev/%s c %d 0'\n",GPIO_DEVICE, GPIO_MAJOR);
    printk(KERN_INFO "'chmod 666 /dev/%s '\n",GPIO_DEVICE);
    
    //4. 물리메모리 번지를 인자로 전달하면 가상메모리 번지를 리턴한다
    map = ioremap(GPIO_BASE, GPIO_SIZE);
    if(!map)
    {
        printk(KERN_INFO "Error : mapping GPIO memory\n");
        iounmap(map);
        return -EBUSY;
    }

    gpio = (volatile unsigned int *)map;
    
    GPIO_OUT(17);
    GPIO_OUT(27);
    GPIO_IN(GPIO_SW);


    return 0;
}

static void __exit_cleanipModule(void)
{
    dev_t devno = MKDEV(GPIO_MAJOR, GPIO_MINOR);
    // 1. 문자 디바이스의 등록(장치번호, 정치명)을 해제한다.
    unregister_chrdev_region(devno,1);

    // 2. 문자 디바이스의 구조체를 제거한다.
    cdev_del(&gpio_cdev);

    // 3. 문 자디바이스의 가상번지를 삭제한다.
    if(gpio)
        iounmap(gpio);
    printk(KERN_INFO "Exit gpio_module : Goodbye!\n");
}

//내가 생성하고자 하는 초기화함수 이름을 적어준다
module_init(__inint_initModule);
//내가 생성하고자 하는 종료함수 이름을 적어준다
module_exit(__exit_cleanipModule);



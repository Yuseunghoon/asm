#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/interrupt.h>

static int __init myinit(void)
{
    printk("my_init\n");
    return 0;
}

void my_exit(void)
{
    printk("my_exit\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Seunghoon Yu <kof22@naver.com>");
MODULE_LICENSE("GPL");

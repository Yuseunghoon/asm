#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/io.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SH YU");
MODULE_DESCRIPTION("Raspberry Pi First Device Driver");


int initModule(void)
{
	printk(KERN_INFO "Init gpio_module\n");
	return 0;
}

void cleanupModule(void)
{
	printk(KERN_INFO "Exit gpio_module\n");
}







//내가 생성하고 하는 초기화함수 이름을 적어준다.
module_init(initModule);

//내가 생성하고 하는 종료함수 이름을 적어준다.
module_exit(cleanupModule);

#include <linux/module.h>
#include <linux/fs.h> 
#include <linux/errno.h> 
#include <linux/miscdevice.h>
#include <linux/kernel.h> 
#include <linux/major.h> 
#include <linux/mutex.h> 
#include <linux/proc_fs.h> 
#include <linux/seq_file.h> 
#include <linux/stat.h> 
#include <linux/init.h> 
#include <linux/device.h> 
#include <linux/tty.h> 
#include <linux/kmod.h> 
#include <linux/gfp.h> 
#include <asm/io.h>
#include "hello_gpio.h"

//对外暴露的全局变量，需要硬件层调用get_board_gpio_op函数，对其初始化注册。
static struct hello_gpio_op *p_gpio_obj;

static int major = 0;
static struct class *class;

static int hello_gpio_open(struct inode *node, struct file *file)
{
	HELLO_DEBUG;
	size_t minor =iminor(node);
	printk("minor:%d\n", minor);
	p_gpio_obj->init(minor);

#if 0
	volatile size_t *CRU_CLKGATE_CON31;
	volatile size_t *GRF_GPIO4C_IOMUX;
	volatile size_t *GPIO_SWPORTA_DDR;
	volatile size_t *GPIO_SWPORTA_DR;

	CRU_CLKGATE_CON31 = ioremap(0xff760000+0x037c, 4);
	GRF_GPIO4C_IOMUX = ioremap(0xff770000+0xe028, 4);
	GPIO_SWPORTA_DDR = ioremap(0xff790000+0x0004, 4);
	GPIO_SWPORTA_DR = ioremap(0xff790000+0x0000, 4);

	printk("addr: %x %x %x %x\n",CRU_CLKGATE_CON31, GRF_GPIO4C_IOMUX, GPIO_SWPORTA_DDR, GPIO_SWPORTA_DR);
	iowrite32((1<<(5+16)) | (0<<5), CRU_CLKGATE_CON31);
	iowrite32((0xF<<16+0)|(0<<0), GRF_GPIO4C_IOMUX);
	iowrite32((3<<16), GPIO_SWPORTA_DDR);
	iowrite32((0<<16), GPIO_SWPORTA_DR);
	//*CRU_CLKGATE_CON31 = (1<<(5+16)) | (0<<5); //段错误！
	//*GRF_GPIO4C_IOMUX = (0xF<<16+0)|(0<<0);
	//*GPIO_SWPORTA_DDR |= (3<<16);
	//*GPIO_SWPORTA_DR |= (3<<16);
#endif
	
	return  0;
}

static int hello_gpio_read(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	HELLO_DEBUG;
	return  0;
}

static int hello_gpio_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	HELLO_DEBUG;
	size_t err;
	char status;
	struct inode* inode = file_inode(file);
	size_t minor = iminor(inode);
	err = copy_from_user(&status, buf, 1);
	p_gpio_obj->ctl(minor, status);
	return  0;	
}

static int hello_gpio_close(struct inode *node, struct file *file)
{
	HELLO_DEBUG;
	return  0;
}

static struct file_operations gpio_drv={
	.owner = THIS_MODULE,
	.open = hello_gpio_open,
	.read = hello_gpio_read,
	.write = hello_gpio_write,
	.release = hello_gpio_close,
};


static size_t __init hello_gpio_init(void)
{
	HELLO_DEBUG;
	ssize_t err;
	size_t index;
	p_gpio_obj = get_board_gpio_op(); //注入
	major = register_chrdev(0, "hello_gpio", &gpio_drv);
	class = class_create(THIS_MODULE, "hello_class");
	err = PTR_ERR(class);
	if(IS_ERR(err))
	{
		HELLO_DEBUG;
		unregister_chrdev(major, "hello_gpio");
		return -1;
	}
	device_create(class, NULL, MKDEV(major, 0), NULL, "hello_gpio");
	return  0;
}

static void __exit hello_gpio_exit(void)
{
	HELLO_DEBUG;
	p_gpio_obj = NULL;
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, "hello_gpio");
}

module_init(hello_gpio_init);
module_exit(hello_gpio_exit);

MODULE_LICENSE("GPL");

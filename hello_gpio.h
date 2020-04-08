#ifndef _HELLO_GPIO_H_
#define _HELLO_GPIO_H_

#define HELLO_DEBUG printk("%s %s line%d\n", __FILE__, __FUNCTION__,__LINE__)
//注册解耦合
struct hello_gpio_op{
	size_t (*init)(size_t which);//初始化哪个设备
	size_t (*ctl)(size_t which, ssize_t status);//控制设备状态
};

extern struct hello_gpio_op *get_board_gpio_op(void);


#endif


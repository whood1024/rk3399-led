#include <asm/io.h>
#include "board_led.h"
/*
	neo-pi-rockchip
	GPIO4 C0 C1 
*/
static volatile size_t *CRU_CLKGATE_CON31;
static volatile size_t *GRF_GPIO4C_IOMUX;
static volatile size_t *GPIO_SWPORTA_DDR;
static volatile size_t *GPIO_SWPORTA_DR;

static ssize_t rk3399_get_addr(void)
{
	 HELLO_DEBUG;
	 CRU_CLKGATE_CON31 = ioremap(0xFF760000+0x037c, 4);
	 if(CRU_CLKGATE_CON31==NULL)
	 {
	 		printk("get virtual address failed\n");
			return -1;
	 }
	 GRF_GPIO4C_IOMUX = ioremap(0xff770000+0xe028, 4);
	 GPIO_SWPORTA_DDR = ioremap(0xff790000+0x0004, 4);
	 GPIO_SWPORTA_DR = ioremap(0xff790000+0x0000, 4);
	 return 0;
}
 
//板端的GPIO初始化
static void rk3399_led_init(size_t which)
{
	HELLO_DEBUG;
	rk3399_get_addr();
	if(which == 0)
	{
	/* a. 使能 GPIO4 
	  * set CRU to enable GPIO4 
	  * CRU_CLKGATE_CON31 0xFF760000 + 0x037c 
	  * (1<<(5+16)) | (0<<5) 
	  */ 
	  //*CRU_CLKGATE_CON31 = (1<<(5+16)) | (0<<5);
	  iowrite32((1<<(5+16)) | (0<<5), CRU_CLKGATE_CON31);

	/* 配置复用端口为GPIO4C1,2输出模式
		GRF_GPIO4C_IOMUX：0xff770000 + 0x0e028
		(0xF<<16+0)|(0<<0)
	*/
	  //*GRF_GPIO4C_IOMUX = (0xF<<16+0)|(0<<0);
	  iowrite32((0xF<<16+0)|(0<<0), GRF_GPIO4C_IOMUX);
	

	/* 配置GPIO4C0,1为输出
	   bit16=1 bit17=1
	   GPIO_SWPORTA_DDR: 0xff790000 + 0x0004
	   (3<<16)
	*/
	  //*GPIO_SWPORTA_DDR |= (3<<16);
	  iowrite32((3<<16)|(ioread32(GPIO_SWPORTA_DDR)), GPIO_SWPORTA_DDR);
	  HELLO_DEBUG;
	}
}

static void rk3399_led_ctl(size_t which, ssize_t status)
{
	HELLO_DEBUG;

	if(which ==0)
	{
		/* 配置GPIO4C0,1 高低电平
		   GPIO_SWPORTA_DR: 0xff790000 + 0x0000
		   bit16=1,bit17=1 高电平 LED灭
		   bit16=0,bit17=0 低电平 LED亮
		*/
		if(status==1)
		{
			iowrite32((~(3<<16))&(ioread32(GPIO_SWPORTA_DR)), GPIO_SWPORTA_DR);
			//*GPIO_SWPORTA_DR |= (3<<16);
		}
		else
		{
			iowrite32((3<<16)|(ioread32(GPIO_SWPORTA_DR)), GPIO_SWPORTA_DR);
			//*GPIO_SWPORTA_DR |= (0<<16);
		}
		
	}
}

//接口
static struct hello_gpio_op board_led_obj={
	.init = rk3399_led_init,
	.ctl = rk3399_led_ctl,
};


//注入
struct hello_gpio_op *get_board_gpio_op(void){
	return &board_led_obj;
}


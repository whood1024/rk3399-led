.PHONY: all clean

KERNEL_DIR=/home/sources/_03kernel-rockchip
CROSS_COMPILE =aarch64-linux-gnu-

all:
	make -C $(KERNEL_DIR) M=`pwd` modules
	$(CROSS_COMPILE)gcc -o App App.c
	
clean:
	make -C $(KERNEL_DIR) M=`pwd` modules clean
	rm -rf modules.order
	rm -f App	

hello_led-y := hello_gpio.o board_led.o
obj-m += hello_led.o


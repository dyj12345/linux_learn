obj-m = module_led.o

KDIR = ~/kernel_src

all:
	cp ~/share/*.c .
	make -C $(KDIR) M=`pwd` modules
	arm-linux-gcc app.c -o app
	cp *.ko ~/share
clean:
	make -C $(KDIR) M=`pwd` modules clean

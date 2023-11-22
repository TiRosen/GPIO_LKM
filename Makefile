
CC := /usr/bin/gcc-12

obj-m += gpio_driver.o 

 


 

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules 

clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

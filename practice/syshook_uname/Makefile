obj-m += syshook.o

all:	
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

deviceclean:
	rmmod syshook.ko

device: 
	insmod syshook.ko

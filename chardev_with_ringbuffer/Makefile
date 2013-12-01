obj-m += chardev.o

all:	
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	make device
	gcc -o run user_code.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	make deviceclean

deviceclean:
	rmmod chardev.ko
	rm /dev/osqueue

device: 
	insmod chardev.ko
	mknod /dev/osqueue c 253 0

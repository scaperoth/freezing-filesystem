obj-m += chardev.o

all:	
	make obj
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	make device

obj:
	gcc -o run user_code.c
	gcc -o callee callee.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	make deviceclean

deviceclean:
	rmmod chardev.ko
	rm /dev/osqueue

device: 
	insmod chardev.ko
	mknod /dev/osqueue c 253 0

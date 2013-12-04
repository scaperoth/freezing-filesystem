obj-m += chardev.o
GCC = gcc
GCFLAGS = -o
OBJ = callee run usrhelp 

all:	
	make obj
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	make device

obj:
	$(GCC) $(GCFLAGS) run user_code.c
	$(GCC) $(GCFLAGS) callee callee.c
	$(GCC) $(GCFLAGS) usrhelp user_helper.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	make deviceclean

deviceclean:
	rmmod chardev.ko
	rm /dev/osqueue

device: 
	insmod chardev.ko
	mknod /dev/osqueue c 253 0

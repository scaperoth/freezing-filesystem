obj-m += chardev.o
GCC = gcc
GCFLAGS = -o
OBJ = callee run usrhelp 

all:	
	make obj
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	make device

obj:
	$(GCC) $(GCFLAGS) callee callee.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	make deviceclean
	rm -f .symvers callee run usrhelp *#* *~ 

deviceclean:
	rmmod chardev.ko
	rm -f /dev/osqueue

device: 
	insmod chardev.ko
	mknod /dev/osqueue c 253 0

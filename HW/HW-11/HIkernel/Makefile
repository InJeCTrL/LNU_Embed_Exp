ifneq ($(KERNELRELEASE),)

	obj-m := HIkernel.o

else

	KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.o *.mod.c *.mod.o *.order *.symvers
endif
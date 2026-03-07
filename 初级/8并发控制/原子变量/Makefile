ifeq ($(KERNELRELEASE),)

ifeq ($(ARCH),arm)
KERNELDIR ?= /home/ghc/Linux4412/kernel/linux-3.14
ROOTFS ?= /opt/4412/rootfs
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
endif

PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules INSTALL_MOD_PATH=$(ROOTFS) modules_install

clean:
	rm -rf *.o *.ko .*.cmd *.mod.* *.mod modules.order Module.symvers .tmpversion

else

obj-m += openonce_atomic.o
endif
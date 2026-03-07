ifeq ($(KERNELRELEASE),)

ifeq ($(ARCH),arm)
KERNELDIR ?= /home/ghc/Linux4412/kernel/linux-3.14
ROOTFS ?= /opt/4412/rootfs
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
endif

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules INSTALL_MOD_PATH=$(ROOTFS) modules_install
clean:
	rm -rf  *.o  *.ko  .*.cmd  *.mod.*  modules.order  Module.symvers   .tmp_versions
else
CONFIG_MODULE_SIG=n
obj-m += myled.o

endif
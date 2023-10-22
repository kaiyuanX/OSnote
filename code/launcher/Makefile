obj-m := launcher.o

# 内核地址
KDIR := $(realpath ../../linux-source-5.4.0)

default:
	echo $(KDIR)
	$(MAKE) -C $(KDIR) M=$(PWD)
	cp launcher.ko ../initramfs/nuke/

clean:
	$(MAKE)-C $(KDIR) M=$(PWD) clean

.PHONY: module clean

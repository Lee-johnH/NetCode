ifneq ($(KERNELRELESE),)
	obj-m := mymap_driver.o 	#编译链接后将生成mymap_driver.o模块
else
	KDIR := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules		
	@#此处将再次调用make
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean 	
	@#删除生成的所有文件
endif

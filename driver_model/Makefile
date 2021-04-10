obj-m += dummy_serial_dev.o dummy_serial_driver.o
KSRC=/home/user/workspace/kernellinux/linux-4.14.202
#replace KSRC if building for other kernel version
all:
	make -C ${KSRC} M=${PWD} modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
clean:
	make -C ${KSRC} M=${PWD} modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- 

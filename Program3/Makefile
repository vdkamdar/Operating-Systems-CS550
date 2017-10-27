obj-m = linepipe.o
KBUILD_CFLAGS += -w
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules $(KBUILD_CFLAGS)
	gcc producer.c -o producer.o
	gcc consumer.c -o consumer.o
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

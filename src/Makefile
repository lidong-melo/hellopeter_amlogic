#
## sample Makefile for hellopeter
#
#
OBJ = hellopeter.o alsalib_test.o serial_test.o alsa_record.o alsa_play.o multi_thread.o serial_test.o gpio.o fifo.o
CFLAGS = -Wall -Wextra
#LDFLAGS = -lasound -L~/buildroot_openlinux_kernel_4.9_20180810/buildroot/package/hellopeter/src/lib
LDFLAGS = -lasound -L/home/puzzlemaster/buildroot_openlinux_kernel_4.9_20180810/buildroot/package/hellopeter/src/lib -lpthread
TARGET=hellopeter

$(info "i am here\n")
# rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -f $(OBJ)

install:
	cp hellopeter $(DESTDIR)/bin/hellopeter

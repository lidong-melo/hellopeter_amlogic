#
## sample Makefile for hellopeter
#
#
OBJ = hellopeter.o alsa_record.o alsa_play.o serial_port.o gpio.o fifo.o log.o
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

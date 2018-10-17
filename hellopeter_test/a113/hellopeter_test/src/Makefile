#
## sample Makefile for hellopeter_test for a113
#  tx2 test code not here, but on tx2
#
#
OBJ = hellopeter_test.o alsa_record.o alsa_play.o serial_port.o gpio.o fifo.o log.o
CFLAGS = -Wall -Wextra
#LDFLAGS = -lasound -L~/buildroot_openlinux_kernel_4.9_20180810/buildroot/package/hellopeter/src/lib
LDFLAGS = -lasound -L/home/puzzlemaster/buildroot_openlinux_kernel_4.9_20180810/buildroot/package/hellopeter_test/src/lib -lpthread
TARGET=hellopeter_test

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
	cp hellopeter_test $(DESTDIR)/bin/hellopeter_test

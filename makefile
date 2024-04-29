TARGET = AlarmClock
ALT_DEVICE_FAMILY ?= soc_cv_av
SOCEDS_ROOT ?= $(SOCEDS_DEST_ROOT)
HWLIBS_ROOT = $(SOCEDS_ROOT)/ip/altera/hps/altera_hps/hwlib
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -g -Wall -std=gnu99 -D$(ALT_DEVICE_FAMILY) -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY) -I$(HWLIBS_ROOT)/include/ -I./alsa/include -w
LDFLAGS = -g -Wall
CC = $(CROSS_COMPILE)gcc
#CC = $(CROSS_COMPILE)g++
ARCH= arm

build: $(TARGET)
all: $(TARGET)

$(TARGET): main.o terasic_lib.o Hardware.o ClockFace.o ClockApp.o Time.o Alarm.o\
LCD_Lib.o LCD_Driver.o LCD_Hw.o lcd_graphic.o font.o
	$(CC) $(LDFLAGS) $^ -o $@ -lrt -lm
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) *.a *.o *~ *.bmp

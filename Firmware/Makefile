CC=avr-gcc
CXX=avr-g++
CFLAGS=-std=c99
CXXFLAGS=-std=c++11
CPPFLAGS=-g -Os -Wall -mcall-prologues -mmcu=$(MMCU) -DF_CPU=$(F_CPU) -I.
MMCU=atmega328p
BOARD=arduino
ifeq ($(BOARD),arduino)
F_CPU=16000000UL
CPPFLAGS+=-DBOARD_DEFINITION=\"src/board_definitions/arduino.h\"
PROGRAMMER_PORT=/dev/tty.usbmodem*
PROGRAMMERFLAGS=-c arduino -P /dev/tty.usbmodem* -p $(MMCU)
else ifeq ($(BOARD),real)
F_CPU=1000000UL
CPPFLAGS+=-DBOARD_DEFINITION=\"src/board_definitions/real.h\"
PROGRAMMERFLAGS=-c avrispv2 -P usb -p $(MMCU)
PROGRAMMER_PORT=usb
endif

TARGET=firmware
ENTRYPOINT=main.cpp
EEPROM_IMAGE=.eeprom_data
CPP_SOURCES=$(ENTRYPOINT)
CPP_SOURCES+=$(wildcard src/*.cpp)
CPP_SOURCES+=$(wildcard lib/*.cpp)
O_FILES+=$(patsubst %.cpp,%.o,$(CPP_SOURCES))
.DEFAULT_GOAL := flash

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.elf: $(O_FILES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

%.hex: %.elf
	avr-objcopy -O ihex $< $@

flash: size program

program: $(TARGET).hex ${EEPROM_IMAGE}
	avrdude $(PROGRAMMERFLAGS) -q -U eeprom:w:$(EEPROM_IMAGE):h -U flash:w:$<

clean:
	rm -f $(TARGET).hex $(TARGET).elf *.o

.depend: $(ENTRYPOINT)
	rm -f "$@"
	$(CPP) $(CPPFLAGS) -MM $^ -MF "$@"

size: $(patsubst %.cpp,%.elf,$(ENTRYPOINT))
	avr-size --mcu=atmega328p --format avr $<

monitor:
	screen $(PROGRAMMER_PORT) 9600

include .depend

.PHONY: program clean

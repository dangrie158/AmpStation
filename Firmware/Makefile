CC=avr-gcc
CXX=avr-g++
CFLAGS=-std=c99
CXXFLAGS=-std=c++11
MMCU=atmega328p
CPPFLAGS=-g -Os -Wall -mcall-prologues -mmcu=$(MMCU) -DF_CPU=16000000UL -I.
PROGRAMMER_PORT=/dev/tty.usbmodem*

TARGET=firmware
ENTRYPOINT=main.cpp
EEPROM_IMAGE=.eeprom_data
CPP_SOURCES=$(ENTRYPOINT)
CPP_SOURCES+=$(wildcard src/*.cpp)
CPP_SOURCES+=$(wildcard lib/*.cpp)
O_FILES+=$(patsubst %.cpp,%.o,$(CPP_SOURCES))

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.elf: $(O_FILES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

%.hex: %.elf
	avr-objcopy -O ihex $< $@

program: $(TARGET).hex ${EEPROM_IMAGE}
	avrdude -p $(MMCU) -c arduino -P $(PROGRAMMER_PORT) -U eeprom:w:${EEPROM_IMAGE}:h -U flash:w:$<

clean:
	rm -f $(TARGET).hex $(TARGET).elf *.o

.depend: $(ENTRYPOINT)
	rm -f "$@"
	$(CPP) $(CPPFLAGS) -MM $^ -MF "$@"

include .depend

.PHONY: program clean

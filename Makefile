ARCH := $(shell uname)
USER := $(shell whoami)

PATH_TOOLCHAIN = /Users/frank/Projects/atmega328pb/avr8-gnu-toolchain-darwin_x86_64/bin
PATH_PACK      = /Users/frank/Projects/atmega328pb/Atmel.ATmega_DFP.1.2.209
AVRDUDE_EXEC   = /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude

# adafruit programmer, sparkfun
# PROGRAMMER = usbtiny

# open evse programmer, baite
PROGRAMMER = USBasp

# STK500 programmer needs port specified, not sure how that works
# pololu
# PROGRAMMER  = STK500

#
# attiny45 @ 8 mhz internal rc
#

# avrdude: safemode: Fuses OK (E:FF, H:DF, L:62)

# http://www.engbedded.com/fusecalc/

# efuse: 11111111 0xff (default)
# hfuse: 11011111 0xdf (default)
# lfuse: 01100010 0x62 = 8 mhz/8 (default)
# lfuse: 11100010 0xe2 = 8 mhz/1


DEVICE    = attiny45
OBJECTS   = main.o board.o uart.o
FUSES     = -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
CLOCK     = 1000000
READFUSES = -U efuse:r:efuse.hex:h -U lfuse:r:lfuse.hex:h -U hfuse:r:hfuse.hex:h

# for extra verbose: -v -v -v
AVRDUDE   = $(AVRDUDE_EXEC) -c $(PROGRAMMER) -C avrdude.conf -p $(DEVICE)

# compiler options:
# -Wall   warnings, all
# -Os     optimization for size
COMPILE = $(PATH_TOOLCHAIN)/avr-gcc -Wall -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -B $(PATH_PACK)/gcc/dev/attiny84/ -I $(PATH_PACK)/include/
OBJDUMP = $(PATH_TOOLCHAIN)/avr-objdump
OBJCOPY = $(PATH_TOOLCHAIN)/avr-objcopy


all:	main.hex main.lss                                        

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

program:	
	$(AVRDUDE) -U flash:w:main.hex:i

readfuse:
	$(AVRDUDE) $(READFUSES)
	cat efuse.hex hfuse.hex lfuse.hex

clean:
	rm -f main.hex main.lss main.elf $(OBJECTS)
	
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	$(OBJCOPY) -j .text -j .data -O ihex main.elf main.hex

%.lss: %.elf
	@echo
	$(OBJDUMP) -h -S $< > $@

cpp:
	$(COMPILE) -E main.c

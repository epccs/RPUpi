# Remote for ATMega328pb bus manager on RPUpi board
#
TARGET = Remote
LIBDIR = ../lib
OBJECTS = main.o \
	rpubus_manager_state.o \
	i2c_cmds.o \
	smbus_cmds.o \
	$(LIBDIR)/uart.o \
	$(LIBDIR)/timers.o \
	$(LIBDIR)/twi0.o \
	$(LIBDIR)/twi1.o

## Chip and project-specific global definitions
MCU   =  atmega328pb
F_CPU = 12000000UL  
BAUD  =  250000UL
I2C0_ADDRESS = 0x29
I2C1_ADDRESS = 0x2A
RPU_ADDRESS = 0x31
# '0' or ascii 0x30 is default address sent on DTR pair when host connects,  e.g. pulls nDTR or nRTS active
RPU_HOST_CONNECT = 0x30
CPPFLAGS = -DRPU_HOST_CONNECT=$(RPU_HOST_CONNECT) -DI2C0_ADDRESS=$(I2C0_ADDRESS) -DI2C1_ADDRESS=$(I2C1_ADDRESS) -DRPU_ADDRESS=$(RPU_ADDRESS) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. 

# at power up send a byte on the DTR pair to unlock the bus
# CPPFLAGS += -DDISCONNECT_AT_PWRUP=1

# at power up set HOST_LOCKOUT_STATUS bit
CPPFLAGS += -DHOST_LOCKOUT=1

## Cross-compilation
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

## Uno with Arduino as ISP sketch shows as ttyACMx
ISP_PORT = /dev/ttyACM0

## Compiler/linker options
CFLAGS = -Os -g -std=gnu99 -Wall
# CFLAGS += -funsigned-char -funsigned-bitfields 
# CFLAGS += -fpack-struct -fshort-enums 
CFLAGS += -ffunction-sections -fdata-sections 

TARGET_ARCH = -mmcu=$(MCU) -B $(LIBDIR)/Atmel.ATmega_DFP.1.3.300.atpack/gcc/dev/atmega328pb/ -I $(LIBDIR)/Atmel.ATmega_DFP.1.3.300.atpack/include/
## if atmega328pb is in avr-gcc mainline use
##TARGET_ARCH = -mmcu=$(MCU)

LDFLAGS = -Wl,-Map,$(TARGET).map 
LDFLAGS += -Wl,--gc-sections 

all: $(TARGET).hex $(TARGET).lst

$(TARGET): $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

## to reduce dupliction I only show the fuse setting in ../Bootloader
## 328p can lock the bootloader section, which is a good idea even if it is not used
isp:
	avrdude -v -p $(MCU) -C +$(LIBDIR)/avrdude/328pb.conf -c stk500v1 -P $(ISP_PORT) -b 19200 -e -U flash:w:$(TARGET).hex -U lock:w:0xef:m

# I am not sure where the avrdude in raspian is from but it may be this one
# https://github.com/facchinm/avrdude
# Raspian version is missing this change sudo for now 
# https://github.com/facchinm/avrdude/commit/aa0d28f774147fb03f8b18cd065586c7a93a2b0b
linuxspi:
	sudo avrdude -v -p $(MCU) -C +$(LIBDIR)/avrdude/328pb.conf -c linuxspi -P /dev/spidev0.0 -e -U flash:w:$(TARGET).hex -U lock:w:0xef:m

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ -o $@
	$(SIZE) -C --mcu=$(MCU) $@
	rm -f $(TARGET).o $(OBJECTS)

clean: 
	rm -f $(TARGET).hex $(TARGET).map $(TARGET).elf $(TARGET).lst
 
%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

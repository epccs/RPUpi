# Description

This shows the setup and methods used for evaluation of RPUpi.

# Table of References


# Table Of Contents:

6. ^3 Power Data
5. ^3 Bootload Remote AVR
4. ^2 Clearance IDE Connector For ICSP Tool
3. ^2 Clearance Between Pi and Plugable
2. ^1 Vertical Mounting
1. ^0 Mounting


## ^3 Power Data

PV input to RPUno is at 200mA in Constant Current mode

```
/1/analog? 7,6,3,2
{"PWR_V":"14.15","PV_V":"16.88","DISCHRG_A":"0.003","CHRG_A":"0.024"}
{"PWR_V":"14.15","PV_V":"16.88","DISCHRG_A":"0.000","CHRG_A":"0.052"}
{"PWR_V":"14.15","PV_V":"16.85","DISCHRG_A":"0.000","CHRG_A":"0.036"}
{"PWR_V":"14.15","PV_V":"16.83","DISCHRG_A":"0.000","CHRG_A":"0.030"}
{"PWR_V":"14.15","PV_V":"16.93","DISCHRG_A":"0.000","CHRG_A":"0.030"}
{"PWR_V":"14.15","PV_V":"16.85","DISCHRG_A":"0.000","CHRG_A":"0.039"}
{"PWR_V":"14.10","PV_V":"16.93","DISCHRG_A":"0.000","CHRG_A":"0.062"}
{"PWR_V":"14.16","PV_V":"16.83","DISCHRG_A":"0.000","CHRG_A":"0.080"}
{"PWR_V":"14.15","PV_V":"16.88","DISCHRG_A":"0.000","CHRG_A":"0.066"}
{"PWR_V":"14.15","PV_V":"16.88","DISCHRG_A":"0.000","CHRG_A":"0.014"}
```

Readings were taken every 30 seconds, this should give an idea how the Pi Zero power usage fluctuates.

PV to RPUno is now turned off so all power is from the 12 SLA Battery (18Ahr AGM). Note 20mA is used on some status LED's controlled by DIO 4 and DIO 13. 

```
/1/analog? 7,6,3,2
{"PWR_V":"13.91","PV_V":"0.16","DISCHRG_A":"0.142","CHRG_A":"0.000"}
{"PWR_V":"13.78","PV_V":"0.36","DISCHRG_A":"0.123","CHRG_A":"0.000"}
{"PWR_V":"13.69","PV_V":"0.34","DISCHRG_A":"0.122","CHRG_A":"0.000"}
{"PWR_V":"13.68","PV_V":"0.29","DISCHRG_A":"0.154","CHRG_A":"0.000"}
{"PWR_V":"13.62","PV_V":"0.31","DISCHRG_A":"0.162","CHRG_A":"0.000"}
{"PWR_V":"13.58","PV_V":"0.31","DISCHRG_A":"0.156","CHRG_A":"0.000"}
{"PWR_V":"13.53","PV_V":"0.31","DISCHRG_A":"0.175","CHRG_A":"0.000"}
{"PWR_V":"13.49","PV_V":"0.31","DISCHRG_A":"0.154","CHRG_A":"0.000"}
{"PWR_V":"13.44","PV_V":"0.34","DISCHRG_A":"0.136","CHRG_A":"0.000"}
```

Readings were taken every 30 seconds.

The [PwrMgt] firmware running on the board under RPUpi can show how many mAHr is used from the battery.

[PwrMgt]: https://github.com/epccs/RPUno/tree/master/PwrMgt

```
/1/charge?
{"CHRG_mAHr":"316.96","DCHRG_mAHr":"19.99","RMNG_mAHr":"0.00","ACCUM_Sec":"14884.78"}
{"CHRG_mAHr":"316.96","DCHRG_mAHr":"21.86","RMNG_mAHr":"0.00","ACCUM_Sec":"14944.78"}
{"CHRG_mAHr":"316.96","DCHRG_mAHr":"23.73","RMNG_mAHr":"0.00","ACCUM_Sec":"15004.78"}
{"CHRG_mAHr":"316.96","DCHRG_mAHr":"25.61","RMNG_mAHr":"0.00","ACCUM_Sec":"15064.78"}
{"CHRG_mAHr":"316.96","DCHRG_mAHr":"27.49","RMNG_mAHr":"0.00","ACCUM_Sec":"15124.78"}
# I ran this befor going out to do some work in the garden, but this is a far as it got.
# Did the AVR mcu reset? 
# The mcu seems to have lost the CHRG mAHr thought the ssh connection never broke
# I bet this has to do with the Night_AttachWork callback that is not implemented 
/1/charge?
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"549.85","RMNG_mAHr":"0.00","ACCUM_Sec":"16483.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"551.86","RMNG_mAHr":"0.00","ACCUM_Sec":"16543.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"553.86","RMNG_mAHr":"0.00","ACCUM_Sec":"16603.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"555.87","RMNG_mAHr":"0.00","ACCUM_Sec":"16663.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"557.88","RMNG_mAHr":"0.00","ACCUM_Sec":"16723.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"559.88","RMNG_mAHr":"0.00","ACCUM_Sec":"16783.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"561.89","RMNG_mAHr":"0.00","ACCUM_Sec":"16843.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"563.89","RMNG_mAHr":"0.00","ACCUM_Sec":"16903.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"565.90","RMNG_mAHr":"0.00","ACCUM_Sec":"16963.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"567.90","RMNG_mAHr":"0.00","ACCUM_Sec":"17023.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"569.91","RMNG_mAHr":"0.00","ACCUM_Sec":"17083.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"571.91","RMNG_mAHr":"0.00","ACCUM_Sec":"17143.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"573.92","RMNG_mAHr":"0.00","ACCUM_Sec":"17203.48"}
{"CHRG_mAHr":"0.00","DCHRG_mAHr":"575.93","RMNG_mAHr":"0.00","ACCUM_Sec":"17263.48"}
/1/analog? 7,6,3,2
{"PWR_V":"12.92","PV_V":"0.31","DISCHRG_A":"0.152","CHRG_A":"0.000"}
```

Charge readings were taken every 60 seconds. The ACCUM Sec is about 4.8 Hr during which it used .6 AHr. That is more power than I would like it to use, but it seems reasonable based on the static current readings. The best charge I've seen so far is about .3AHr per day for a Watt of PV so this is saying I need a minim of 10 Watt of PV to run the Pi, and it needs to always be the best day for charging that I've seen the data for.

These were all done over SSH and WiFi (ohterwise the Pi had no load)

```
sudo iwlist wlan0 scan
TBD
```


## ^3 Bootload Remote AVR

The hostname of the the Pi Zero is pi3, it was the third Pi Zero I have setup, I did not think about how confusing that would be. The default bootload address on the multi-drop RS-422 is '0', and I have not changed it, I keep the [i2c-debug] firmware on an ATmega328p board under an RPUftdi on the bench at that address, so I will just upload that program. 

[i2c-debug]: https://github.com/epccs/RPUno/tree/master/i2c-debug

```
rsutherland@pi3:~/Samba/RPUno/i2c-debug $ uname -a
Linux pi3 4.9.41+ #1023 Tue Aug 8 15:47:12 BST 2017 armv6l GNU/Linux
rsutherland@pi3:~/Samba/RPUno/i2c-debug $ make bootload
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o main.o main.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o i2c-scan.o i2c-scan.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o i2c-cmd.o i2c-cmd.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o ../Uart/id.o ../Uart/id.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o ../lib/timers.o ../lib/timers.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o ../lib/uart.o ../lib/uart.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o ../lib/twi.o ../lib/twi.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o ../lib/rpu_mgr.o ../lib/rpu_mgr.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o ../lib/adc.o ../lib/adc.c
avr-gcc -Os -g -std=gnu99 -Wall -ffunction-sections -fdata-sections  -DF_CPU=16000000UL   -DBAUD=38400UL -I.  -mmcu=atmega328p -c -o ../lib/parse.o ../lib/parse.c
avr-gcc -Wl,-Map,I2c-debug.map  -Wl,--gc-sections  -mmcu=atmega328p main.o i2c-scan.o i2c-cmd.o ../Uart/id.o ../lib/timers.o ../lib/uart.o ../lib/twi.o ../lib/rpu_mgr.o ../lib/adc.o ../lib/parse.o -o I2c-debug.elf
avr-size -C --mcu=atmega328p I2c-debug.elf
AVR Memory Usage
----------------
Device: atmega328p

Program:    8684 bytes (26.5% Full)
(.text + .data + .bootloader)

Data:        375 bytes (18.3% Full)
(.data + .bss + .noinit)


rm -f I2c-debug.o main.o i2c-scan.o i2c-cmd.o ../Uart/id.o ../lib/timers.o ../lib/uart.o ../lib/twi.o ../lib/rpu_mgr.o ../lib/adc.o ../lib/parse.o
avr-objcopy -j .text -j .data -O ihex I2c-debug.elf I2c-debug.hex
rm -f I2c-debug.elf
avrdude -v -p atmega328p -c arduino -P /dev/ttyAMA0 -b 115200 -U flash:w:I2c-debug.hex

avrdude: Version 6.1, compiled on Jul  7 2015 at 10:29:47
         Copyright (c) 2000-2005 Brian Dean, http://www.bdmicro.com/
         Copyright (c) 2007-2014 Joerg Wunsch

         System wide configuration file is "/etc/avrdude.conf"
         User configuration file is "/home/rsutherland/.avrduderc"
         User configuration file does not exist or is not a regular file, skipping

         Using Port                    : /dev/ttyAMA0
         Using Programmer              : arduino
         Overriding Baud Rate          : 115200
         AVR Part                      : ATmega328P
         Chip Erase delay              : 9000 us
         PAGEL                         : PD7
         BS2                           : PC2
         RESET disposition             : dedicated
         RETRY pulse                   : SCK
         serial program mode           : yes
         parallel program mode         : yes
         Timeout                       : 200
         StabDelay                     : 100
         CmdexeDelay                   : 25
         SyncLoops                     : 32
         ByteDelay                     : 0
         PollIndex                     : 3
         PollValue                     : 0x53
         Memory Detail                 :

                                  Block Poll               Page                       Polled
           Memory Type Mode Delay Size  Indx Paged  Size   Size #Pages MinW  MaxW   ReadBack
           ----------- ---- ----- ----- ---- ------ ------ ---- ------ ----- ----- ---------
           eeprom        65    20     4    0 no       1024    4      0  3600  3600 0xff 0xff
           flash         65     6   128    0 yes     32768  128    256  4500  4500 0xff 0xff
           lfuse          0     0     0    0 no          1    0      0  4500  4500 0x00 0x00
           hfuse          0     0     0    0 no          1    0      0  4500  4500 0x00 0x00
           efuse          0     0     0    0 no          1    0      0  4500  4500 0x00 0x00
           lock           0     0     0    0 no          1    0      0  4500  4500 0x00 0x00
           calibration    0     0     0    0 no          1    0      0     0     0 0x00 0x00
           signature      0     0     0    0 no          3    0      0     0     0 0x00 0x00

         Programmer Type : Arduino
         Description     : Arduino
         Hardware Version: 3
         Firmware Version: 4.4
         Vtarget         : 0.3 V
         Varef           : 0.3 V
         Oscillator      : 28.800 kHz
         SCK period      : 3.3 us

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e950f
avrdude: safemode: lfuse reads as 0
avrdude: safemode: hfuse reads as 0
avrdude: safemode: efuse reads as 0
avrdude: NOTE: "flash" memory has been specified, an erase cycle will be performed
         To disable this feature, specify the -D option.
avrdude: erasing chip
avrdude: reading input file "I2c-debug.hex"
avrdude: input file I2c-debug.hex auto detected as Intel Hex
avrdude: writing flash (8684 bytes):

Writing | ################################################## | 100% 1.20s

avrdude: 8684 bytes of flash written
avrdude: verifying flash memory against I2c-debug.hex:
avrdude: load data flash data from input file I2c-debug.hex:
avrdude: input file I2c-debug.hex auto detected as Intel Hex
avrdude: input file I2c-debug.hex contains 8684 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 0.95s

avrdude: verifying ...
avrdude: 8684 bytes of flash verified

avrdude: safemode: lfuse reads as 0
avrdude: safemode: hfuse reads as 0
avrdude: safemode: efuse reads as 0
avrdude: safemode: Fuses OK (E:00, H:00, L:00)

avrdude done.  Thank you.

rsutherland@pi3:~/Samba/RPUno/i2c-debug $ picocom -b 38400 /dev/ttyAMA0
picocom v1.7

port is        : /dev/ttyAMA0
flowcontrol    : none
baudrate is    : 38400
parity is      : none
databits are   : 8
escape is      : C-a
local echo is  : no
noinit is      : no
noreset is     : no
nolock is      : no
send_cmd is    : sz -vv
receive_cmd is : rz -vv
imap is        : 
omap is        : 
emap is        : crcrlf,delbs,

Terminal ready
/0/id?
{"id":{"name":"I2Cdebug^1","desc":"RPUno Board /w atmega328p and LT3652","avr-gcc":"4.8"}}
/1/id?
{"id":{"name":"PwrMgt","desc":"RPUno Board /w atmega328p and LT3652","avr-gcc":"4.9"}}

Thanks for using picocom
rsutherland@pi3:~/Samba/RPUno/i2c-debug $ 

```

So this seems to have worked. The Pi cross compiler for AVR is older than the one on Ubuntu 16.04.


## ^2 Clearance IDE Connector For ICSP Tool

ICSP tools normaly have an IDE connector, now it has room to fit.

![Clearance IDE ICSP](./16197^2,ClearanceIdeConnForICSP.jpg "Clearance IDE ICSP")


## ^2 Clearance Between Pi and Plugable

The Pi does not overhang the plugable connectors now, but it is a tight fit.

![Clearance Pi Plugable](./16197^2,ClearanceBetweenPiAndPlugable.jpg "Clearance Pi Plugable")


## ^1 Vertical Mounting

Pinout was changed so the Pi Zero mounts vertically. The pluggable connectors on RPUno^5 can just barely be removed (or plugged in) with the Pi on the shield in. Take care when removing the Pi since the SD card and/or camera port of the Pi Zero can be damaged.

![Vertical Mounting](./16197^1,MntOnRPUno^5.jpg "Vertical Mounting")


## ^0 Mounting

Show how this version of the shield mounts on a RPUno^4. 

![Shield Mounting](./16197^0,AfterTesting.jpg "Shield Mounting")


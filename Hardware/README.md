# Hardware

## Overview

This board connects a Pi Zero to a multi-drop RS-422 bus and a RPUno board.

## Inputs/Outputs/Functions

```
        Pi Zero power is from a 5V SMPS regulator powered by the Vin pin
        Pi Zero UART connects to RS-422 pairs (e.g. Pi Tx to RPU_BUS Rx)
        RS-422 pairs daisy-chain to other RPUadpt boards
        RPUno UART connects to RS-422 pairs (e.g. 328p Rx to RPU_BUS Rx)
        An out of band management pair is also available
        SPI is available between Pi Zero and local RPUno
```

## Uses

```
        Use solar power (e.g. 20W PV needed) from a RPUno to run a Pi Zero
        Pi with Linux, ssh, Python, and AVR toolchain is a compelling platform.
        Allows (locally compiled) updates without exposing serial ports to a network.
```

## Notice

```
        If the SBC bootloads local RPUno it must not turn off VIN power at reset.
```


# Table Of Contents

1. [Status](#status)
2. [Design](#design)
3. [Bill of Materials](#bill-of-materials)
4. [How To Use](#how-to-use)


# Status

![Status](./status_icon.png "RPUno Status")

```
        ^1  Done: Design, Layout, BOM, Review*, Order Boards,
            WIP: Assembly,
            Todo: Testing, Evaluation.
            *during review the Design may change without changing the revision.
            Note: Power for the RPU_BUS is now taken from +5V of the MCU board. The 
            SBC (Pi Zero) takes power from VIN. Changed Pi pinout so it will mount to a 
            right angle 2x20 header (Pi Zero is perpendicular to the shield). 
            Move management ICSP header so it clears the Pi Zero header.
            
        ^0  Done: Design, Layout, BOM, Review*, Order Boards, Assembly, Testing,
            WIP: Evaluation.
            Todo:  
            *during review the Design may change without changing the revision.
```

Debugging and fixing problems i.e. [Schooling] [./Schooling/].

Setup and methods used for [Evaluation] [./Evaluation/]


# Design

The board is 0.063 thick, FR4, two layer, 1 oz copper with ENIG (gold) finish.

![Top](./Documents/16197,Top.png "RPUpi Top")
![TAssy](./Documents/16197,TAssy.jpg "RPUpi Top Assy")
![Bottom](./Documents/16197,Bottom.png "RPUpi Bottom")
![BAssy](./Documents/16197,BAssy.jpg "RPUpi Bottom Assy")


## Electrical Parameters (Typical)

```
        VIN pin needs 7V to 30V at 15W for SMPS regulator that powers Pi Zero
        5V is used to power RPU_BUS
        This is OSH so refer to the parts used for storage and operation limits.
```

## Mounting

```
        Shield or Mezzanine
```

## Electrical Schematic

![Schematic](./Documents/16197,Schematic.png "RPUpi Schematic")

## Testing

Check correct assembly and function with [Testing] [./Testing/]


# Bill of Materials

Import the [BOM][./Design/16197,BOM.csv] into LibreOffice Calc (or Excel) with semicolon separated values, or use a text editor.


# How To Use

## Pi Zero Setup 

[Linux] is used on the Pi Zero. Bascily the Pi Zero is a Single Board Computer (SBC) running Linux. It can be used as a Linux host machine and has enough memory and processing power for applications (like the AVR toolchain) as well as self-hosted compiling for its own applications and services like an MQTT broker. It can even act as a small SCADA system for multiple RPUno boards.

[Linux]: ./linux.md

The BCM2835 Broadcom chip used in the Raspberry Pi Zero is a Cortex-A7 (aka ARM11) running at 1 GHz it is well supported by many Linux distributions, with [Raspbian] being the main distribution that the Raspberry Pi Foundation points to. 

[Raspbian]: https://www.raspbian.org/

__WARNING: The shield will be damaged if removed from a powered RPUno board. Before separating a shield from the RPUno it is very important to step back, take a breath and double check that all power sources are disconnected.

## Serial

The Pi serial port (RX and TX) connects to transceivers that drive differential pairs and the crossover output from those transceivers go to the shield's header for Tx and Rx. This allows the Pi Zero to talk to the MCU node with serial as would normally be expected.  But now the serial is also on the differential pairs, and can be daisy-chained with CAT5 to other nodes using an RPUadpt shield. 

CTS and RTS lines are on BCM 16 and 17 as the ALT3 option. Only BCM 17 is on the original 26 pin connector. BCM 16 is on the new 40 pin. I use this [rpirtscts] command-line utility for enabling hardware flow control on the Pi Zero serial port. 

[rpirtscts]: https://github.com/epccs/RPUpi/tree/master/RPiRtsCts

The Pi can run avrdude which allows programming AVR's serial bootloaders. 

## Management

The shield has a bus manager, though its firmware needs to be finished. It has access to enable/disable each transceiver's receiver and/or driver. This means that each (or all) node(s) can be isolated from the serial bus (both host and/or MCU), the implications are significant. The original intent was to allow boot loading with the point to point full duplex tools (e.g. optiboot/xboot and avrdude). It may also allow a user board accidently programmed to hold its TX line low to be locked off the bus. The goal is to allow boot loading a new executable binary image no matter how goofed up the application is, which is a very nice thing. 

## Transceiver (RS-485)

The transceivers have a built-in fail-safe bias, which is a little complicated to explain, but it makes an undriven bus (e.g. the failed condition) a defined true or high state. That is if I turn off all the transceiver's drivers the bus is guaranteed to be in a defined state (e.g. the true state). I have setup the transceivers to turn off the driver while the UART outputs a true (e.g. its default). That further means the driver will automatically drive the bus only when data is sent (e.g. drives when a zero or false bit is transmitted), so nothing needs to be done in software to turn off the bus (except to not talk, which can be a challenge). The bus manager can be used (I2C) to enable/disable the transceiver's receiver and/or driver if it has a firmware loaded to do so.

## SPI

The Pi also has an SPI port that is level converted on the shield to work with the local RPUno. The SPI port can also be used for ICSP programming of the local microcontroller on the RPUno, although this requires that VIN to the RPUpi shield is not disconnected when the MCU is put in programming mode (e.g. on RPUno IO2 must have a pull-up so it does not float).

## I2C

The Pi I2C port is not connected to the onboard bus manager. The Pi has 1.8k pull-ups to 3.3V, and when the shield powers down VIN it will pull down the I2C lines as the Pi can no longer sustain its 3.3V supply. This means communication between the Pi and bus manager needs to occur like a normal host, e.g. by way of the UART (or SPI).


## SD

Card [corruption] seems to happen when the SD card is doing wear leveling operations at the time power is removed. It may be possible with this setup to push the shutdown button and have that run a script that before halting tells the node to wait for a while and then make sure the current draw from the battery is stabilized before turning off VIN or disconnecting the battery. The idea is that wear leveling will draw current somewhat randomly until all the page updates are done. The BCM2835 will be halted, and looping (it is checking one of the I2C pins) and using a steady current draw. So when the current draw is stable the ware leveling should be done and safe to disconnect power.

[corruption]: http://hackaday.com/2016/08/03/single-board-revolution-preventing-flash-memory-corruption/

## Agile

I'm not an Agile developer but these are some of my thoughts. When developing software or hardware the only thing that seems to get results is to first make something, anything really, and then start fixing the broken stuff. Add new stuff to push it in the desired direction (that is not evolution BTW, but it is also not intelligent design, it is something in between). Keep iterating by fixing, and adding (and sometimes change direction if needed). It will never be truly finished, but at some point, things start to click and reveal if it is useful or useless. To speed up this process the hardware needs to be remotely programmable, and I guess I like how an AVR with a serial bootloader does this trick. It allows full hardware control by uploading an executable binary image compiled from C, or C++ if you can deal with heap and stack usage better than I know how too. Allowing a peasant, like myself, direct access to the machine registers is normally a bad idea, but that is what maximizes options. Understand that hardware of this nature has no safety guards, again the uploaded machine code has full register level access (the training wheels are off).

## OpenOCD

The Pi can run OpenOCD to program ARM MCU's (SWD interface).

## Security

A Yun [worm] is now in wild, the AVR hardware on YUN is exposed through OpenWRT services (telnet and ilk). That means that data can be sent to the AVR hardware without going through an authentication system like SSH (which does run on OpenWRT). The worm injects crafted raw data a way that causes the AVR to use up all of it SRAM and then cause a heap overflow and then set the AVR control registers.

[worm]: http://hackaday.com/2016/11/11/arduworm-a-malware-for-your-arduino-yun/

One lesson I see is to not expose the raw hardware. The AVR serial is user implemented, so it can overrun. In my serial examples, I try to keep track of the serial line length and ignore bytes when a line is too long, but that is just me. I also use C and do not use malloc functions so only the stack memory is used (not heap memory). Arduino compiles with C++ which mixes heap and stack memory usage. C++ objects in local scope may have members allocated on the stack (e.g. pointers) and its constructed type objects on the heap (e.g. template of MyClass ). The C++ language is sometimes frowned on for embedded bare metal applications due to these issues. Linux is an OS and as such will manage the memory, it sets the MMU to indicate overflows outside an allocated memory space. FreeRTOS provides tools to help detect the problem as well. An OS is probably a good idea if you have to use C++.


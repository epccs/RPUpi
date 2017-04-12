# RPUpi

From <https://github.com/epccs/RPUpi>

## Overview

Shield used to connect a Pi Zero (or W) host to a multidrop RS-422 (RPU_BUS) and an [RPUno] or [Irrigate7]. The multidrop bus can connect additional [RPUno] or [Irrigate7] with [RPUadpt] shields.

[HackADay](https://hackaday.io/project/16424-rpupi)

[Forum](http://rpubus.org/bb/viewforum.php?f=8)

[OSHpark](https://oshpark.com/shared_projects/KWIJgjF8)

[RPUno]: https://github.com/epccs/RPUno
[Irrigate7]: https://github.com/epccs/Irrigate7
[RPUadpt]: https://github.com/epccs/RPUadpt
[RPUftdi]: https://github.com/epccs/RPUftdi

## Status

![Status](./Hardware/status_icon.png "Status")

## [Hardware](./Hardware)

Hardware files are in Eagle, there is also some testing, evaluation, and schooling notes for referance.

## Example

A Pi Zero host computer controls the microcontrollers through a serial (UART) interface. The microcontrollers run a command processor which accepts interactive textual commands and operate their peripherals as a bare metal machine. The microcontroller firmware is compiled from source that is version managed on GitHub, it is then compiled and uploaded to the targets serial bootloader with a software toolchain. Since this needs to work from a Pi Zero (as well as from a desktop) the only reasonably easy option I see is to use AVR microcontrollers since a no fuss toolchain for them is on Debian (which is where Pi's Raspbian and the OS I use on my desktop is from). The only network port that I expose on the Pi is SSH so I have a way to do remote access, the Pi Zero does not need to map the raw serial port to the network because the development toolchain is on the Pi Zero.

![MultiDrop](./Hardware/Documents/MultiDrop.png "MultiDrop")

To bootload the AVR from the Pi's serial port, first it needs to be disabled as a [debug port] and then enabled for [serial] use, also the [RTS and CTS] pin functions should be enabled (since nRTS can be used to start the bootloader just like nDTR is from a desktop). 

[debug port]: ./Hardware/Testing/linux.md#serial
[serial]: ./Hardware/Testing/linux.md#serial
[RTS and CTS]: ./RPiRtsCts

The Pi Zero also needs a way to initiate a [halt] from a push button. When the RPUpi shield is mounted on an [RPUno] the VIN pin can be powered off with a somewhat involved [power management] firmware technique.  This allows a manual shutdown, or the RPUno can request a halt with an I2C command,  or the Pi can request the RPUno to request a halt over its serial interface (or perhaps a heartbeat to keep it active). 

[halt]: ./Shutdown
[power management]: https://github.com/epccs/RPUno/tree/master/PwrMgt

## AVR toolchain

The core files for this board are in the /lib folder. Each example has its files and a Makefile in its own folder. The toolchain packages that I use are available on Ubuntu and Raspbian. 

```
sudo apt-get install git gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/RPUno
```

[gcc-avr](http://packages.ubuntu.com/search?keywords=gcc-avr)
[binutils-avr](http://packages.ubuntu.com/search?keywords=binutils-avr)
[gdb-avr](http://packages.ubuntu.com/search?keywords=gdb-avr)
[avr-libc](http://packages.ubuntu.com/search?keywords=avr-libc)
[avrdude](http://packages.ubuntu.com/search?keywords=avrdude)
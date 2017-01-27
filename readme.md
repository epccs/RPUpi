# RPUpi

From <https://github.com/epccs/RPUpi>

## Overview

Shield used to connect a Pi Zero host to a multidrop RS-422 (RPU_BUS) and an [RPUno] or [Irrigate7]. The multidrop bus can connect additional [RPUno] or [Irrigate7] with [RPUadpt] shields.

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

A Pi Zero host computer controls the microcontrollers through the serial (UART) interface. The microcontrollers run a command processor which accepts interactive textual commands and operates the peripherals as a bare metal system. The microcontroller firmware can be compiled from source that is version managed on GitHub, it can then be uploaded to the targets bootloader with avrdude. The Pi Zero host can be remotely accessed using SSH. The Pi Zero does not need to expose a raw serial port to the network because the microcontrollers development toolchain can reside on the Pi Zero.

![MultiDrop](./Hardware/Documents/MultiDrop.png "MultiDrop")

To bootload the AVR from the Pi serial port, first it needs to be disabled as a [debug port] and then enabled for use as a [serial] device, also the [RTS and CTS] pin functions can be enabled (since nRTS can be used in place of nDTR, to start the bootloader). 

[debug port]: ./Hardware/Testing/linux.md#serial
[serial]: ./Hardware/Testing/linux.md#serial
[RTS and CTS]: ./RPiRtsCts

The Pi also needs a way to [halt] from an onboard button push. When mounted on an [RPUno] the shield VIN pin can be powered off using a somewhat involved [power management] firmware technique.

[halt]: ./Shutdown
[power management]: https://github.com/epccs/RPUno/tree/master/PwrMgt

## AVR toolchain

The core files for this board are in the /lib folder. Each example has its files and a Makefile in its own folder. The toolchain packages that I use are available on Ubuntu and Raspbian. 

* sudo apt-get install [gcc-avr]
* sudo apt-get install [binutils-avr]
* sudo apt-get install [gdb-avr]
* sudo apt-get install [avr-libc]
* sudo apt-get install [avrdude]
    
[gcc-avr]: http://packages.ubuntu.com/search?keywords=gcc-avr
[binutils-avr]: http://packages.ubuntu.com/search?keywords=binutils-avr
[gdb-avr]: http://packages.ubuntu.com/search?keywords=gdb-avr
[avr-libc]: http://packages.ubuntu.com/search?keywords=avr-libc
[avrdude]: http://packages.ubuntu.com/search?keywords=avrdude[avrdude](http://packages.ubuntu.com/search?keywords=avrdude)
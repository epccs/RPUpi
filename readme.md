# RPUpi

From <https://github.com/epccs/RPUpi>

## Overview

Shield used to connect a Pi Zero (or W) host to a multidrop serial (RPU_BUS) and an [RPUno], [RPUlux] or [Irrigate7]. The multidrop bus can connect additional [RPUno] or [Irrigate7] with [RPUadpt] shields.

[Forum](https://rpubus.org/bb/viewforum.php?f=8)

[HackADay](https://hackaday.io/project/16424-rpupi-a-shield-for-pi-zero-and-rs-422-over-cat5)

[RPUno]: https://github.com/epccs/RPUno
[RPUlux]: https://github.com/epccs/RPUlux
[Irrigate7]: https://github.com/epccs/Irrigate7
[RPUadpt]: https://github.com/epccs/RPUadpt
[RPUftdi]: https://github.com/epccs/RPUftdi

## Status

Available through [Tindie](https://www.tindie.com/products/10350/)

![Status](./Hardware/status_icon.png "Status")

## [Hardware](./Hardware)

Hardware files are in Eagle, there is also some testing, evaluation, and schooling notes for referance.

## Example

A Pi Zero host computer interfaces with several microcontrollers through its serial (UART) interface in this simplified diagram. 

![Simplified Diag](./Hardware/Documents/SimplifiedBlockDiag.png "Simplified Diag")

Both the Pi and microcontroller(s) can run simple UART based serial communication software. I prefer textual commands, so my examples generally use a textual command processor which echoes back the command like an interactive terminal, the commands are used by the firmware command processor to run routines or configure state machines on the bare metal microcontrollers. The Pi host the toolchain needed to compile the firmware from the source which can be version managed as I do on GitHub, it can also use the toolchains uploader to push a firmware image to the targets serial bootloader. The result is one or more bare metal machine(s) that can place complete attention on a task (e.g. no half-baked network stack to timeshare), and a separate networking machine that is doing the many activities that are needed for modern networking with a heavily tested and well-known networking stack. This is not an IoT system, it is a computer hosting an operating system connected to controllers, instruments or peripherals. IoT is the home of bootloaders with built-in half-baked network stacks that update from the cloud or expose undesired ports that present a serious security risk.

![MultiDrop](./Hardware/Documents/MultiDrop.png "MultiDrop")

To bootload the AVR from the Pi's serial port, first it needs to be disabled as a [debug port] and then enabled for [serial] for Linux to use, also the [RTS and CTS] pin functions should be enabled (since nRTS is used to start the bootloader). 

[debug port]: ./Hardware/Testing/linux.md#serial
[serial]: ./Hardware/Testing/linux.md#serial
[RTS and CTS]: ./RPiRtsCts

The Pi Zero also needs a way to initiate a [halt] from a push button. When the RPUpi shield is mounted on an [RPUno] the VIN pin can be powered off with a somewhat involved [power management] firmware technique.  This allows a manual shutdown, or the RPUno can request a halt with an I2C command,  or the Pi can request the RPUno to request a halt over its serial interface (or perhaps a heartbeat to keep it active). 

[halt]: ./Shutdown
[power management]: https://github.com/epccs/RPUno/tree/master/PwrMgt

## AVR toolchain

The core files for this boards bus manager are in the /lib folder. Each example has its files and a Makefile in its own folder. The toolchain packages that I use are available on Ubuntu and Raspbian. 

```
sudo apt-get install git gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/RPUpi
```

* [gcc-avr](https://packages.ubuntu.com/search?keywords=gcc-avr)
* [binutils-avr](https://packages.ubuntu.com/search?keywords=binutils-avr)
* [gdb-avr](https://packages.ubuntu.com/search?keywords=gdb-avr)
* [avr-libc](https://packages.ubuntu.com/search?keywords=avr-libc)
* [avrdude](https://packages.ubuntu.com/search?keywords=avrdude)
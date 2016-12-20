# RPUadpt

From <https://github.com/epccs/RPUpi>

## Overview

Shield used to connect a Pi Zero host to [RPUno], and a multidrop RS-422 (RPU_BUS) that can connect additional RPUno with [RPUadpt] shields.

[HackADay](https://hackaday.io/project/16424-rpupi)

[Forum](http://rpubus.org/bb/viewforum.php?f=8)

[OSHpark](https://oshpark.com/shared_projects/KWIJgjF8)

[RPUno]: https://github.com/epccs/RPUno
[RPUadpt]: https://github.com/epccs/RPUadpt

## Status

![Status](https://raw.githubusercontent.com/epccs/RPUftdi/master/Hardware/status_icon.png "Status")

## [Hardware](./Hardware)

Hardware files are in Eagle, there is also some testing, evaluation, and schooling notes for referance.

## Example

A Raspberry Pi Zero should be able to work as a host computer for RPU_BUS microcontrollers from its serial (UART) interface. The goal is to run a command processor that accepts interactive textual commands to configure and operate the microcontroller bare metal software. This means the microcontroller can perform instrumentation task like event capture for a flow meter or the control of an array of half-bridge power outputs for a water pump. Sometimes a control loop needs to be tightly integrated, for example, a self-balancing BLDC controller would probably work better if the bare metal combined the BLDC control with an error amplifier and the PID for the balance signal, and another error amplifier and PID for hall sensor location signal. The two PID's would have to be configured so they oscillate in a desirable way (I guess the balance would oscillate while hunting for the location).

![MultiDrop](./Hardware/Documents/MultiDrop.png "MultiDrop")

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
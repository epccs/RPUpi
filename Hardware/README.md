# Hardware

## Overview

This board connects a Pi Zero (not provided) to a multi-drop RS-422 bus and a [RPUno] board. 

[RPUno]: https://github.com/epccs/RPUno

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
        Use solar power (e.g. 20W PV needed) from a [RPUno] to run a Pi Zero
        Pi with Linux, ssh, Python, and AVR toolchain is a compelling platform.
        Allows (locally compiled) updates without exposing serial ports to a network.
```

## Notice

```
        If the Pi Zero bootloads its local [RPUno] then the VIN power must 
        not turn off after reset (at time of writing the defaut setup should 
        work). 
        
        I advise acquiring a Pi Zero, setting it up on your own network and 
        learn how to use the serial, I2C, and SPI interfaces before acquiring 
        this board. 
```


# Table Of Contents

1. [Status](#status)
2. [Design](#design)
3. [Bill of Materials](#bill-of-materials)
4. [How To Use](#how-to-use)


# Status

![Status](./status_icon.png "RPUpi Status")

```
        ^3  Done: 
            WIP: Design,
            Todo:  Layout, BOM, Review*, Order Boards, Assembly, Testing, Evaluation.
            *during review the Design may change without changing the revision.
            I2C add 182 Ohm between shield pins and bus manager
            RS-422 Buffer needs Power (power U3 with +3V3) 
            Flip U3E and U3F with U1E and U1F.

        ^2  Done: Design, Layout, BOM, Review*, Order Boards, Assembly, 
            WIP: Testing,
            Todo: Evaluation.
            *during review the Design may change without changing the revision.
            Run I2C from MCU board to bus manager with 3V3 pull-up (not to Pi).
            Add Option to use I2C1 with a ATmega328pb (note the Pi will pull these lines down when power is off).
            Add pull up to the Pi Tx line.
            Add 10k on SCK and MOSI so the 74LVC07 can't damage the MCU board's SPI pins.
            Have PI3V3 power the 74LVC07, outputs hi-z (IOFF) when power is off.
            Move Pi back .2"
            Add 74LVC07 buffer to Pi serial interface so it will hi-z (IOFF) nRTS and PI_TX when PI3V3 is off.
            Populate with 12MHz crystal to use 250kbit rate on DTR pair.
            location: 2017-5-26 Test Bench, hacked U3 to power serial buffers witout Pi.

        ^1  Done: Design, Layout, BOM, Review*, Order Boards, Assembly, Testing,
            WIP: Evaluation.
            location: 2017-2-6 Test Bench, used as an RPUadpt with an RPUno^4.
            location: 2017-4-11 Scrap (in que).
```

Debugging and fixing problems i.e. [Schooling](./Schooling/)

Setup and methods used for [Evaluation](./Evaluation/)


# Design

The board is 0.063 thick, FR4, two layer, 1 oz copper with ENIG (gold) finish.

![Top](./Documents/16197,Top.png "RPUpi Top")
![TAssy](./Documents/16197,TAssy.jpg "RPUpi Top Assy")
![Bottom](./Documents/16197,Bottom.png "RPUpi Bottom")
![BAssy](./Documents/16197,BAssy.jpg "RPUpi Bottom Assy")


## Electrical Parameters (Typical)

```
        VIN pin needs 7V to 30V (RPUno has 12.8V) at up to 4W for SMPS regulator to power a Pi Zero
        5V pin needs 150mA (RPUno has 1+ Amp) to power RPU_BUS
```

## Mounting

```
        Shield or Mezzanine
```

## Electrical Schematic

![Schematic](./Documents/16197,Schematic.png "RPUpi Schematic")

## Testing

Check correct assembly and function with [Testing](./Testing/)


# Bill of Materials

Import the [BOM](./Design/16197,BOM.csv) into LibreOffice Calc (or Excel) with semicolon separated values, or use a text editor.


# How To Use

## ICSP

[ArduinoISP] sketch on an [Uno] with a SPI level converter is the [ICSP] tool I use to program the bus manager with the [Host2Remote] firmware. I then plug the RPUpi into an [RPUno] board and load my application firmware (e.g. [Solenoid] is used with a [K3] board). The RPUpi's Pi Zero host can communicate through RS-422 with other RPUno boards that have an [RPUadpt] pluged onto them. The additional boards will need the [Remote] firmware on there bus manager and have the rpu_bus address set in EEPROM.

[ArduinoISP]: https://github.com/arduino/Arduino/blob/master/build/shared/examples/11.ArduinoISP/ArduinoISP/ArduinoISP.ino
[Uno]: https://www.adafruit.com/product/50
[ICSP]: https://github.com/epccs/Driver/tree/master/ICSP
[Host2Remote]: ../Host2Remote
[Remote]: https://github.com/epccs/RPUadpt/tree/master/Remote
[Solenoid]: https://github.com/epccs/RPUno/tree/master/Solenoid
[K3]: https://github.com/epccs/Driver/tree/master/K3
[RPUadpt]: https://github.com/epccs/RPUadpt
[RPUno]: https://github.com/epccs/RPUno

## Pi Zero Setup 

The Pi Zero is a Single Board Computer (SBC) running [Linux]. I use it as a host machine since it has enough memory and processing power for the AVR toolchain as well as self-hosted compiling and other applications and services. My use is kind of like a small headless SCADA system controller for the RPUno boards I daisy-chained with CAT5. It is not an IoT setup since the control system is self-contained, and I still don't see a compelling reason to send data about my garden to the Nebula or is relative Nimbus.

[Linux]: ./Testing/linux.md

The BCM2835 Broadcom chip used in the Raspberry Pi Zero is a Cortex-A7 (aka ARM11) running at 1 GHz it is well supported by many Linux distributions, with [Raspbian] being the main distribution that the Raspberry Pi Foundation points to. 

[Raspbian]: https://www.raspbian.org/

__WARNING: The shield will be damaged if removed from a powered RPUno board. Before separating a shield from the RPUno it is very important to check that all power sources are disconnected.__

## Serial

The Pi serial port (RX is BCM 15 and TX is BCM 14) connects to transceivers that drive differential pairs and then crossover to go to the shield's header for Tx and Rx (e.g. Pi BCM Rx goes to Tx pin on the shield for MCU's UART). This allows the Pi Zero to talk to the MCU's UART with serial as is expected. But the serial is also copyed on the differential pairs, and can be daisy-chained with CAT5 to other nodes using an RPUadpt shield. 

![Pi Pinout](./Documents/Pi-pinout-graphic.png "Pi Pinout")

The Pi's handshake lines nCTS and nRTS lines are on BCM 16 and 17 when the ALT3 option is active. BCM 17 is on the original 26 pin Pi connector, but BCM 16 is on the new 40 pin connector. I use this [rpirtscts] program as a command-line utility for enabling hardware flow control on the Pi Zero serial port. 

[rpirtscts]: https://github.com/epccs/RPUpi/tree/master/RPiRtsCts

The Pi Zero serial lines (Rx, Tx, nRTS, and nCTS) are interfaced through a 74LVC07A buffer which is powered from the Pi's 3V3 power. When the Pi Zero is powered off the IOFF feature of the buffer will turn off (or hi-z) it's open collector output which will allow a pull-up to set the proper value on nRTS and the Tx line from the host. This allows the RPU_BUS to be used when the Pi Zero is powered off (or not plugged in).

The Pi can run avrdude which allows programming AVR's serial bootloaders. 

## Management

The shield has a bus manager, though its firmware is in development. It has access to enable/disable each transceiver's receiver and/or driver. This means that each (or all) node(s) can be isolated from the serial bus (both host and/or MCU), the implications are significant. The original intent was to allow boot loading with a point to point full duplex mode (e.g. optiboot/xboot and avrdude need this). The goal is to allow boot loading a new executable binary image even if the bare metal application is severely goofed up (similar to an Arduino Uno). 

## Transceiver (RS-485)

The transceivers have a built-in fail-safe bias, which is a little complicated to explain, but it makes an undriven bus (e.g. the failed condition) a defined true or high state. That is if I turn off all the transceiver's driving the bus (only one should do that), it is guaranteed to be in a defined state (e.g. the true state). I have setup the transceivers to turn off the driver while the UART output is a true (e.g. its default value). That further means the driver will automatically drive the bus only when data is sent (e.g. drives when a zero or false bit is transmitted), so nothing needs to be done in software to turn off the bus (except to not talk, which can be a challenge). The bus manager can be used (I2C) to enable/disable the transceiver's receiver and/or driver (requires your firmware on the bus manager to do so, this is only hardware).

## SPI

The Pi Zero SPI lines (MOSI, SCK, MISO) are interfaced through a 74LVC07A buffer which is powered from the Pi's 3V3 power. When the Pi Zero is powered off the IOFF feature of the buffer will turn off (or hi-z) it's open collector output which will allow a pull-up (IOREF from the MCU board) to set the value on SCK and the MOSI line from the host. 

Known issues: RPUno maps its SPI lines to the plugable connectors for digital control (e.g. the K3 board needs the lines). I am not yet sure the best way to offer this.

## I2C

The Pi I2C port is not connected to the onboard bus manager. The Pi has 1.8k pull-ups to 3.3V, and when the shield powers down VIN it will pull down the I2C lines as the Pi can no longer sustain its 3.3V supply. 

A future update may include an ATmega328pb that has an extra I2C interface that could be used as an independent channel between the Pi Zero and the manager without locking up the MCU to the manager when the Pi Zero is powered down. 

## SD

Card [corruption] seems to happen when the SD card is doing wear leveling operations at the time power is removed. It may be possible with this setup to push the shutdown button and have that run a script that before halting tells the node to wait for a while and then make sure the current draw from the battery is stabilized before turning off VIN or disconnecting the battery. The idea is that wear leveling will draw current somewhat randomly until all the page updates are done. The BCM2835 will be halted, and looping (it is checking one of the I2C pins) and using a steady current draw. So when the current draw is stable the wear leveling should be done and safe to disconnect power.

[corruption]: http://hackaday.com/2016/08/03/single-board-revolution-preventing-flash-memory-corruption/

## Agile

I'm not an Agile developer but when developing software or hardware the only thing that seems to get results is to make the first attempt at something, and then start fixing the broken stuff. Then add new stuff to push it in the desired direction. Keep iterating by fixing, and adding (and sometimes change direction when needed). It will never be truly finished, but at some point, things start to click and reveal if it is useful or useless. 


## Security

Hardware with connectivity is easy to do presently but most of it ends up in the landfill quickly. The problem is that connectivity is like an exposed surface and is difficult to maintain. A device with RS-232 has a very limited connectivity surface. Only the serial interface can be used to control the device and it is connected to a computer (e.g. Pi Zero) that the user maintains. RS-422 connectivity is similar to RS-232 but the differential pairs can run over a 1km, and to multiple devices. 

Serial software is typically done to control a UART for RS-232 and does not know how to control the differential transceiver for RS-422. This problem can be compensated for if the transceiver includes hardware for failsafe biasing, which basically means the differential lines have a defined state when not driven. When failsafe is used the transceiver can also enable its transmitter automatically for the opposite state.  That results in the ability of software done for RS-232 to also work over RS-422. Connecting multiple devices over RS-422 to a single computer reduces the maintenance work load. 

Programming the devices on the serial interface is less about security and more about keeping things useful. I like how an AVR with a serial bootloader does this. It uploads programs that have full hardware control since they are an executable binary image compiled from C (or C++). This allows me direct access to the machine registers which maximizes what I can do with the controller. Understand that hardware of this nature has no safety guards since the uploaded machine code has full register level access. I would suggest only using boards with sufficient documentation and have the necessary hardware safety guards.

A Yun [worm].

[worm]: http://hackaday.com/2016/11/11/arduworm-a-malware-for-your-arduino-yun/

One lesson I see is to not expose the raw serial hardware on a network (i.e. minimize the exposed surface). Although I try to keep track of the serial line length and ignore bytes when a line is too long in my examples, I bet there are mistakes. Since I don't set the Pi Zero to show the network its serial interface the mistakes on my AVR software should not be a security problem. 


## AVR vs ARM

To prevent heap memory fragmentation on the AVR I use C and avoid the malloc functions. This means only the stack and static memory is used. Compiling with C++ can mix heap and stack memory usage in complicated ways I don't understand (and have had problems with). On the Pi, C++ is fine since Linux can manage the heap, but bare metal does nothing for heap memory management it just makes a fragmented mess.  Anyway, I tend to use Python on the Pi, so my C++ is not good enough to know when I am using the heap.

The AVR has a single clock domain (for the most part), so the GPIO, peripherals and flash memory operate in a predictable and repeatable way. That is to say, an AVR will pull an instruction from flash and use it to fiddle with an output in a predictable consistent way, however a fast machine may need to wait for instructions and will experience timing variations from the instruction cashing system as well as other clock domain transitions that cause timing variability in devices that run with multiple clock domains (e.g. MCU at higher speeds have to pre-fetch instructions from flash that runs in a slower clock domain, and write to GPIO in yet another slower clock domain).

When a task is small and/or simple enough to be done with an AVR it takes less time and effort to just use an AVR. It uses the same GCC toolchain as the big boys (GCC also does x86, ARM, MIPS...), but the AVR is a simple device with a fraction of the documentation to understand.
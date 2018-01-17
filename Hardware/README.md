# Hardware

## Overview

This board connects a Pi Zero [W] (your computer is not provided with this board) to a multi-drop RS-422 bus and a [RPUno] or [Irrigate7] board. 

[RPUno]: https://github.com/epccs/RPUno
[Irrigate7]: https://github.com/epccs/Irrigate7

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
        If the Pi Zero bootloads its local board (the one under the RPUpi) then 
        the VIN power from the local board must not turn off after a reset 
        (at time of writing the defaut setup should work). 
        
        I advise acquiring a Pi Zero, setting it up on your own network and 
        learn how to setup and use the network, ssh, AVR toolchain, and 
        the serial port (e.g. /dev/ttyAMA0 with RTS/CTS enabled). 
```


# Table Of Contents

1. [Status](#status)
2. [Design](#design)
3. [Bill of Materials](#bill-of-materials)
4. [How To Use](#how-to-use)


# Status

![Status](./status_icon.png "RPUpi Status")

```
        ^3  Done: Design, Layout, BOM, Review*, Order Boards, Assembly, Testing,
            WIP: Evaluation.
            Todo: 
            *during review the Design may change without changing the revision.
            I2C added 182 Ohm between shield pins and bus manager
            RS-422 Buffer has Power (power U3 with +3V3) 
            Swaped U3E and U3F with U1E and U1F.
            Through hole LED so it can be moved outside the enclosure.

        ^2  Done: Design, Layout, BOM, Review*, Order Boards, Assembly, Testing,
            WIP: Evaluation.
            location: 2017-5-26 Test Bench, hacked U3 to power serial buffers witout Pi.

        ^1  Done: Design, Layout, BOM, Review*, Order Boards, Assembly, Testing,
            WIP: Evaluation.
            location: 2017-2-6 Test Bench, used as an RPUadpt with an RPUno^4.
            location: 2017-4-11 Scrap (in que, some parts salvaged 2017-7-18).
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

Your [Raspberry Pi] is your computer, you are the expert because I surely am just barely at the noob level. Don't buy this board and expect that I can help with your computer, I can't, I can barely keep my garden alive. So far I have got SSH to work and the AVR toolchain and was able to bootload an ATmega328p over the RS-422. The serial console program I use is picocom. 

[Raspberry Pi]: https://www.raspberrypi.org/forums/

## Simplified Serial

To allow a hardware UART on a Pi Single Board Computer to interface with an AVR UART the following connections can be used. Notice that the AVR is powered by 3.3V.

![Pi2AVR](./Documents/Pi2AVR.png "Pi to AVR")

Now lets set things up to allow use with a 5V AVR and the ability to turn the SBC off (IOFF) without it pulling down on the serial port lines.

![Pi2AVR_wIOFF](./Documents/Pi2AVR_wIOFF.png "Pi to AVR with IOFF")

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

## Pi Zero Setup 

The Pi Zero is a Single Board Computer (SBC) running [Linux]. I use it as a host machine since it has enough memory and processing power for the AVR toolchain as well as self-hosted compiling for itself and other applications and services (it furnishes just enough Linux at the edge). My use is sort of like a headless SCADA system for the control boards I have daisy-chain with the RS-422 bus. This is not an IoT setup rather it is a classic control system (but it is headless). The control boards have example firmware that provides a minimalistic command line interface for the Pi SBC to interact with. Essentially the controller just listens for an address and a command to perform from the RS-422 bus, the Pi SBC does all the network stuff (e.g. SSH) and runs the high-level programs (e.g. Python).

[Linux]: ./Testing/linux.md

The BCM2835 Broadcom chip used in the Raspberry Pi Zero is an ARM11 running at 1 GHz it is well supported by the [Raspbian] distribution that the Raspberry Pi Foundation works on. 

[Raspbian]: https://www.raspbian.org/

__WARNING: The shield will be damaged if removed from a powered RPUno board. Before separating a shield from the RPUno it is very important to check that all power sources are disconnected.__

## Serial

The Pi serial port (RX is BCM 15 and TX is BCM 14) connects to transceivers that drive differential pairs and then crossover to go to the shield's header for Tx and Rx e.g. Pi BCM Rx goes through a transceiver to Tx pin on the shield for MCU's UART. This allows the Pi Zero to talk to the shield's UART with serial as is expected. But the serial is also copied on the transceiver differential pairs, and can thus be daisy-chained to other nodes using an RPUadpt shield and CAT5 cables. 

![Pi Pinout](./Documents/Pi-pinout-graphic.png "Pi Pinout")

The Pi's handshake lines nCTS and nRTS lines are on BCM 16 and 17 when the ALT3 option is active. BCM 17 is on the original 26 pin Pi connector, but BCM 16 is on the new 40 pin connector. I use this [rpirtscts] program as a command-line utility for enabling hardware flow control on the Pi Zero serial port. 

[rpirtscts]: https://github.com/epccs/RPUpi/tree/master/RPiRtsCts

The Pi Zero serial lines (Rx, Tx, nRTS, and nCTS) are interfaced through a 74LVC07A buffer which is powered from the Pi's 3V3 power. When the Pi Zero is powered off the IOFF feature of the buffer will turn off (or hi-z) it's open collector output which will allow a pull-up to set the proper value on nRTS and the Tx line from the host. This allows the RPU_BUS to be used when the Pi Zero is powered off (or not plugged in). Also, a 74LVC07A buffer is used between the transceiver and the shield so the MCU on the shield can run at 3.3V or 5V, this buffer is powered from the shield so the shield UART can talk on the RS-422 when the Pi Zero is powered down (a big league ability).

When the Pi's handshake lines are enabled picocom and avrdude work like an RPUftdi from a Ubuntu computer which sets nDTR and nRTS which allows the bus manager to bradcast a bootload address that in the case of avrdude allows programming the AVR's serial bootloader and in the case of picocom means waiting for the bootloader to timeout. 

## RS-422 Management

The shield has a bus manager, though the example firmware is ongoing. It has access to enable/disable each transceiver receiver and/or driver. This means that each (or all) node(s) can be isolated from the serial bus (both host and/or MCU), the implications are significant. The original intent was to allow boot loading with a point to point full duplex mode (e.g. optiboot/xboot and avrdude need this). The reason Arduino Uno is so amazing is that it allows boot loading a new executable binary image even when the bare metal application is severely goofed up, I am trying to retain the nearly bulitproof upload. 

## Transceiver (RS-485)

The transceivers have a built-in fail-safe bias, which is a little complicated to explain, but it makes an undriven bus (e.g. the failed condition) a defined true or high state. That is if I turn off all the transceiver's driving the bus (only one should drive the bus at any given time), it is guaranteed to be in a defined state (e.g. the true state). I have setup the transceivers to automaticly turn off the driver while the UART output is a true (e.g. HIGH is its default value). That further means the driver will automatically drive the bus only when data is sent (e.g. drives when a zero or LOW bit is transmitted), so nothing needs to be done in software to turn off the bus. The bus manager can be controled over I2C to enable/disable the transceiver's receiver and/or driver (requires your firmware on the bus manager to do so, this is only hardware).

## SPI

The Pi Zero SPI lines (MOSI, SCK, MISO) are interfaced through a 74LVC07A buffer which is powered from the Pi's 3V3 power. When the Pi Zero is powered off the IOFF feature of the buffer will turn off (or hi-z) it's open collector output which will allow a pull-up (IOREF from the MCU board) to set the value on SCK and the MOSI line or allow the shield MCU to control them (though they have a pull up). 

Known issues: RPUno and Irrigate7 map the SPI lines to pluggable connectors for digital control so make sure those pluggable connectors are free if SPI is to be used.

## I2C

The Pi I2C port is not connected to the onboard bus manager. The Pi has 1.8k pull-ups to 3.3V, and when the shield powers down VIN it will pull down the I2C lines as the Pi can no longer sustain its 3.3V supply. 

A future update may include an ATmega328pb that has an extra I2C interface that could be used as an independent channel between the Pi Zero and the manager without locking up the MCU to the manager when the Pi Zero is powered down (Note: I am not working on this, it is just an idea with some of the hardware in place). 

## SD

Card [corruption] seems to happen when the SD card is doing wear leveling operations at the time power is removed. It may be possible with this setup to push the shutdown button and have that run a script that before halting tells the node to wait for a while and then make sure the current draw from the battery is stabilized before turning off VIN or disconnecting the battery. The idea is that wear leveling will draw current somewhat randomly until all the page updates are done. The BCM2835 will be halted, and looping (it is checking one of the I2C pins) and using a steady current draw. So when the current draw is stable the wear leveling should be done and safe to disconnect power.

[corruption]: http://hackaday.com/2016/08/03/single-board-revolution-preventing-flash-memory-corruption/

[Panasonic] has some robust SD cards available from Digi-key that should help when they are needed.

[Panasonic]: https://www.digikey.com/product-detail/en/panasonic-electronic-components/RP-SMTT32DA1/P122039-ND/6596411

## Agile

I'm not an Agile developer but when developing software or hardware the only thing that seems to get results is to make the first attempt at something, and then start fixing the broken stuff. Then add new stuff to push it in the desired direction. Keep iterating by fixing, and adding (and sometimes change direction when needed). It will never be truly finished, but at some point, things start to click and reveal if it is useful or useless (haha...  RPUpi has taken four iterations just to see the first bootload of an AVR from a Pi Zero over RS-422). 


## Security

Hardware with connectivity is easy to do presently but most of it ends up in the landfill quickly. The problem is that connectivity is like an exposed surface and is difficult to maintain. A device with RS-232 has a very limited connectivity surface. Only the serial interface can be used to control the device and it is connected to a computer (e.g. Pi Zero) that the user maintains. RS-422 connectivity is similar to RS-232 but the differential pairs can run over a 1km, and to multiple devices. 

Serial software is typically done to control a UART for RS-232 and does not know how to control the differential transceiver used for RS-422. This problem can be compensated for if the transceiver includes hardware for failsafe biasing, which basically means the differential lines have a defined state when not driven. When failsafe is used the transceiver can also enable its transmitter automatically when the UART goes LOW. That results in the ability of software done for RS-232 to also work over RS-422. Connecting multiple devices over RS-422 to a single computer reduces the users maintenance work load (which is one place all things IoT fail for me). 

Programming the devices on the serial interface is less about security and more about keeping things useful. I like how an AVR with a serial bootloader does this. It uploads programs that have full hardware control since they are an executable binary image compiled from C (or C++). This allows me direct access to the machine registers which maximizes what I can do with the controller. Understand that hardware of this nature has no safety guards since the uploaded machine code has full register level access. I would suggest only using boards with sufficient documentation and have the necessary hardware safety guards.

A Yun [worm].

[worm]: http://hackaday.com/2016/11/11/arduworm-a-malware-for-your-arduino-yun/

One lesson I see is to not expose the raw serial hardware on a network (i.e. minimize the exposed surface). I don't setup the Pi Zero to expose its serial interface to the network so the mistakes on my AVR software should not be a security problem. If I setup SSH on the Pi Zero wrong then I have a problem.


## Compiling

To prevent heap memory fragmentation on the AVR I use C and avoid the malloc functions. This means only the stack and static memory is used. Compiling with C++ can mix heap and stack memory usage in complicated ways I don't understand (and have had problems with). On the Pi, C++ is fine since Linux can manage the heap, but bare metal does nothing for heap memory management it just makes a fragmented mess.  Anyway, I tend to use Python on the Pi, so my C++ is not good enough to know when I am using the heap.

The AVR has a single clock domain (for the most part), so the GPIO, peripherals and flash memory operate in a predictable and repeatable way. That is to say, an AVR will pull an instruction from flash and use it to fiddle with an output in a predictable consistent way, however a fast machine may need to wait for instructions and will experience timing variations from the instruction cashing system as well as other clock domain transitions that cause timing variability in devices that run with multiple clock domains (e.g. MCU at higher speeds have to pre-fetch instructions from flash which runs in a slower clock domain, and write to GPIO in yet another slow clock domain).

When a task is small and/or simple enough to be done with an AVR it takes less time and effort to just use an AVR. It uses the same GCC toolchain as the big boys (GCC also does x86, ARM, MIPS...), but the AVR is a simple device with a fraction of the documentation to understand (and the garden ain't going to eat itself... or wait actually it sort of does).

# Hardware

## Overview

Eagle Files, BOM, Status, and how to Test.

![Schematic](https://raw.githubusercontent.com/epccs/RPUpi/master/Hardware/16197,Schematic.png "RPUpi Schematic")

# Notes

```
Full Duplex RS-422 multi-drop RX and TX pairs.
Half Duplx RS-485 out of band management (DTR) pair.
Fail Safe state (high) when differential line is undriven.
Resting state of differential pair (RX, TX and DTR) is undriven.
Fits RPUno, Irrigate7, and PulseDAQ boards.
May also fit other Arduino compatible boards.
USB soft start prevents brownout of other devices during hot plugging.
Power from USB is is given to +5V on the MCU node board.
Pi Zero UART connects to RX and TX pairs.
Pi Zero UART nRTS is sent to BUS manager.
I2C Interface between BUS manager and Pi Zero [and MCU node].  
BUS manager MCU type: ATmega328p
BUS manager MCU clock: 8MHz internal
BUS manager MCU Voltage: 3.3V (e.g. IOREF is 5V)
```

The manager can (with proper programming), send an out of band instruction to remote BUS manager(s) that will lockout unselected nodes to perform bootload on a selected node.
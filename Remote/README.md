# Remote

This firmware is for the bus manager on an RPUpi board, it will watch for a byte matching the local RPU_ADDRESS on the DTR pair and when seen reset the local controller placing it in bootloader mode. A non-matching byte will disconnect the RX and TX lines from the local controller and be in lockout mode until a LOCKOUT_DELAY completes or an RPU_NORMAL_MODE byte is seen on the DTR pair.

## Overview

In normal mode, the RX and TX lines (twisted pairs) are connected through transceivers to the controller board RX and TX pins while the DTR pair is connected to the bus manager UART and is used to set the system-wide bus state.

During lockout mode, the RX and TX serial lines are disconnected at the transceivers from the controller board RX and TX pins. Use LOCKOUT_DELAY to set the time in mSec.

Bootload mode occurs when a byte on the DTR pair matches the RPU_ADDRESS of the shield. It will cause a pulse on the reset pin to activate the bootloader on the local microcontroller board. After the bootloader is done, the local microcontroller will run its application, which, if it reads the RPU_ADDRESS will cause the manager to send an RPU_NORMAL_MODE byte on the DTR pair. The RPU_ADDRESS is read with a command from the controller board. If the RPU_ADDRESS is not read, the bus manager will timeout but not connect the RX and TX transceivers to the local controller board. Use BOOTLOADER_ACTIVE to set the time in mSec; it needs to be less than LOCKOUT_DELAY.

The lockout mode occurs when a byte on the DTR pair does not match the RPU_ADDRESS of the manager. It will cause the lockout condition and last for a duration determined by the LOCKOUT_DELAY or when an RPU_NORMAL_MODE byte is seen on the DTR pair.

When nRTS (or nDTR on RPUadpt) are pulled active the bus manager will connect the HOST_TX and HOST_RX lines to the RX and TX pairs, and pull the nCTS (and nDSR) lines active to let the host know it is Ok to send. If the bus is in use, the host will remain disconnected from the bus. Note the Remote firmware sets a status bit at startup that prevents the host from connecting until it is cleared with an I2C command.

Arduino Mode is a permanent bootload mode so that the Arduino IDE can connect to a specific address (e.g., point to point). It needs to be enabled by I2C or SMBus. The command will cause a byte to be sent on the DTR pair that sets the arduino_mode thus overriding the lockout timeout and the bootload timeout (e.g., lockout and bootload mode are everlasting).

Test Mode (Not Done, it is not yet even a work in progress; this is just a list of ideas). Disable: turn off all transceivers (even the DTR), allows the controller to measure input current. DtrEnable: turn on the DTR transceiver while the manager has a HIGH on its DTR_TXD line, enables the controller to measure input current. nDtrEnable: turn on the DTR transceiver while the manager has a LOW on its DTR_TXD line, allows the controller to measure input current. Rest is TBD.

## Firmware Upload

Use an ICSP tool connected to the bus manager (set the ISP_PORT in Makefile) run 'make' to compile then 'make isp' to flash the bus manager.

```
sudo apt-get install make git gcc-avr binutils-avr gdb-avr avr-libc avrdude
git clone https://github.com/epccs/RPUpi/
cd /RPUpi/Remote
make isp
...
avrdude done.  Thank you.
```

## Addressing

The Address '1' on the RPU_BUS is 0x31, (e.g., not 0x1 but the ASCII value for the character).

When HOST_nRTS is pulled active from a host trying to connect to the serial bus, the local bus manager will set localhost_active and send the bootloader_address over the DTR pair. If an address received by way of the DTR pair matches the local RPU_ADDRESS the bus manager will enter bootloader mode (marked with bootloader_started), and connect the shield RX/TX to the RS-422 (see connect_bootload_mode() function), all other addresses are locked out. After a LOCKOUT_DELAY time or when a normal mode byte is seen on the DTR pair, the lockout ends and normal mode resumes. The node that has bootloader_started broadcast the return to normal mode byte on the DTR pair when that node has the RPU_ADDRESS read from its bus manager over I2C (otherwise it will time out and not connect the controller RX/TX to serial).


## Bus Manager Modes

In Normal Mode, the RPU bus manager connects the local MCU node to the RPU bus if it is RPU aware (e.g., ask for RPU_ADDRESS over I2C). Otherwise, it will not attach the local MCU's TX to the bus but does connect RX. The host will be connected unless it is foreign.

In bootload mode, the RPU bus manager connects the local controller to the serial bus. Also, the host will be connected unless it is foreign. It is expected that all other nodes are in lockout mode. Note the BOOTLOADER_ACTIVE delay is less than the LOCKOUT_DELAY, but it needs to be in bootload mode long enough to allow uploading. A slow bootloader will require longer delays.

In lockout mode, if the host is foreign, both the local controller and Host are disconnected from the bus. Otherwise, the host remains connected.


## I2C and SMBus Slave

There are two TWI interfaces one acts as an I2C slave and is used to connect with the local microcontroller, while the other is an SMBus slave and connects with the local host (e.g., an R-Pi.) The commands sent are the same in both cases, but Linux does not like repeated starts or clock stretching so the SMBus read is done as a second bus transaction. I'm not sure the method is correct, but it seems to work, I echo back the previous transaction for an SMBus read. The masters sent (slave received) data is used to size the reply, so add a byte after the command for the manager to fill in with the reply. The I2C address is 0x29 (dec 41) and SMBus is 0x2A (dec 42). It is organized as an array of commands. 

0. read the shields RPU_BUS address and activate normal mode (broadcast if localhost_active).
1. set the shields RPU_BUS address and write it to EEPROM.
2. read the address sent when DTR/RTS toggles.
3. write the address that will be sent when DTR/RTS toggles
4. read RPUpi shutdown (the ICP1 pin has a weak pull-up and a momentary switch).
5. set RPUpi shutdown (pull down ICP1 for SHUTDOWN_TIME to cause the host to halt).
6. reads status bits [0:DTR readback timeout, 1:twi transmit fail, 2:DTR readback not match, 3:host lockout].
7. writes (or clears) status.

The controller has all commands available, but the HOST is limited to 0, 2, 3, 6, 7.

Connect to i2c-debug on an RPUno with an RPU shield using picocom (or ilk). 

``` 
picocom -b 38400 /dev/ttyUSB0
``` 


## RPU /w i2c-debug scan

Scan for the I2C0 slave address of shield and address.

``` 
picocom -b 38400 /dev/ttyUSB0
/1/id?
{"id":{"name":"I2Cdebug^1","desc":"RPUno (14140^9) Board /w atmega328p","avr-gcc":"5.4.0"}}
/1/iscan?
{"scan":[{"addr":"0x29"}]}
```


## Raspberry Pi scan the shield address

I2C1 slave port is for host access. A Raspberry Pi is set up as follows.

```
sudo apt-get install i2c-tools python3-smbus
sudo usermod -a -G i2c rsutherland
# logout for the change to take
i2cdetect 1
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-1.
I will probe address range 0x03-0x77.
Continue? [Y/n] Y
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- 2a -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
```


## RPU /w i2c-debug read the shield address

The local RPU address can be read.

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 0,255
{"txBuffer[2]":[{"data":"0x0"},{"data":"0xFF"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x0"},{"data":"0x31"}]}
``` 


## Raspberry Pi read the shield address

The host can read the local RPU_ADDRESS with the I2C1 port.

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
bus.write_i2c_block_data(42, 0, [255])
#read_i2c_block_data(I2C_ADDR, OFFSET, NUM_OF_BYTES)
# OFFSET is not implemented
print(bus.read_i2c_block_data(42, 0, 2))
[0, 49]
``` 


## RPU /w i2c-debug set RPU_BUS address

__Warning:__ this changes eeprom, flashing with ICSP does not clear eeprom due to fuse setting.

Using an RPUno and an RPUftdi shield, connect another RPUno with i2c-debug firmware to the RPUpi shield that needs its address set. The default RPU_BUS address can be changed from '1' to any other value. 

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 1,50
{"txBuffer[2]":[{"data":"0x1"},{"data":"0x32"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x1"},{"data":"0x32"}]}
``` 

The RPU program normally reads the address during setup in which case it needs a reset to get the update. The reset can be done by setting its bootload address (bellow).

```
/2/id?
{"id":{"name":"I2Cdebug^1","desc":"RPUno (14140^9) Board /w atmega328p","avr-gcc":"5.4.0"}}
``` 


## RPU /w i2c-debug read the address sent when DTR/RTS toggles

I2C0 can be used by the RPU to Read the local bootload address that it will send when a host connects to it.

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 2,255
{"txBuffer[2]":[{"data":"0x2"},{"data":"0xFF"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x2"},{"data":"0x30"}]}
``` 

Address 0x30 is ascii '0' so the RPU at that location will be reset and connected to the serial bus when a host connects to the shield at address '1'. 


## Raspberry Pi read the address sent when DTR/RTS toggles

I2C1 can be used by the host to Read the local bootload address that it will send when a host connects to it.

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
bus.write_i2c_block_data(42, 2, [255])
#read_i2c_block_data(I2C_ADDR, OFFSET, NUM_OF_BYTES)
#OFFSET is not implemented
print(bus.read_i2c_block_data(42, 0, 2))
[2, 48]
``` 

Address 48 is 0x30 or ascii '0' so the RPU at that location will be reset and connected to the serial bus when the Raspberry Pi connects to the shield at this location (use command 1 to find the local address).


## RPU /w i2c-debug set the bootload address

__Note__: this valuse is not saved in eeprom so a power loss will set it back to '0'.

I2C0 can be used by the RPU to set the local bootload address that it will send when a host connects to it. When DTR/RTS toggles send ('2' is 0x32 is 50).

```
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 3,50
{"txBuffer[2]":[{"data":"0x3"},{"data":"0x32"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x3"},{"data":"0x32"}]}
``` 

exit picocom with C-a, C-x. 

Connect with picocom again. 

``` 
picocom -b 38400 /dev/ttyUSB0
``` 

This will toggle RTS on the RPUpi shield and the manager will send 0x32 on the DTR pair. The RPUpi shield should blink slow to indicate a lockout, while the shield with address '2' blinks fast to indicate bootloader mode. The lockout timeout LOCKOUT_DELAY can be adjusted in firmware.


## Raspberry Pi set the bootload address

__Note__: this valuse is not saved in eeprom so a power loss will set it back to '0'.

I2C1 can be used by the host to set the local bootload address that it will send when a host connects to it. When DTR/RTS toggles send ('2' is 0x32 is 50).

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
bus.write_i2c_block_data(42, 3, [50])
#read_i2c_block_data(I2C_ADDR, OFFSET, NUM_OF_BYTES)
#OFFSET is not implemented
print(bus.read_i2c_block_data(42,0, 2))
[3, 50]
``` 


## RPU /w i2c-debug read if HOST shutdown detected

To check if host got a manual hault command (e.g. if the shutdown button got pressed) send the I2C shutdown command 4 (first byte), with place holder data (second byte). This clears shutdown_detected flag that was used to keep the LED_BUILTIN from blinking.

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 4,255
{"txBuffer[2]":[{"data":"0x4"},{"data":"0xFF"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x4"},{"data":"0x1"}]}
/1/ibuff 4,255
{"txBuffer[2]":[{"data":"0x4"},{"data":"0xFF"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x4"},{"data":"0x0"}]}
``` 

The above used a remote host and shield.

Second value in rxBuffer has shutdown_detected value 0x1, it is cleared after reading.


## RPU /w i2c-debug set HOST to shutdown

To hault the host send the I2C shutdown command 5 (first byte), with data 1 (second byte) which sets shutdown_started, clears shutdown_detected and pulls down the SHUTDOWN (ICP1) pin. The shutdown_started flag is also used to stop blinking of the LED_BUILTIN to reduce power usage noise so that the host power usage can be clearly seen.

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 5,1
{"txBuffer[2]":[{"data":"0x5"},{"data":"0x1"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x5"},{"data":"0x1"}]}
``` 

The above used a remote host and shield.


## RPU /w i2c-debug read status bits

I2C0 can be used by the RPU to read the local status bits.

0. DTR readback timeout
1. twi transmit fail 
2. DTR readback not match
3. host lockout

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 6,255
{"txBuffer[2]":[{"data":"0x6"},{"data":"0xFF"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x6"},{"data":"0x8"}]}
``` 


## Raspberry Pi read status bits

I2C1 can be used by the host to read the local status bits.

0. DTR readback timeout
1. twi transmit fail 
2. DTR readback not match
3. host lockout

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
bus.write_i2c_block_data(42, 6, [255])
#read_i2c_block_data(I2C_ADDR, OFFSET, NUM_OF_BYTES)
#OFFSET is not implemented
print(bus.read_i2c_block_data(42,0, 2))
[6, 8]
``` 

Bit 3 is set so the host can not connect until that has been cleared.


## RPU /w i2c-debug set status bits

I2C0 can be used by the RPU to set the local status bits.

0. DTR readback timeout
1. twi transmit fail 
2. DTR readback not match
3. host lockout

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 7,8
{"txBuffer[2]":[{"data":"0x7"},{"data":"0x0"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x7"},{"data":"0x0"}]}
``` 

## Raspberry Pi sets status bits

I2C1 can be used by the host to read the local status bits.

0. DTR readback timeout
1. twi transmit fail 
2. DTR readback not match
3. host lockout

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
bus.write_i2c_block_data(42, 7, [0])
#read_i2c_block_data(I2C_ADDR, OFFSET, NUM_OF_BYTES)
#OFFSET is not implemented
print(bus.read_i2c_block_data(42,0, 2))
[7, 0]
exit()
picocom -b 38400 /dev/ttyAMA0
...
Terminal ready
/1/id?
# C-a, C-x.
``` 

The Raspberry Pi can bootload a target on the RPU serial bus.


## Notes

If the program using a serial device (e.g., avrdude) gets sent a SIGINT (Ctrl+C) it may not leave the USB UART device or its driver in the proper state (e.g., the DTR/RTS may remain active). One way to clear this is to use modprobe to remove and reload the device driver. Hardware UART (e.g., /dev/ttyAMA0) does not have this issue.

``` 
# Serial restart (list device drivers)
lsmod | grep usbserial
# ftdi uses the ftdi_sio driver
# try to remove the driver
sudo modprobe -r ftdi_sio
# then load the driver again
sudo modprobe ftdi_sio
# the physcal UART chip will still have DTR/RTS active
# but picocom can open and release it now to clear the
# active lines.
picocom -b 38400 /dev/ttyUSB0
# C-a, C-x.
``` 

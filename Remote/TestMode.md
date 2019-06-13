# Test Mode Commands

192..254 (OxC0..0xFF | 0b11000000..0b11111111)

192. save trancever control bits 0:0:TX_nRE:TX_DE:DTR_nRE:DTR_DE:RX_nRE:RX_DE for test_mode.
193. recover trancever control bits after test_mode.
194. read trancever control bits durring test_mode bits, e.g. 0b00101010 is TX_nRE = 1, TX_DE =0, DTR_nRE =1, DTR_DE = 0, RX_nRE =1, RX_DE = 0.
195. set trancever control bits durring test_mode bits, e.g. 0b00101010 is TX_nRE = 1, TX_DE =0, DTR_nRE =1, DTR_DE = 0, RX_nRE =1, RX_DE = 0.


## Cmd 192 from a controller /w i2c-debug set transceiver test mode

Set test_mode, use the bootload port interface since the RPUbus transceivers are turned off.

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 192,1
{"txBuffer[2]":[{"data":"0xC0"},{"data":"0x01"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0xC0"},{"data":"0x01"}]}
``` 

also done on the second I2C channel that I buffer and use SMBus commands

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 42
{"address":"0x2A"}
/1/ibuff 192,1
{"txBuffer[2]":[{"data":"0xC1"},{"data":"0x01"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x0"},{"data":"0xFF"}]}
/1/ibuff 255
{"txBuffer[2]":[{"data":"0xFF"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0xC0"},{"data":"0xC0"}]}
```

The first read does a write followed by a repeated start and then read. SMBus does not do the repeated start, it does the write with the command and data, followed by another write with the command then switches to reading data. I am using a old buffer copy to fill the read from the previous write. I need to use a command byte that is not valid durring the read that way I don't have a side effect.

A copy of the command was put in data byte as a test.


## Cmd 192 from a Raspberry Pi set transceiver test mode

Set test_mode with an R-Pi over SMBus.

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
#read_i2c_block_data(I2C_ADDR, I2C_COMMAND, NUM_OF_BYTES)
# use command 255 during the read since a valid command can have undesired effects
# what is my address
bus.write_i2c_block_data(42, 0, [255])
print("'"+chr(bus.read_i2c_block_data(42, 255, 2)[1])+"'" )
'2'
# set the bootload address to my address
bus.write_i2c_block_data(42, 3, [ord('2')])
print("'"+chr(bus.read_i2c_block_data(42, 255, 2)[1])+"'" )
'2'
# clear the host lockout status bit so serial from this host can work
bus.write_i2c_block_data(42, 7, [0])
print(bus.read_i2c_block_data(42,0, 2))
[7, 0]
exit()
# on an RPUno load the blinkLED application which uses the UART
git clone https://github.com/epccs/RPUno
cd /RPUno/BlinkLED
make
make bootload
# now back to 
python3
import smbus
bus = smbus.SMBus(1)
# and set the test_mode
bus.write_i2c_block_data(42, 192, [1])
print(bus.read_i2c_block_data(42, 0, 2))
[64, 192]
``` 

I have the I2C function copy the command byte to the data byte, so it is correct within the I2C function that starts the test_mode. However, somewhere along the line, the command is getting corrupted and that is what gets sent back during the buffered (for SMbus) read. 

``` 
picocom -b 38400 /dev/ttyAMA0
a

``` 

Start another SSH session to set transceiver control bits (e.g. command 195) while using picocom to verify they are working.


## Cmd 193 from a controller /w i2c-debug recover after transceiver test

Recover trancever control bits after test_mode.

data returned is the recoverd trancever control bits: 0:0:TX_nRE:TX_DE:DTR_nRE:DTR_DE:RX_nRE:RX_DE

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 193,1
{"txBuffer[2]":[{"data":"0xC1"},{"data":"0x01"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0xC1"},{"data":"0x15"}]}
``` 

also done on the second I2C channel that I use SMBus with

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 42
{"address":"0x2A"}
/1/ibuff 193,1
{"txBuffer[2]":[{"data":"0xC1"},{"data":"0x01"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0xFD"},{"data":"0xFF"}]}
/1/ibuff 255
{"txBuffer[2]":[{"data":"0xFF"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0xC1"},{"data":"0x15"}]}
``` 

The command 0xFD is an error code caused by the last command (it was a read that had no data).

The second read is the old buffer from the first command, it has valid data 0x15 or 0b00010101. The trancever control bits are: 0:0:TX_nRE:TX_DE:DTR_nRE:DTR_DE:RX_nRE:RX_DE


## Cmd 193 from a Raspberry Pi recover after transceiver test

Set test_mode with an R-Pi over SMBus.

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
#read_i2c_block_data(I2C_ADDR, I2C_COMMAND, NUM_OF_BYTES)
# use command 255 during the read since a valid command can have undesired effects
# end the test_mode data byte is replaced with the recoverd trancever control
bus.write_i2c_block_data(42, 193, [255])
print(bus.read_i2c_block_data(42, 0, 2))
[65, 21]
bin(21)
'0b10101'
``` 

The trancever control bits are: 0:0:TX_nRE:TX_DE:DTR_nRE:DTR_DE:RX_nRE:RX_DE

So everything was enabled, it is sort of a stealth mode after power-up, and can allow a host to talk to new controllers on the bus if they don't toggle there RTS lines (e.g., the test case is an R-Pi on an RPUpi shield on an RPUno, all freshly powered.) Stealth mode ends when a byte is seen on the DTR pair, that is what will establish an accurate bus state, so stealth mode is an artifact of laziness after power up and may need to change. 

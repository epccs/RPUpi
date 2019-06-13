# Point To Point Commands

64..127 (Ox40..0x7F | 0b01000000..0b01111111)

64. set arduino_mode which is the number of times LOCKOUT_DELAY and BOOTLOADER_ACTIVE last forever when the host RTS toggles active (should allow Arduino IDE to make a number of p2p connections to the bootload address)
65. read arduino_mode

## Cmd 64 from a controller /w i2c-debug set p2p mode

set arduino_mode so LOCKOUT_DELAY and BOOTLOADER_ACTIVE last forever when the host RTS toggles active. Arduino IDE can then make p2p connections to the bootload address.

``` 
picocom -b 38400 /dev/ttyUSB0
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 64,1
{"txBuffer[2]":[{"data":"0x80"},{"data":"0x01"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x80"},{"data":"0x01"}]}
``` 

That was silly since i2c-debug will read the RPUbus address over I2C and cause the manager to send a normal mode (e.g., point to multi-point) on the DTR pair. 


## Cmd 64 from a Raspberry Pi set p2p mode

An R-Pi host can do it with SMBus, lets try with two RPUpi boards.

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
#read_i2c_block_data(I2C_ADDR, I2C_COMMAND, NUM_OF_BYTES)
# use command 255 during the read since a valid command can have undesired effects
# what is the bootload address
bus.write_i2c_block_data(42, 2, [255])
print("'"+chr(bus.read_i2c_block_data(42, 255, 2)[1])+"'" )
'0'
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
print(bus.read_i2c_block_data(42,255, 2))
[7, 0]
exit()
# on an RPUno load the blinkLED application which does not read the bus address
git clone https://github.com/epccs/RPUno
cd /RPUno/BlinkLED
make
make bootload
# now back to 
python3
import smbus
bus = smbus.SMBus(1)
# and set the arduino_mode
bus.write_i2c_block_data(42, 64, [1])
print(bus.read_i2c_block_data(42, 255, 2))
[64, 1]
# the R-Pi host can now connect by serial so LOCKOUT_DELAY and BOOTLOADER_ACTIVE last forever
``` 

Now the R-Pi can connect to serial in P2P mode.

``` 
picocom -b 38400 /dev/ttyAMA0
``` 

At this time, the point to point mode persists, I will sort out more details when they are needed.


## Cmd 65 from a Raspberry Pi read p2p mode

An R-Pi host can use SMBus to check for p2p mode.

```
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
#read_i2c_block_data(I2C_ADDR, I2C_COMMAND, NUM_OF_BYTES)
# use command 255 during the read since a valid command can have undesired effects
# what is the bootload address
bus.write_i2c_block_data(42, 65, [255])
print(bus.read_i2c_block_data(42, 255, 2))
[65, 0]
```
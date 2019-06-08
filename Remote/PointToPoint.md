# Point To Point Commands


## RPU /w i2c-debug read the shield address

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


## Raspberry Pi read the shield address

An R-Pi host can do it with SMBus, lets try with two RPUpi boards.

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
#read_i2c_block_data(I2C_ADDR, OFFSET, NUM_OF_BYTES)
# OFFSET is not implemented
# what is the bootload address
bus.write_i2c_block_data(42, 2, [255])
print("'"+chr(bus.read_i2c_block_data(42, 0, 2)[1])+"'" )
'0'
# what is my address
bus.write_i2c_block_data(42, 0, [255])
print("'"+chr(bus.read_i2c_block_data(42, 0, 2)[1])+"'" )
'2'
# set the bootload address to my address
bus.write_i2c_block_data(42, 3, [ord('2')])
print("'"+chr(bus.read_i2c_block_data(42, 0, 2)[1])+"'" )
'2'
# clear the host lockout status bit so serial from this host can work
bus.write_i2c_block_data(42, 7, [0])
print(bus.read_i2c_block_data(42,0, 2))
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
print(bus.read_i2c_block_data(42, 0, 2))
[64, 1]
# the R-Pi host can now connect by serial so LOCKOUT_DELAY and BOOTLOADER_ACTIVE last forever
``` 

Now the R-Pi can connect to serial in P2P mode.

``` 
picocom -b 38400 /dev/ttyAMA0
``` 

At this time, the point to point mode persists, I will sort out more details when they are needed.
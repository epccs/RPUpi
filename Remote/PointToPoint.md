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

The R-Pi host can do better with SMBus.

``` 
python3
import smbus
bus = smbus.SMBus(1)
#write_i2c_block_data(I2C_ADDR, I2C_COMMAND, DATA)
bus.write_i2c_block_data(42, 64, [1])
#read_i2c_block_data(I2C_ADDR, OFFSET, NUM_OF_BYTES)
# OFFSET is not implemented
print(bus.read_i2c_block_data(42, 0, 2))
[64, 1]
``` 

Now the R-Pi can connect to serial in P2P as long as the target does not read the RPUbus address from its manager over I2C.



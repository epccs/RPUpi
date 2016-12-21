# Description

Some lessons I learned doing RPUpi.

# Table Of Contents:

2. [ ^1 I2C](#1-i2c)
1. [ ^0 U3 Placed Backward](#0-u3-placed-backward)


## ^1 I2C

Cut I2C from Pi Zero and use a 1.8k pull-up to the bus manager 3V3. I am using I2C between the RPUno and bus manager to pass the rpu_bus address, I would prefer the RPUno programs work with an RPUpi shield without change. So the I2C from Pi to bus manager needs removed. Also when the RPUno kills power to the Pi that will pull down the I2C lines and block the bus manager from I2C communication with the RPUno.

![Cut I2C](./16197^1,CutI2cFromPiJmpMcuAndPullUp.jpg "Cut I2C From Pi")


## ^0 U3 Placed Backward

I though the via was my mark for pin one, need to fix the packages so this does not happen again.

![Via vs Pin Mark](./16197^0,U3placedBackward.jpg "Via vs Pin Mark")




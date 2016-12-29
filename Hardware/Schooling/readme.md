# Description

Some lessons I learned doing RPUpi.

# Table Of Contents:

4. [^1 Open SCK and MOSI](#1-open-sck-and-mosi)
3. [^1 Pi Tx Pull-Up](#1-pi-tx-pull-up)
2. [^1 I2C](#1-i2c)
1. [^0 U3 Placed Backward](#0-u3-placed-backward)


## ^1 Open SCK and MOSI

The 74LVC07 can damage the AVR by sinking current, therefore I need to open these traces and place a 10k Ohm resistor. This will allow the AVR to drive without causing damage when the 74LVC07's open drain output is sinking.

![Open SCK MOSI](./16197^1_OpenSckMosi.png "Open SCK MOSI")

Some nasty rework to add the 10k for more evaluation.

![10k ON SCK MOSI](./16197^1_10kOnSckMosi.jpg "10k ON SCK MOSI")

## ^1 Pi Tx Pull-Up

Add a 10k pull up to the Pi Zero Tx line. This will automatically disable the transceiver driver when the Pi is not powered or is absent.

![Pi Tx Pull Up Pads](./16197^1,PiTxPullUpPads.png "Pi Tx Pull Up Pads")
![Pi Tx With 10k Pull Up](./16197^1,PiTxWith10kPullUp.jpg "Pi Tx With 10k Pull Up")


## ^1 I2C

Cut I2C from Pi Zero and use a 1.8k pull-up to the bus manager 3V3. I am using I2C between the RPUno and bus manager to pass the rpu_bus address, I would prefer the RPUno programs work with an RPUpi shield without change. So the I2C from Pi to bus manager needs removed. Also when the RPUno kills power to the Pi that will pull down the I2C lines and block the bus manager from I2C communication with the RPUno.

![Cut I2C](./16197^1,CutI2cFromPiJmpMcuAndPullUp.jpg "Cut I2C From Pi")


## ^0 U3 Placed Backward

I though the via was my mark for pin one, need to fix the packages so this does not happen again.

![Via vs Pin Mark](./16197^0,U3placedBackward.jpg "Via vs Pin Mark")




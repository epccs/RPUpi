# Description

This is a list of Test preformed on each RPUpi after assembly.

# Table of References


# Table Of Contents:

1. Basics
2. Assembly check
3. IC Solder Test
4. Bias +5V and Check LDO Regulator
5. Set MCU Fuse
6. Load CheckDTR Firmware
7. Check Differential Bias
8. Differential Loopback with TX Driver
9. Differential Loopback with RX Driver
10. Bias VIN and Check +5V_2PI from SMPS
11. Host Lockout
12. Pi Zero without SD card
13. Boot Headless Pi Zero
14. SPI 


## Basics

These tests are for an assembled RPUpi board 16197^4 which may be referred to as a Unit Under Test (UUT). If the UUT fails and can be reworked then do so, otherwise it needs to be scraped. 

**Warning: never use a soldering iron to rework ceramic capacitors due to the thermal shock.**
    
Items used for test.

![ItemsUsedForTest](./16197_ItemsUsedForTest.jpg "RPUpi Items Used For Test")


## Assembly check

After assembly check the circuit carefully to make sure all parts are soldered and correct. The device marking is used as the part name on the schematic and assembly drawing. Check the bill of materials to figure out what the device is.


## IC Solder Test

Check continuity between pin and pad by measuring the reverse body diode drop from 0V (aka ground) and all other IC pads not connected to 0V. This value will vary somewhat depending on what the pin does, but there is typically an ESD diode to ground or sometimes a body diode (e.g. open drain MOSFET), thus a value of .4V to .7V is valid to indicate a solder connection. Note the RS485 drivers will show high impedance on the differential lines, so skip those.


## Bias +5V and Check LDO Regulator


Apply a 30mA current limited (CC mode) supply set at 5V to the +5V (J7 pin 4) and 0V (J7 pin 2) header pins. Check that the input current is for the blank MCU. Turn off power.

```
^4 removed LDO
{  "I_IN_BLANKMCU_mA":[5.2,] }
^3
{  "I_IN_BLANKMCU_mA":[2.1,1.8,2.2,2.2,],
    "LDO_V":[3.302,3.282,3.276,3.292,] }
```


## Set MCU Fuse

Install Git and AVR toolchain on Ubuntu (I use 18.04). 

```
sudo apt-get install git make gcc-avr binutils-avr gdb-avr avr-libc avrdude
```

Clone the RPUpi repository.

```
git clone https://github.com/epccs/RPUpi
cd RPUpi/Bootload
```

Connect a 5V supply with CC mode set at 30mA to the +5V (J7 pin 4) and  0V (J7 pin 2). Connect the ICSP tool (J9). The MCU needs its fuses set, so run the Makefile rule to do that. 

```
make fuse
```

Note: There is not a bootloader, it just sets fuses.

Disconnect the ICSP tool and measure the input current for 12Mhz crystal at 3.3V. It takes a long time to settle.

```
^4
{  "I_IN_MCU_12MHZ_LP-CRYSTAL_mA":[14.9,]}
^3
{  "I_IN_MCU_12MHZ_LP-CRYSTAL_mA":[4.9,5.1,5.0,5.1,]}
```


## Load CheckDTR Firmware

Plug a header (or jumper) onto the +5V pin so that IOREF is jumpered to +5V. Connect TX pin to IOREF to pull it up (the RPUno normaly does his). Plug a CAT5 RJ45 stub with 100 Ohm RX, TX and DTR pair terminations. Connect a 5V supply with CC mode set at 50mA to the +5V that was jumpered to IOREF (J8 pin 2) and  0V (J7 pin 2). Connect the ICSP tool (J9).

NOTE: IOREF J7 pin 4 is not connected so use J8 pin 2

Use the command line to select the CheckDTR source working directory. Run the makefile rule used to load CheckDTR firmware that verifies DTR control is working:

```
cd ~RPUpi/CheckDTR
make isp
```

The program loops through the test. It blinks the red LED to show which test number is setup. If it keeps repeating a test then that test has failed.

As the firmware loops, the input current can be measured, it should have two distinct levels, one when the DTR pair is driven low and one when the DTR pair is not driven. The blinking LED leaves the DMM unsettled. Turn off the power.

```

^3
{  "DTR_HLF_LD_mA":[33.4,33.8,33.6,33.7,],
    "DTR_NO_LD_mA":[10.0,10.5,10.5,10.5,] }
```

Note: the ICSP tool is pluged in and has some pullups with the level shift. 


## Check Differential Bias

Plug a header (or jumper) onto the +5V pin so that IOREF is jumpered to +5V. Plug a CAT5 RJ45 stub with 100 Ohm RX, TX and DTR pair terminations. Connect TX pin to 0V to pull it down to simulate the MCU sending data. Connect a 5V supply with CC mode set at 100mA to the +5V that was jumpered to IOREF (J8 pin 2) and 0V (J7 pin 2).

NOTE: IOREF J7 pin 4 is not connected 

Hold down the shutdown switch while running the CheckDTR firmware to set TX_DE and RX_DE high.

Check  that the input current is cycling between 56mA and 33mA. At 56mA the TX driver is driving the TX pair with half load and DTR driver is driving the DTR pair with a half load, while ony the TX pair is driven at 33mA. 

```
{  "DTR_TX_HLF_LD_mA":[56.0,56.9,57.0,56.8,],
    "TX_HLF_LD_mA":[33.0,33.7,33.7,33.7,] }
```


## Differential Loopback with TX Driver

Plug a header (or jumper) onto the +5V pin so that IOREF is jumpered to +5V. Plug a CAT5 RJ45 stub with 100 Ohm RX, TX and DTR pair terminations. Connect TX pin to 0V to pull it down to simulate the MCU sending data. Connect a 5V supply with CC mode set at 100mA to the +5V that was jumpered to IOREF (J8 pin 2) and 0V (J7 pin 2).

NOTE: IOREF J7 pin 4 is not connected

Connect a RJ45 loopback connector to allow the TX differential pair to drive RX differential pair and measure the input current. The TX driver is now driving 50 Ohms, which is the normal load. Verify that RX has 0V on it now.

```
{  "DTR_HLF_LD_TX_FL_LD_mA":[72.4,73.9,74.0,74.3,],
    "TX_FL_LD_mA":[49.1,50.7,50.9,51.0,] }
```

Turn off power.


## Differential Loopback with RX Driver

Continuing from previous test, now disconnect TX from ground and Connect it to IOREF, which will disable the TX driver (U2) so that the RX driver (U6) can operate through the RJ45 loopback. Keep the CAT5 RJ45 stub with 100 Ohm RX, TX and DTR pair terminations and the RJ45 loopback connector so the TX pair is looped back to the RX pair. 

Bias PI3V3 (J1 pin 1) with IOREF (The Pi will power this with 3.3V but it will work with the 5V on IOREF for testing).

Connect PI_TX (J1 pin 8) to 0V (J1 pin 6) to cause the RX driver to drive the RX pair. 

![ItemsUsedForTest9](./16197_ItemsUsedForTest[9].jpg "RPUpi Items Used For Test 9")

Power on 5V supply at 100mA. Hold down the shutdown switch while running the CheckDTR firmware to set TX_DE and RX_DE high.

Measure the supply current when RX is driven and when a DTR half load is added.

```
{  "DTR_HLF_LD_RX_FL_LD_mA":[83.3,88.0,88.3,88.0,],
    "RX_FL_LD_mA":[60.0,65.0,65.1,65.0,] }
```

Turn off power. Disconnect everything.


## Bias VIN and Check +5V_2PI from SMPS

Apply a 30mA current limited (CC mode) supply set at 12.8V to the VIN (J7 pin 1) and 0V (J7 pin 3) header pins. Measure the SMPS providing +5V_2PI (J1 pin 2). Check that the input current is for no load. Turn off power.

```
{  "VIN@NOLD_mA":[0.2,0.2,0.2,0.2,],
    +5V2PI_V":[4.97,4.99,5.00,5.00,] }
```

## Host Lockout

The Remote firmware will ignore the host nRTS signal until the status bit is cleared.

Plug an [RPUftdi] shield with [Host2Remote] firmware onto an [RPUno] or Uno board (not the UUT but a separate board) connect the USB to computer and load [I2C-Debug] on it. Note that the default bootload address allows this to work.

[RPUftdi]: https://github.com/epccs/RPUftdi
[Host2Remote]: https://github.com/epccs/RPUftdi/tree/master/Host2Remote
[RPUno]: https://github.com/epccs/RPUno
[I2C-Debug]: https://github.com/epccs/RPUno/tree/master/i2c-debug

```
cd ~RPUno/i2c-debug
make bootload
```

Use picocom to set the bootload address. The RPUftdi is at address 0x30 and the UUT will be at address 0x31.

```
picocom -b 38400 /dev/ttyUSB0
...
Terminal ready
/0/iaddr 41
{"address":"0x29"}
/0/ibuff 3,49
{"txBuffer":[{"data":"0x3"},{"data":"0x31"}]}
/0/iread? 2
{"rxBuffer":[{"data":"0x3"},{"data":"0x31"}]}
```

Exit picocom (Cntl^a and Cntl^x). 

Plug the UUT (the [RPUpi] shield) onto a second RPUno board. Connect the ICSP tool to UUT (J9). Power the RPUno (e.g. connect a 12V SLA battery and supply the PV input with 180mA CC and 20V). Load the Remote firmware onto UUT.

Use the command line to select the source working directory for the UUT bus manager firmware to use (i.e. [Remote]). Run the makefile rule used to load it:

[Remote]: https://github.com/epccs/RPUadpt/tree/master/Remote

```
cd ~RPUpi/Remote
make isp
```

The UUT RPU_ADDRESS defaults to 0x31 with that firmware, so the RPUno under the UUT will bootload when the computer opens the serial port on the RPUftdi. Now install [PwrMgt] on the RPUno under UUT. 

[PwrMgt]: https://github.com/epccs/RPUno/tree/master/PwrMgt

```
cd ~RPUno/PwrMgt
make bootload
```

Read the status byte from I2C with command 6, and check that it shows the host lockout bit 3 is set. A dumy byte (255) is put in the buffer to tell the slave to size the reply with one data byte.

```
picocom -b 38400 /dev/ttyUSB0
...
Terminal ready
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 6,255
{"txBuffer":[{"data":"0x6"},{"data":"0xFF"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x6"},{"data":"0x8"}]}
```

Measure the analog values. The charge data is accumulated from the time the RPUno started.

```
/1/analog? 2,3,6,7
{"CHRG_A":"0.060","DISCHRG_A":"0.000","PV_V":"20.02","PWR_V":"14.32"}
/1/charge?
{"CHRG_mAHr":"5.31","DCHRG_mAHr":"0.00","RMNG_mAHr":"0.00","ACCUM_Sec":"311.94"}
```

Turn off power to PV input.

```
/1/analog? 2,3,6,7
{"CHRG_A":"0.000","DISCHRG_A":"0.027","PV_V":"0.36","PWR_V":"13.94"}
/1/charge?
{"CHRG_mAHr":"8.55","DCHRG_mAHr":"0.21","RMNG_mAHr":"0.00","ACCUM_Sec":"551.38"}
Ctrl-a and Ctrl-x
```

Disconnect the Battery.

## Pi Zero without SD card

Plug in a Pi Zero without an SD card. Connect a battery to RPUno and power the PV input at 20V with CC 180mA.

The Pi Zero takes about 15mA for a second and then 23mA without an SD card. Verify the Pi's zero used for test has a 3V3 output.

Note: pi-bench is 3.309V,

Measure the analog values (from Ubuntu and RPUftid connection).

```
picocom -b 38400 /dev/ttyUSB0
...
Terminal ready
/1/analog? 2,3,6,7
{"CHRG_A":"0.092","DISCHRG_A":"0.000","PV_V":"19.87","PWR_V":"14.37"}
```

Turn off power to PV input.

```
/1/analog? 2,3,6,7
{"CHRG_A":"0.000","DISCHRG_A":"0.042","PV_V":"0.42","PWR_V":"13.58"}
```

Turn off the VIN pin

```
/1/vin DOWN
{"VIN":"CCSHUTDOWN"}
{"VIN":"I2CHAULT"}
{"VIN":"ATHAULTCURR"}
{"VIN":"DELAY"}
{"VIN":"WEARLEVELINGCLEAR"}
{"VIN":"DOWN"}`
/1/analog? 2,3,6,7
{"CHRG_A":"0.000","DISCHRG_A":"0.027","PV_V":"0.42","PWR_V":"13.41"}
```

The Pi Zero is using about 15mA from the battery (which is converted to 5V befor going to the Pi Zero).


## Boot Headless Pi Zero

Connect a 12V SLA AGM battery to an RPUno. Plug a Pi Zero into the RPUpi board. Plug an SD card setup with Raspbian and setup these headless [Linux] settings (e.g. pi-bench). Power the RPUno's PV input with a source at 180mA constant current and 20V. Wait for the green LED on the Pi Zero to be uninterrupted.

[Linux]: ./linux.md

```
ssh pi-bench.local
```

The [RTS/CTS] functions should enable if these [Linux] settins have been used (e.g. pi-bench).

[RTS/CTS]: https://github.com/epccs/RPUpi/tree/master/RPiRtsCts

```
sudo ./bin/rpirtscts on
Pi Zero Rev 1.3 with 40 pin GPIO header detected
Enabling CTS0 and RTS0 on GPIOs 16 and 17
```

Clear the lockout bit to alow the Pi Zero to use RS-422 as a host (e.g. from the Pi Zero in sneaky mode).

```
picocom -b 38400 /dev/ttyAMA0
...
Terminal ready
/1/iaddr 41
{"address":"0x29"}
/1/ibuff 3,49
{"txBuffer[2]":[{"data":"0x3"},{"data":"0x31"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x3"},{"data":"0x31"}]}
/1/ibuff 7,0
{"txBuffer[2]":[{"data":"0x7"},{"data":"0x0"}]}
/1/iread? 2
# RS-422 ASCII character glitch may show now since the RPUno has just been reset
```

After the UUT bus managers resets the RPUno it is mounted on wait for the RS-422 to start working (ending sheaky mode causes a reset that may show as an ASCII character), measure the analog values.

```
/1/analog? 2,3,6,7
{"CHRG_A":"0.088","DISCHRG_A":"0.000","PV_V":"16.91","PWR_V":"14.06"}
```

Turn off power to PV input.

```
/1/analog? 2,3,6,7
{"CHRG_A":"0.000","DISCHRG_A":"0.113","PV_V":"0.10","PWR_V":"13.79"}
{"CHRG_A":"0.000","DISCHRG_A":"0.096","PV_V":"0.36","PWR_V":"13.71"}
{"CHRG_A":"0.000","DISCHRG_A":"0.096","PV_V":"0.31","PWR_V":"13.60"}
{"CHRG_A":"0.000","DISCHRG_A":"0.190","PV_V":"0.39","PWR_V":"13.59"}
/1/charge?
{"CHRG_mAHr":"463.09","DCHRG_mAHr":"6.77","RMNG_mAHr":"0.00","ACCUM_Sec":"24562.52"}
{"CHRG_mAHr":"463.09","DCHRG_mAHr":"8.35","RMNG_mAHr":"0.00","ACCUM_Sec":"24622.52"}
{"CHRG_mAHr":"463.09","DCHRG_mAHr":"9.96","RMNG_mAHr":"0.00","ACCUM_Sec":"24682.54"}
```

exit picocom

```
Ctrl-a and Ctrl-x
Thanks for using picocom
```


## SPI

Now install [SpiSlv] on the RPUno under the UUT (note bus manager was setup to broadcast its own bootload address in the previous test).

[SpiSlv]: https://github.com/epccs/RPUno/tree/master/SpiSlv

```
cd ~RPUno/SpiSlv
make bootload
```

Enable the AVR SPI interface and exit picocom

```
picocom -b 38400 /dev/ttyAMA0
...
Terminal ready
/1/spi UP
{"SPI":"UP"}
Ctrl-a and Ctrl-x
Thanks for using picocom
```

Run spidev_test (see [SpiSlv]) on the Pi Zero:

``` 
./spidev_test -s 500000 -D /dev/spidev0.0

spi mode: 0
bits per word: 8
max speed: 500000 Hz (500 KHz)

00 FF FF FF FF FF
FF 40 00 00 00 00
95 FF FF FF FF FF
FF FF FF FF FF FF
FF FF FF FF FF FF
FF DE AD BE EF BA
AD F0

./spidev_test -s 500000 -D /dev/spidev0.0

spi mode: 0
bits per word: 8
max speed: 500000 Hz (500 KHz)

0D FF FF FF FF FF
FF 40 00 00 00 00
95 FF FF FF FF FF
FF FF FF FF FF FF
FF FF FF FF FF FF
FF DE AD BE EF BA
AD F0
``` 

Note the SPI output is offset a byte since it was sent back from the AVR. Close SSH.

```
exit
```

Press the shutdown button and wait for the green LED on the Pi Zero to be off uninterrupted. For referance another host (e.g. from a RPUftdi or another RPUpi) can take the RS-422 bus now and be used to restart this Pi Zero (it needs [PwrMgt] on the RPUno). 

```
picocom -b 38400 /dev/ttyUSB0
...
Terminal ready
/1/vin DOWN
{"VIN":"CCSHUTDOWN"}
{"VIN":"I2CHAULT"}
{"VIN":"ATHAULTCURR"}
{"VIN":"DELAY"}
{"VIN":"WEARLEVELINGCLEAR"}
{"VIN":"DOWN"}
/1/in
/1/vin UP
{"VIN":"UP"}
Ctrl-a and Ctrl-x
Thanks for using picocom
# wait for Pi led to be uninterrupted
ssh pi-bench.local
```

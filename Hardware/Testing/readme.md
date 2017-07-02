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
13. Boot Pi Zero
14. With Pi Zero and Wi-Fi


## Basics

These tests are for an assembled RPUpi board 16197^2 which may be referred to as a Unit Under Test (UUT). If the UUT fails and can be reworked then do so, otherwise it needs to be scraped. 

**Warning: never use a soldering iron to rework ceramic capacitors due to the thermal shock.**
    
Items used for test.

![ItemsUsedForTest](./16197_ItemsUsedForTest.jpg "RPUpi Items Used For Test")


## Assembly check

After assembly check the circuit carefully to make sure all parts are soldered and correct. The device marking is used as the part name on the schematic and assembly drawing. Check the bill of materials to figure out what the device is.


## IC Solder Test

Check continuity between pin and pad by measuring the reverse body diode drop from 0V (aka ground) and all other IC pads not connected to 0V. This value will vary somewhat depending on what the pin does, but there is typically an ESD diode to ground or sometimes a body diode (e.g. open drain MOSFET), thus a value of .4V to .7V is valid to indicate a solder connection. Note the RS485 drivers will show high impedance on the differential lines, so skip those.


## Bias +5V and Check LDO Regulator


Apply a 30mA current limited (CC mode) supply set at 5V to the +5V (J7 pin 4) and 0V (J7 pin 2) header pins. Check that the the linear regulator has 3.3V (J9 pin 2).  Check that the input current is for the blank MCU. Turn off power.

```
{  "I_IN_BLANKMCU_mA":[2.1,],
    "LDO_V":[3.302,] }
```


## Set MCU Fuse

Install Git and AVR toolchain on Ubuntu (16.04, on an old computer try https://wiki.ubuntu.com/Lubuntu). 

```
sudo apt-get install git gcc-avr binutils-avr gdb-avr avr-libc avrdude
```

Clone the RPUadpt repository.

```
cd ~
git clone https://github.com/epccs/RPUpi
cd ~/RPUpi/Bootload
```

Connect a 5V supply with CC mode set at 30mA to the +5V (J7 pin 4) and  0V (J7 pin 2). Connect the ICSP tool (J9). The MCU needs its fuses set, so run the Makefile rule to do that. 

```
make fuse
```

Note: There is not a bootloader, it just sets fuses.

Disconnect the ICSP tool and measure the input current for 12Mhz crystal at 3.3V. It takes a long time to settel.

```
{  "I_IN_MCU_12MHZ_LP-CRYSTAL_mA":[4.9,]}
```


## Load CheckDTR Firmware

Plug a header (or jumper) onto the +5V pin so that IOREF is jumpered to +5V. Connect TX pin to IOREF to pull it up (the RPUno normaly does this). Plug a CAT5 RJ45 stub with 100 Ohm RX, TX and DTR pair terminations. Connect a 5V supply with CC mode set at 50mA to the +5V that was jumpered to IOREF (J7 pin 4) and  0V (J7 pin 2). Connect the ICSP tool (J9).

Use the command line to select the CheckDTR source working directory. Run the makefile rule used to load CheckDTR firmware that verifies DTR control is working:

```
cd ~RPUpi/CheckDTR
make isp
```

The program loops through the test. It blinks the red LED to show which test number is setup. If it keeps repeating a test then that test has failed.

As the firmware loops the input current can be measured, it should have two distinct levels, one when the DTR pair is driven low and one when the DTR pair is not driven. The blinking LED leaves the DMM unsettled. Turn off power.

```
{  "DTR_HLF_LD_mA":[33.4,],
    "DTR_NO_LD_mA":[10.0,] }
```

Note: the ICSP tool is pluged in and has some pullups with the level shift. 


## Check Differential Bias

Plug a header (or jumper) onto the +5V pin so that IOREF is jumpered to +5V. Plug a CAT5 RJ45 stub with 100 Ohm RX, TX and DTR pair terminations. Connect TX pin to 0V to pull it down to simulate the MCU sending data. Connect a 5V supply with CC mode set at 100mA to the +5V that was jumpered to IOREF (J7 pin 4) and 0V (J7 pin 2).

Hold down the shutdown switch while running the CheckDTR firmware to set TX_DE and RX_DE high.

Check  that the input current is cycling between 56mA and 33mA. At 56mA the TX driver is driving the TX pair with half load and DTR driver is driving the DTR pair with a half load, while ony the TX pair is driven at 33mA. 

```
{  "DTR_TX_HLF_LD_mA":[56.0,],
    "TX_HLF_LD_mA":[33.0,] }
```


## Differential Loopback with TX Driver

Same as Differential Bias test with a plug in a RJ45 loopback connector to connect the TX differential pair to the RX differential pair and the input current. The TX driver is now driving a differential pair with 50 Ohms on it, which is the normal load. Verify that RX has 0V on it now.

```
{  "DTR_HLF_LD_TX_FL_LD_mA":[72.4,],
    "TX_FL_LD_mA":[49.1,] }
```

Turn off power.


## Differential Loopback with RX Driver

Continuing from previous test, now disconnect TX from ground and Connect it to IOREF, which will disable the TX driver (U2) so that the RX driver (U6) can operate through the RJ45 loopback. Keep the CAT5 RJ45 stub with 100 Ohm RX, TX and DTR pair terminations and the RJ45 loopback connector so the TX pair is looped back to the RX pair. 

Bias PI3V3 (J1 pin 1) with IOREF (The Pi will power this with 3.3V but it will work with the 5V on IOREF for testing).

Connect PI_TX (J1 pin 8) to 0V to cause the RX driver to drive the RX pair. 

![ItemsUsedForTest9](./16197_ItemsUsedForTest[9].jpg "RPUpi Items Used For Test 9")

Power on 5V supply at 100mA. Hold down the shutdown switch while running the CheckDTR firmware to set TX_DE and RX_DE high.

Measure the supply current when RX is driven and when a DTR half load is added.

```
{  "DTR_HLF_LD_RX_FL_LD_mA":[83.3,],
    "RX_FL_LD_mA":[60.0,] }
```

Disconnect the PI3V3 (J1 pin 1) bias. Disconnect the PI_TX also. Turn off power.


## Bias VIN and Check +5V_2PI from SMPS

Apply a 30mA current limited (CC mode) supply set at 12.8V to the VIN (J7 pin 1) and 0V (J7 pin 3) header pins. Measure the SMPS providing +5V_2PI (J1 pin 2). Check that the input current is for no load. Turn off power.

```
{  "VIN@NOLD_mA":[0.2,],
    +5V2PI_V":[4.97,] }
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

NOTE for ^3, this is when ^2 was was found to not have power on U3, so the MCU UART on RPUno was not able to talk to the RPUftdi UART (a hack was done to power U3 see schooling for more info).

Connect a battery to RPUno and power the PV input at 20V with CC 180mA. Measure the analog values.

```
/1/analog? 2,3,6,7
{"CHRG_A":"0.027","DISCHRG_A":"0.000","PV_V":"19.98","PWR_V":"13.54"}
/1/charge?
{"CHRG_mAHr":"0.62","DCHRG_mAHr":"0.00","RMNG_mAHr":"0.00","ACCUM_Sec":"90.14"}
```

Turn off power to PV input.

```
/1/analog? 2,3,6,7
{"CHRG_A":"0.001","DISCHRG_A":"0.019","PV_V":"0.39","PWR_V":"13.48"}
/1/charge?
{"CHRG_mAHr":"0.95","DCHRG_mAHr":"0.08","RMNG_mAHr":"0.00","ACCUM_Sec":"163.58"}
```

Disconnect the Battery.

## Pi Zero without SD card

Plug in a Pi Zero without an SD card. Connect a battery to RPUno and power the PV input at 20V with CC 180mA.

Measure the analog values.


    
NOTE: The Pi Zero takes about 15mA for a second and then 23mA without an SD card. Verify the Pi's 3V3 output.

```
{ "I50115_V":[4.96,],
  "FrstSecVIN_mA":[15.2,],
  "WithoutSDcardVIN_mA":[22.6,],
  "Pi3V3_V":[3.28,]}
```


## Boot Pi Zero

Clear the lockout bit to alow the Pi Zero to use RS-422 as a host.

```
/1/buffer 7,0
/1/read? 2
```


Apply a 12.4V current limited source at about 250mA to VIN (J7 pin 1) and 0V (J7 pin 3). Use SD card with shutdown-sw.py script. Watch VIN current durring bootup. Wait for idling current then press the shutdown switch and measure VIN current after shutdown.

```
        TODO:  some data from the unit(s)
            { "MaxAtBoot_mA":[83.0,],
               "IdlingAftrBoot_mA":[40,],
               "ShutdownAftrIdling_mA":[10.6,]}
```

## With Pi Zero and Wi-Fi

Use SD card with [network] setup and measure VIN current with and without power management enabled. Note my testing shows the WiFi drops the network after a day or so with power managment enabled. 
    
[network]: ./linux.md

```
        TODO:  some data from the unit(s)
            { "MaxWthWiFiAtBoot_mA":[122,],
              "IdlingWthWiFiAftrBoot_mA":[63,],
              "IdlingWthWiFiNoPwrMgtAfterBoot_mA":[85,]
              "ShutdownWthWiFiAftrIdling_mA":[11,]}
```

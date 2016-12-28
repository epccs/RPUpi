# Description

This is a list of Test preformed on each RPUpi after assembly.

# Table of References


# Table Of Contents:

1. [Basics](#basics)
2. [Assembly check](#assembly-check)
3. [IC Solder Test](#ic-solder-test)
4. [Bias 3V3](#bias-3v3)
5. [Bias +5V and Check LDO Regulator](#bias-5v-and-check-ldo-regulator)
6. [Set MCU Fuse](#set-mcu-fuse)
7. [Load CheckDTR Firmware](#load-checkdtr-firmware)
8. [Check Differential Bias](#check-differential-bias)
9. [Differential Loopback with TX Driver](#differential-loopback-with-tx-driver)
10. [Differential Loopback with RX Driver](#differential-loopback-with-rx-driver)
11. [Load Lockout Firmware](#load-lockout-firmware)
12. [Pi Zero without SD card](#pi-zero-without-sd-card)
13. [Boot Pi Zero](#boot-pi-zero)
14. [With Pi Zero and Wi-Fi](#with-pi-zero-and-wi-fi)


## Basics

These tests are for an assembled RPUno board 16197^0 which may be referred to as a Unit Under Test (UUT). If the UUT fails and can be reworked then do so, otherwise it needs to be scraped. 

**Warning: never use a soldering iron to rework ceramic capacitors due to the thermal shock.**
    
Items used for test.

![ItemsUsedForTest](./16197_ItemsUsedForTest.jpg "RPUpi Items Used For Test")


## Assembly check

After assembly check the circuit carefully to make sure all parts are soldered and correct. The device marking is used as the part name on the schematic and assembly drawing.  In other words, the name KB33 (U6 on ^1) is actually the device marking. Check the bill of materials to figure out what the device is, which shows that it is a Micrel MIC5205-3.3YM5.


## IC Solder Test

Check continuity between pin and pad by measuring the reverse body diode drop from 0V (aka ground) and all other IC pads not connected to 0V. This value will vary somewhat depending on what the pin does, but there is typically an ESD diode to ground or sometimes a body diode (e.g. open drain MOSFET), thus a value of .4V to .7V is valid to indicate a solder connection. Note the RS485 drivers will show high impedance on the differential lines, so skip those.


## Bias 3V3

Apply a 3V3 current limited source at about 20mA to the ICSP VCC (J10 pin 2) and ICSP 0V (J10 pin 6). Check that the current drawn is about 5 mA.

```
        Data from unit(s):
            { "I_IN_BLANKMCU_mA":[2.0,] }
```


## Bias +5V and Check LDO Regulator

NOTE: ^0 did not have a LDO it used 3V3 from the Pi (this has changed in ^1). 

Apply a 5V current limited source (about 20mA) to the +5V (J8 pin 4) and 0V (J8 pin 2). Check that the the linear regulator has 3.3V (use ICSP J10 pin2).  Check that the input current for the blank MCU is less than 5mA. Turn off power.

NOTE: Startup with the least power that is reasonable to limit damage 
should something be shorted.

```
        Data from unit(s):
            { "I_IN_PI_mA":[2.2,],
               "LDO_V":[3.30,] }
```


## Set MCU Fuse

Apply a 5V current limited source at about 50mA to the +5V (J8 pin 4) and 0V (J8 pin 2). The MCU needs its fuses set, run "make fuse" to do that. Remove the ICSP tool to read input current.

Use the <https://github.com/epccs/RPUpi/tree/master/Bootload> Makefile

Note: there is not a bootloader, it just sets fuses.

```
        TODO:  some data from unit(s)
            { "I_IN_MCU_8MHZ_INTRN_mA":[4.0,]}
```


## Load CheckDTR Firmware

If termination resistors are not placed connect a CAT5 cable with the 100 Ohm termination resistors. Apply a 5V current limited source at about 50mA to the +5V (J8 pin 4) and 0V (J8 pin 2). Connect the TX (J4 pin 4) to 5V to pull it up (the MCU normaly does this). Connect IOREF (J9 pin 2) to 5V. Load  CheckDTR firmware with "make isp" to verify DTR control is working:

Use the <https://github.com/epccs/RPUpi/tree/master/CheckDTR> Makefile

The program loops through the test. It blinks the LED to show which test number is setup. If it keeps repeating a test then that test has failed.

As the firmware loops the input current can be measured, it should have two distinct levels, one when the DTR pair is driven low with a 100 Ohm termination (e.g. half loaded) and one when the DTR pair is not driven. The blinking LED leaves the DMM unsettled. Turn off power.

```
        TODO:  some data from the unit(s)
            { "DTR_HLF_LD_mA":[36.9,],
               "DTR_NO_LD_mA":[13.8,] }
```


## Check Differential Bias

If termination resistors are not placed connect a CAT5 cable with the 100 Ohm termination resistors. Apply a 5V current limited source at about 100mA to the +5V (J8 pin 4) and 0V (J8 pin 2). Connect the TX (J4 pin 4) to 5V to pull it up (the MCU normaly does this). Connect IOREF (J9 pin 2) to 5V. Hold down the shutdown switch while running the CheckDTR firmware to set TX_DE and RX_DE high. Verify that RX (J4 pin 3) has 5V and TX (J4 pin 4) has 5V. Check that TX pair transceiver driver input has 3.3V (U1 pin 4) and its driver enable (U1 pin 3) is pulled low while TX_DE is high. Check that HOST_TX has 3.3V. Check that RX transceiver enable (U6 pin 3) is pulled low while RX_DE is high. 
    
Now disconnect TX from IOREF and connect it to 0V, to simulate the MCU sending data. Check  that the input current is now cycling between 59mA and 37mA. At 59mA the TX driver is driving the TX pair with half load and DTR driver is driving the DTR pair with a half load, while ony the TX pair is driven at 37mA. Turn off power.

```
        TODO:  some data from the unit(s)
            { "DTR_TX_HLF_LD_mA":[59.9,],
               "TX_HLF_LD_mA":[36.7,] }
```


## Differential Loopback with TX Driver

If termination resistors are not placed connect a CAT5 cable with the 100 Ohm termination resistors. Apply a 5V current limited source at about 100mA to the +5V (J8 pin 4) and 0V (J8 pin 2). Connect the TX (J4 pin 4) to 0V to pull it down. Connect IOREF (J9 pin 2) to 5V. Plug in a RJ45 loopback connector to connect the TX differential pair to the RX differential pair and the input current. Hold down the shutdown switch while running the CheckDTR firmware to set TX_DE and RX_DE high. The TX driver is now driving a differential pair with 50 Ohms on it, which is the normal load. Verify that RX has 0V on it now. Turn off power.

```
        TODO:  some data from the unit(s)
            { "DTR_HLF_LD_TX_FL_LD_mA":[77.2,],
               "TX_FL_LD_mA":[54.1,] }
```


## Differential Loopback with RX Driver

If termination resistors are not placed connect a CAT5 cable with the 100 Ohm termination resistors. Apply a 5V current limited source at about 100mA to the +5V (J8 pin 4) and 0V (J8 pin 2). Connect the TX (J4 pin 4) to 5V to pull it up, which will disable the TX driver (U2) so that the RX driver (U6) can operate through the RJ45 loopback. Plug in the RJ45 loopback connector so the TX pair is looped back to the RX pair. Connect HOST_TX (J1 pin 8) to 0V (J1 pin 6) to cause the RX driver to drive the RX pair (which is also looped into the TX pair). Hold down the shutdown switch while running the CheckDTR firmware to set TX_DE and RX_DE high. Measure supply current after test four when RX is driven with 50 Ohm and DTR is driven with 100 Ohm. Also measure supply current after test two when RX has 50 Ohm and DTR is not driven. Power off and remove connections.

```
        TODO:  some data from the unit(s)
            { "DTR_HLF_LD_RX_FL_LD_mA":[77.3,],
               "RX_FL_LD_mA":[54.2,] }
```


## Load Lockout Firmware

Apply a 5V current limited source at about 100mA to the +5V (J8 pin 4) and 0V (J8 pin 2). Upload the Lockout firmware to allow connecting with Pi Zero serial port: 

Use the <https://github.com/epccs/RPUpi/tree/master/Lockout> Makefile 
        
This firmware turns off TX_nRE (and everything else) so that a serial port consol my be connected to the Pi Zero without interferance from the RPU_BUS. It also blinks the LED_BUILTIN until the Pi Shutdown switch is pressed, and bias the switch with a weak pull-up.


## Pi Zero without SD card

Apply a 5V current limited source at about 150mA to the +5V (J8 pin 4) and 0V (J8 pin 2). Apply a 12.4V current limited source at about 250mA to VIN (J8 pin 1) and 0V (J8 pin 3). Measure U4 (OKI-78SR-5) output (5V) going to Pi Zero and the 3.3V from the Pi's onboard SMPS. Measure the VIN current.
    
NOTE: The Pi Zero takes about 15mA for a second and then 23mA without an SD card. Verify the Pi's 3V3 output.

```
        TODO:  some data from the unit(s)
            { "I50115_V":[4.96,],
               "FrstSecVIN_mA":[15.2,],
               "WithoutSDcardVIN_mA":[22.6,],
               "Pi3V3_V":[3.28,]}
```


## Boot Pi Zero

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

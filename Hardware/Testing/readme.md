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

Apply a 3V3 current limited source at about 20mA to the ICSP VCC (J9 pin 2) and ICSP 0V (J9 pin 6). Check that the current drawn is about 5 mA.

```
        TODO:  some data from unit(s)
            { "I_IN_BLANKMCU_mA":[4.6,] }
```


## Bias +5V and Check LDO Regulator

Apply a current limited (&lt;20mA) supply set with 14V to the +BAT and -BAT connector and verify that voltage does not get through to TP4. 


## Set MCU Fuse

Apply a 3V3 current limited source at about 50mA to the ICSP VCC (J9 pin 2) and ICSP 0V (J9 pin 6). The MCU needs its fuses set, run "make fuse" to do that.

Use the <https://github.com/epccs/RPUpi/tree/master/Bootload> Makefile

Note: there is not a bootloader, it just sets fuses.

```
        TODO:  some data from unit(s)
            { "I_IN_MCU_8MHZ_INTRN_mA":[8.2,]}
```


## Load CheckDTR Firmware

Apply a 3V3 current limited source at about 50mA to the ICSP VCC (J9 pin 2) and ICSP 0V (J9 pin 6). Jumper MCU's SPI VCC (J8 pin2) to the 3.3V also. Use CheckDTR firmware to verify DTR control is working:

Use the <https://github.com/epccs/RPUpi/tree/master/CheckDTR> Makefile

The program loops through the test. It blinks the LED to show which test number is setup. If it keeps repeating a test then that test has failed.
    
Note: if termination resistors are not placed connect a CAT5 cable with the termination resistors.

As the firmware loops the input current can be measured, it should have two distinct levels, one when the DTR pair is driven low with a 100 Ohm termination (e.g. half loaded) and one when the DTR pair is not driven. The blinking LED leaves the DMM unsettled. 

```
        TODO:  some data from the unit(s)
            { "DTR_HLF_LD_mA":[36.5,],
               "DTR_NO_LD_mA":[14.0,] }
```


## Check Differential Bias

Apply a 3.3V current limited source at about 100mA to the management ICSP VCC (J9 pin 2) and ICSP 0V (J9 pin 6). Jumper MCU's SPI VCC (J8 pin2) to the 3.3V also. Connect the TX (J3 pin 4) to 3.3V to pull it up (the MCU normaly does this). Connect HOST_TX (U1 pin 8) to 3.3V (U1 pin 1). Hold down the shutdown switch while running the CheckDTR firmware to set TX_DE and RX_DE high. Verify that both RX (J3 pin3) and TX (J3 pin 4) have 3.3V. Check TX transceiver driver input has 3.3V (U2 pin 4) and its driver enable (U2 pin 3) is pulled low while TX_DE is high. Check that HOST_TX has 3.3V (on ^0 a jumper is needed, go up six lines). Check that RX transceiver enable (U6 pin 3) is pulled low while RX_DE is high. 
    
Now disconnect TX from IOREF and connect it to 0V, to simulate the MCU sending data. Check  that the input current is now cycling between 59mA and 37mA. At 59mA the TX driver is driving the TX pair with half load and DTR driver is driving the DTR pair with a half load, while ony the TX pair is driven at 37mA. 

```
        TODO:  some data from the unit(s)
            { "DTR_TX_HLF_LD_mA":[58.7,],
               "TX_HLF_LD_mA":[36.6,] }
```


## Differential Loopback with TX Driver

Plug in a RJ45 loopback connector to connect the TX differential pair to the RX differential pair and the input current. The TX driver is now driving a differential pair with 50 Ohms on it, which is the normal load. Verify that RX has 0V on it now. Turn off power.

```
        TODO:  some data from the unit(s)
            { "DTR_HLF_LD_TX_FL_LD_mA":[74.4,],
               "TX_FL_LD_mA":[52.0,] }
```


## Differential Loopback with RX Driver

Disconnect TX from ground and Connect it to IOREF, which will disable the TX driver (U2) so that the RX driver (U6) can operate through the RJ45 loopback. Plug in the RJ45 loopback connector so the TX pair is looped back to the RX pair. Connect HOST_TX (U1 pin 8) to 0V (U1 pin 6) to cause the RX driver to drive the RX pair (which is also looped into the TX pair). Measure the supply current for when RX is driven with 50 Ohm and DTR driven with 100 Ohm. Also measure when RX has 50 Ohm and DTR is not driven.

```
        TODO:  some data from the unit(s)
            { "DTR_HLF_LD_RX_FL_LD_mA":[74.7,],
               "RX_FL_LD_mA":[52.4,] }
```

Power off and remove all connections.


## Load Lockout Firmware

Apply a 3V3 current limited source at about 100mA to the ICSP VCC (J9 pin 2) and ICSP 0V (J9 pin 6). Upload the Lockout firmware to allow connecting with Pi Zero serial port: 

Use the <https://github.com/epccs/RPUpi/tree/master/Lockout> Makefile 
        
This firmware turns off TX_nRE (and everything else) so that a serial port consol my be connected to the Pi Zero without interferance from the RPU_BUS. It also blinks the LED_BUILTIN until the Pi Shutdown switch is pressed, and bias the switch with a weak pull-up.


## Pi Zero without SD card

Apply a 12.4V current limited source at about 250mA to VIN (J7 pin 1) and 0V (J7 pin 3). Measure U5 (OKI-78SR-5) output (5V) going to Pi Zero and the 3.3V from the Pi's onboard SMPS. Measure the VIN current.
    
NOTE: The Pi Zero takes about 11.3mA for a second and then 23.4mA without an SD card. Verify the Pi's 3V3 output.

```
        TODO:  some data from the unit(s)
            { "I50115_V":[4.99,],
               "FrstSecVIN_mA":[11.3,],
               "WithoutSDcardVIN_mA":[23.4,],
               "Pi3V3_V":[3.29,]}
```


## Boot Pi Zero

Apply a 12.4V current limited source at about 250mA to VIN (J7 pin 1) and 0V (J7 pin 3). Use SD card with shutdown-sw.py script. Watch VIN current durring bootup. Wait for idling current then press the shutdown switch and measure VIN current after shutdown.

```
        TODO:  some data from the unit(s)
            { "MaxVINatBoot_mA":[85.0,],
               "IdlingVIN_mA":[42,],
               "ShutdownVIN_mA":[13.5,]}
```

## With Pi Zero and Wi-Fi

Use SD card with [network] setup and measure VIN current with and without power management enabled. Note my testing shows the WiFi drops the network after a day or so with power managment enabled. 
    
[network]: ./linux.md

```
        TODO:  some data from the unit(s)
            { "MaxWiFiVINatBoot_mA":[128.0,],
               "IdlingWiFiVIN_mA":[65,],
               "IdlingWiFiNoPwrManagementVIN_mA":[85,]}
```

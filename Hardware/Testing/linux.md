# Description

This is a list of notes regarding Linux things for RPUpi.


# Table Of Contents:

1. [Prepare SD Card](#prepare-sd-card)
2. [Headless Setup](#headless-setup)
3. [Network Setup](#network-setup)
4. [SSH](#ssh)
5. [Samba](#samba)
6. [Python 3](#python-3)
7. [Serial](#serial)
8. [Packages used for the AVR toolchain](#packages-used-for-the-avr-toolchain)
9. [Avrdude](#avrdude)
10. [GPIO, I2C, and SPI](#gpio-i2c-and-spi)
11. [WiFi Dropout](#wiFi-dropout)
12. [Package Updates](#package-updates)


## Prepare SD Card

Download the latest [Raspbin] image.
        
[Raspbin]: https://www.raspberrypi.org/downloads/raspbian/

Download [Etcher] (note, I have only used this on Ubuntu)
        
[Etcher]: https://etcher.io/
        
On Ubuntu 17.04 I needed to install libgconf-2-4 to allow etcher to run. Etcher (in the zip) is an application image, it does not install anything it just needs premision to run.

```
sudo apt-get install libgconf-2-4
...


## Headless Setup

After [Etcher] has put the Raspbian image onto the SD card I mount it to the Ubuntu system. The easiest way to do this is just to unplug the card and plug it back in. It will have a boot area and the system folders (e.g. /etc, /home...) which we can change.

First configure the WiFi (e.g. Edit the /etc/network/interfaces) for your network, I show some of my Network Setup bellow.

Next add an empty ssh file to the boot area. I do this with the touch command from the  Ubuntu computer I used to setup the SD card.

```
touch ssh
```

On my network I can then ssh pi@raspberrypi.local with the default password "raspberry", I then change the password as well as the hostname.

```
ssh pi@raspberrypi.local

user: pi
password: raspberry

# a comment (like in bash or python) so you know typing it is a waste of effort.
# change password
passwd

# check for updates
# last year rpi-update was faster than apt-get update
# but on 8/9/17 it was not very fast and I fear it is the wrong way to get updates
# resync packages
sudo apt-get update
# install the newest version of packages
sudo apt-get upgrade

# Use this tool to setup (e.g. set the hostname: pi-bench, pi3, console login). 
sudo raspi-config

# I put the same username on Windows and other Linux machines for use with ssh, Samba and ilk.
# it makes life a littel less painful
sudo adduser rsutherland

# add myself to the sudoers list (bottom)
# see https://www.raspberrypi.org/documentation/linux/usage/users.md
sudo visudo
rsutherland  ALL=(ALL) NOPASSWD: ALL

# always shutdown befor turning off the power
sudo shutdown -h now
```

After a hault the Pi starts to reboot, but early in the GPU stages it starts to monitor BCM3 for a low which when seen will cause it to continue booting and [wake] up. Note that BCM3 is an I2C line and has a 1.8k pull-up. 

[wake]: https://www.raspberrypi.org/forums/viewtopic.php?p=733677#p733677

Add [Shutdown] also.

[Shutdown]: ../../Shutdown


## Network setup 

```
# scan, if you want to see other networks to connect
sudo iwlist wlan0 scan

# My WyFi Authentication Method is WPA2-Personal with AES type WEP encryption
# I edit the SD card from another machine e.g. using nano
sudo nano /etc/wpa_supplicant/wpa_supplicant.conf
```

Edit the /etc/wpa_supplicant/wpa_supplicant.conf file with the following.

```
#country=GB
#ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
#update_config=1

# bellow are the settings I used
country=US
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
 ssid="EPCCS2"
 psk="yourkeynotmynetworkey"
 key_mgmt=WPA-PSK
}
```

Now restart the network

```
sudo ifdown wlan0
sudo ifup wlan0

# the avahi-daemon (installed by default) allows computers on the LAN to be found 
# by hostname (e.g. Zeroconf or Bonjour, Ubuntu 16.04 has it, but it does not work on 14.04 for me)
#sudo apt-get install avahi-daemon
#sudo insserv avahi-daemon
# see https://learn.adafruit.com/bonjour-zeroconf-networking-for-windows-and-linux

# also see WiFi Dropout (bellow) after getting updates
```


## SSH

```
#check (it has to be installed by default or the headless install will not work)
dpkg -l openssh-server
openssh-server          1:6.7p1-5+deb8u2 armhf 

# check the config (make a backup befor changing anything, but I see nil to change)
# sudo cp /etc/ssh/sshd_config /etc/ssh/sshd_config.factory-defaults
# sudo chmod a-w /etc/ssh/sshd_config.factory-defaults
sudo nano /etc/ssh/sshd_config

# put some ssh keys on the machine, it can make login much less painfull.
# I have a bash scrip to do some of the grunt work
[mkdir bin]
cd bin
wget https://raw.githubusercontent.com/epccs/RPUpi/master/Hardware/Testing/mkeys
chmod u+x mkeys
# note if you have a private key (e.g. id_dsa or id_rsa file) and you want to use it then place it in the .ssh folder now
~/bin/mkeys localhost
# that should have built the public (and if missing a new private) key and added the public key to the authorized file 
# now try to log in (it should not ask for a password)
ssh localhost
# if that works one of the putty tools can convert the private key for use on Windows.
# mkeys can also place the public key on the authorized file of other Linux machines, e.g. conversion is a 
# Ubuntu 16.04 and it's zeroconf seems to work (try with a ping first).
~/bin/mkeys conversion.local
ssh conversion.local
```


## Samba

Samba is for windows file sharing

```
sudo apt-get update
sudo apt-get install samba samba-common-bin
sudo smbpasswd -a rsutherland

#Folder to share
mkdir /home/rsutherland/Samba

sudo nano /etc/samba/smb.conf
```

Add to file after the comment, the workgroup does not need changed.

```
#===== Global Settings ==========
workgroup = WORKGROUP

# add this to the very end of the file
[Samba]
path = /home/rsutherland/Samba
valid users = rsutherland
read only = no
```

Restart Samba and check for errors.

```
sudo service smbd restart

#Check for errors
testparm

# my user name (rsutherland) on Windows can map 
# to the share on the Pi Zero (computer name is raspberrypi)
\\raspberrypi\Samba
```
Note the Pi also shows the user home folders so it has a setting that Ubuntu did not... do I care... no.


## Python 3

On Raspbian Lite only Python 2 is installed but I will use Python 3, so it needs added.

```
sudo apt-get install python3
```

## Serial 

I want to use the serial port to interface over my RS-422 bus. So, select Interfacing Options => Serial and check that it is not used for a login.

```
ssh pi-bench.local
sudo raspi-config
```

Disable the boot console (raspi-config does this).

```
# Pi Zero
sudo systemctl stop serial-getty@ttyAMA0.service
sudo systemctl disable serial-getty@ttyAMA0.service
# Pi Zero W
sudo systemctl stop serial-getty@ttyS0.service
sudo systemctl disable serial-getty@ttyS0.service
```

Remove the console from /boot/cmdline.txt (raspi-config does this), which looks like:

```
dwc_otg.lpm_enable=0 console=serial0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline fsck.repair=yes root wait
```

Change it to:

```
dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline fsck.repair=yes root wait
```

I want to use /dev/ttyAMA0 since I think it has nRTS and nCTS functions. So I need to disable bluetooth.

```
# Pi Zero
# has no bluetooth
# Pi Zero W
systemctl disable bluetooth.service
```

The UART needs enabled for the kernel. E.g. set enable_uart in /boot/config.txt then reboot (raspi-config seems to have done this).

```
dtoverlay=pi3-disable-bt
enable_uart=1
```

Turn on RST/CTS using a program built with the Pi toolchain (is there another way?)

```
sudo apt-get install git
# I like to put my git repos in a samba share
[mkdir Samba]
[cd Samba]
git clone git://github.com/epccs/RPUpi.git
cd RPUpi/RPiRtsCts
make
cp rpirtscts ~/bin/rpirtscts
make clean
cd ~
chmod ugo+x ~/bin/rpirtscts
sudo ./bin/rpirtscts on
```

I use picocom to connect with the serial devices. Make sure the user (rsutherland is for myself) is in the dialout group.

```
sudo apt-get install picocom
sudo usermod -a -G dialout rsutherland
# restart the ssh login to gain the new group 
picocom -b 38400 /dev/ttyAMA0
```


## Packages used for the AVR toolchain

```
sudo apt-get install gcc-avr
sudo apt-get install binutils-avr
sudo apt-get install gdb-avr
sudo apt-get install avr-libc
sudo apt-get install avrdude
```

## Avrdude

Avrdude is a programming tool for AVR microcontrollers, it is used to place the binary image into the controller and set a verity of hardware options (fuses). Let's look at the RPUno files to see how it works. Adc is an interactive command line program that works over the RS-422 serial (which is the purpose of RPUpi) linking the Pi UART to the AVR UART, it shows a way to read analog channels from the ATmega328p on the RPUno from the Pi. 

```
[cd Samba]
git clone git://github.com/epccs/RPUno.git
```

Generally, avrdude is setup to run with rules in a Makefile. The following is what the rules for the [Adc] firmware Makefile need to do.

[Adc]: https://github.com/epccs/RPUno/tree/master/Adc

```
avrdude -v -p atmega328p -c arduino -P /dev/ttyAMA0 -b 115200 -U flash:w:Adc.hex
```

The BOOT_PORT needs to change from /dev/ttyUSB0 to /dev/ttyAMA0. Probably the easy thing to do is add a new rule to bootload from a Pi host (TBD).

The option -c arduino (programmer-id) means an optiboot programmer is going to be the target. I have put the optiboot bootloader on the ATmega328p in RPUno.

Remember to enable CTS/RST befor using Avrdude.

```
sudo ./bin/rpirtscts on
Pi Zero Rev 1.3 with 40 pin GPIO header detected
Enabling CTS0 and RTS0 on GPIOs 16 and 17
```


## GPIO, I2C, and SPI

<https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup>

Why not use the GPIO of the Pi Zero for things like flow meter pulse timing. Explaining such things is complicated, Ken Shirriff provides some references to the GPIO abstraction available on BeagleBone (it is similar to the Pi). Ken goes a little deeper than most, so this may help explain why some things need to be done in metal rather than trying to fit them into a time division multiplexing system.

<http://hackaday.com/2016/08/19/ken-shirriff-demystifies-beaglebone-io/>
<http://www.righto.com/2016/08/the-beaglebones-io-pins-inside-software.html>

The Sitara has some bare metal processors on the chip to provide a place for code that needs continuous loops (e.g. not time slices). This is good but the Sitara metal processors (PRU) are not so easy to use like an AVR and there is compelling reasons to believe that big complex chips like the Sitara are more likely to suffer CMOS latch-up which would then cascade into the PRU. 

One of my areas of interest is event capture (e.g. eCAP on BBB) but all evidence shows it is not supported with any operating system. The Sitara may be programed as a bare metal device to access the enhanced capture module, but then I still need an SBC running Linux. If Linux could be setup to ignore the eCAP hardware so the PRU can control it then serious questions about security would be raised. Linux needs to use the MMU to protect the systems memory space, but if the PRU can access that space it is the same as unprotected.


## WiFi Dropout

I am using a Realtek <https://www.adafruit.com/products/814>

Check if the rtl8188cus driver has minimum power management enabled (e.g. returns a 1) 

```
cat /sys/module/8192cu/parameters/rtw_power_mgnt
```

Change (or add) the config file so it will be off when booted

```
sudo nano /etc/modprobe.d/8192cu.conf
```

edit the file to look like

```
# Disable power saving
options 8192cu rtw_power_mgnt=0 rtw_enusbss=1 rtw_ips_mode=1 
```

restart

```
sudo shutdown -r now
```


## Package Updates

```
# resynchronize
sudo apt-get update
# upgrade + handles dependencies
sudo apt-get dist-upgrade
# remove stale dependencies
sudo apt-get autoremove
sudo apt-get clean
```

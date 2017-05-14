# Description

This is a list of notes regarding Linux things for RPUpi.


# Table Of Contents:

1. [Prepare SD Card](#prepare-sd-card)
2. [Boot With Serial Consol](#boot-with-serial-consol)
3. [Network Setup](#network-setup)
4. [SSH](#ssh)
5. [Samba](#samba)
6. [Serial](#serial)
7. [Packages used for the AVR toolchain](#packages-used-for-the-avr-toolchain)
8. [Avrdude](#avrdude)
9. [GPIO, I2C, and SPI](#gpio-i2c-and-spi)
10. [WiFi Dropout](#wiFi-dropout)
11. [Package Updates](#package-updates)


## Prepare SD Card

Download the latest [Raspbin] image.
        
[Raspbin]: https://www.raspberrypi.org/downloads/raspbian/

Download [Fedora ARM Installer] (Windows and Fedora)
        
[Fedora ARM Installer]: http://fedoraproject.org/wiki/Fedora_ARM_Installer
        
Use the Fedora ARM Installer to put the Raspbin image onto a microSD card.
        
[Win32DiskImager] can also read or write an SD card image.
        
[Win32DiskImager]: https://sourceforge.net/projects/win32diskimager/
        
On Linux use dd (change /dev/sdx to that of your SD card device)

```
            # read the image
            dd if=/dev/sdx of=/path/to/image.img bs=1M
            # write the image
            dd if=/path/to/image.img of=/dev/sdx
```


## Boot With Serial Consol

```
user: pi
password: raspberry

# a comment (like in bash or python) so you know typing it is a waste of effort.
# change password
passwd

# check for updates (setup network first)
# this is much faster than apt-get update
sudo rpi-update

# See if this tool has anything you want to setup. 
sudo raspi-config

# hostname has the name of this machine, and hosts is used for routing on a network (e.g. static networks)
# raspi-config can also do this (some name(s) I use: pi-bench)
sudo nano /etc/hostname
sudo nano /etc/hosts

# I put the same username on Windows and other Linux machines for use with ssh, Samba and ilk.
# it makes life a littel less painful
sudo adduser rsutherland

# add myself to the sudoers list (bottom)
# see https://www.raspberrypi.org/documentation/linux/usage/users.md
sudo visudo
&gt;rsutherland  ALL=(ALL) NOPASSWD: ALL

# always shutdown befor turning off the power
sudo shutdown -h now
```

After a hault the Pi starts to reboot, but early in the GPU stages it starts to monitor BCM3 for a low which when seen will cause it to continue booting and [wake] up. Note that BCM3 is an I2C line and has a 1.8k pull-up. 

[wake]: https://www.raspberrypi.org/forums/viewtopic.php?p=733677#p733677


## Network setup 

```
# scan, if you see your network it should connect
sudo iwlist wlan0 scan

# My WyFi Authentication Method is WPA2-Personal with AES type WEP encryption
# so I edit as the following
sudo nano /etc/wpa_supplicant/wpa_supplicant.conf
```

Edit the file with the following.

```
#country=GB
#ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
#update_config=1

# bellow are the settings I used (note I was not setting GROUP, it was set to root on the next line)
country=US
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
#ctrl_interface_group=0
update_config=1

network={
 ssid="EPCCS"
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

# some versions just for a referance point
uname -a
Linux raspberrypi 4.4.11+ #888 Mon May 23 20:02:58 BST 2016 armv6l GNU/Linux
python3
Python 3.4.2 (default, Oct 19 2014, 13:31:11)
python
Python 2.7.9 (default, Mar  8 2015, 00:52:26)

# after my latest update I have
uname -a
Linux pi-bench 4.4.23+ #913 Tue Oct 4 13:56:56 BST 2016 armv6l GNU/Linux
```


## SSH

```
#check (it is installed by default)
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
wget https://github.com/epccs/RPUpi/blob/master/Hardware/Testing/mkeys
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


## Serial 

To use the serial port select Interfacing Options => Serial and stop using it for a login (Pi needs another true UART).

```
sudo raspi-config
```

The UART also needs enabled for the kernel. E.g. set enable_uart in /boot/config.txt then reboot.

```
enable_uart=1
```

I use picocom to connect with the serial devices. Make sure the user (rsutherland is for myself) is in the dialout group.

```
sudo apt-get install picocom
sudo usermod -a -G dialout rsutherland
# restart the ssh login to gain the new group 
picocom -b 115200 /dev/ttyAMA0
```


## Packages used for the AVR toolchain

```
# versions are for referance (today is 7/22/16)
# gcc-avr (1:4.8.1+Atmel3.4.4-2)
sudo apt-get install gcc-avr
# binutils-avr (2.24+Atmel3.4.4-1)
sudo apt-get install binutils-avr
# gdb-avr (7.7-2)
sudo apt-get install gdb-avr
# avr-libc (1:1.8.0+Atmel3.4.4-1)
sudo apt-get install avr-libc
# avrdude (6.1-2+rpi1)
sudo apt-get install avrdude

# next I put my RPUno Mercurial repo into the Samba share and was 
# able to ssh in and build the BlinkLED makefile just like on Ubuntu... nice. 
```

## Avrdude

```
    TBD
    -c arduino -P /dev/ttyAMA0 -b 115200 is used for avr with obitboot.  The kernel uses the port for debug spew (fix TBD).
    
    -c pi_1  Need to setup a -C config file for bitbang mode see:
        https://learn.adafruit.com/program-an-avr-or-arduino-using-raspberry-pi-gpio-pins/configuration
        
    -c linuxspi is another option that may work with the spi interface, 
        use -b 200000 to slow down some report errors at the default.
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

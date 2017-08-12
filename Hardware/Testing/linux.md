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
```


## Headless Setup

After [Etcher] has put the Raspbian image onto the SD card I mount it to the Ubuntu system. The easiest way to do this is just to unplug the card and plug it back in. It will have a boot area and the system folders (e.g. /etc, /home...) which we can change.

First configure the WiFi (e.g. Edit the /etc/wpa_supplicant/wpa_supplicant.conf) for your network, I show some of my [Network Setup](#network-setup) bellow.

Next add an empty ssh file to the boot area. I do this with the touch command from a consol on the Ubuntu computer I used to setup the SD card.

```
# on Ubuntu 17.04 the SD card automaticly mounts when pluged in at /media/username
cd /media/rsutherland
ls
# it shows: 673b8ab6-6426-474b-87d3-71bff0fcebc3  boot
# for the card I just did Ubuntu called the root mount 673b8ab6-6426-474b-87d3-71bff0fcebc3
# so to setup the Wi-Fi I can edit
sudo nano /media/rsutherland/673b8ab6-6426-474b-87d3-71bff0fcebc3/etc/wpa_supplicant/wpa_supplicant.conf
# to tell the startup system to run the SSH server I add an empty file to the boot mount (which has its expected name)  
touch /media/rsutherland/boot/ssh
```

Put it in the Pi and boot... On my network I can then ssh pi@raspberrypi.local with the default password "raspberry", I then change the password as well as the hostname.

```
ssh pi@raspberrypi.local

user: pi
password: raspberry

# change password
passwd

# Use the raspi-config tool to setup e.g. set the hostname: pi1, pi-bench, pi3.
# set boot options: choose to boot into a CLI (e.g. text console) for headless systems (mount SD on Ubuntu to change network)
# also turn off the login shell to the serial port, and enable the serial port hardware (e.g. /dev/ttyAMA0).
sudo raspi-config

# I put the same username on Windows and other Linux machines for use with ssh, Samba and ilk.
# it makes life a littel less painful
sudo adduser rsutherland

# add myself to the sudoers list (bottom)
# see https://www.raspberrypi.org/documentation/linux/usage/users.md
sudo visudo
rsutherland  ALL=(ALL) NOPASSWD: ALL

# check for updates
# last year rpi-update was faster than apt-get update
# but on 8/9/17 rpi-update was not very fast and I fear it is the wrong way to get updates
# resync packages
sudo apt-get update
# install the newest version of packages
sudo apt-get upgrade

# restart, I like to have a chance to close ssh and see that the system is going down for a reboot.
sudo shutdown -r 1
# output: Shutdown scheduled for Sat 2017-08-12 04:28:25 UTC, use 'shutdown -c' to cancel.
# output: Broadcast message from root@pi-bench (Sat 2017-08-12 04:27:25 UTC):
# output: The system is going down for reboot at Sat 2017-08-12 04:28:25 UTC!
exit
logout
Connection to pi-bench.local closed.
# back to the system I started from
```

After reboot add some scripts for RPUpi [Shutdown] and [RPiRtsCts].

[Shutdown]: ../../Shutdown
[RPiRtsCts]: ../../RPiRtsCts

```
ssh pi-bench.local

# since I am loging in from rsutherland on another machine ssh will try to use that user name.
mkdir bin
mkdir Samba
sudo apt-get install git
cd Samba
git clone git://github.com/epccs/RPUpi.git
cd RPUpi/RPiRtsCts
make
cp rpirtscts ~/bin/rpirtscts
make clean
cd ~
chmod ugo+x ~/bin/rpirtscts
cd ~/bin
wget https://raw.githubusercontent.com/epccs/RPUpi/master/Shutdown/shutdown-sw.py
chmod ugo+x shutdown-sw.py
sudo nano /etc/rc.local
```

Follow these rc.local [recommendations].

[recommendations]: https://www.raspberrypi.org/documentation/linux/usage/rc-local.md

```
python /home/rsutherland/bin/shutdown-sw.py &
/home/rsutherland/bin/rpirtscts on &
exit 0
```

Always shutdown befor turning off the power. 

```
sudo shutdown -h now
```

After a hault the Pi starts to reboot, but early in the GPU stages it starts to monitor BCM3 for a low which when seen will cause it to continue booting and [wake] up. Note that BCM3 is an I2C line and has a 1.8k pull-up. 

[wake]: https://www.raspberrypi.org/forums/viewtopic.php?p=733677#p733677


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

Samba is for Windows file sharing.

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

# my user name (rsutherland) on Windows can now map 
# to the share on the Pi Zero (computer name is pi-bench)
\\pi-bench\Samba
```
Note the Pi also shows the user home folders so it has a setting that Ubuntu did not, I will ignor it.


## Python 3

On Raspbian Lite only Python 2 is installed but I will use Python 3, so it needs added.

```
sudo apt-get install python3
```

## Serial 

Use raspi-config to turn off the login shell to the serial port, and enable the serial port hardware (e.g. /dev/ttyAMA0).

```
ssh pi-bench.local
sudo raspi-config
```

Disable the boot console (raspi-config does this now).

```
# Pi Zero
sudo systemctl stop serial-getty@ttyAMA0.service
sudo systemctl disable serial-getty@ttyAMA0.service
# Pi Zero W
sudo systemctl stop serial-getty@ttyS0.service
sudo systemctl disable serial-getty@ttyS0.service
```

Remove the console from /boot/cmdline.txt (raspi-config does this now), which looks like:

```
dwc_otg.lpm_enable=0 console=serial0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline fsck.repair=yes root wait
```

Change it to:

```
dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline fsck.repair=yes root wait
```

I want to use /dev/ttyAMA0 since it has nRTS and nCTS functions. On a Pi Zero W the bluetooth needs to be disabled.

```
# Pi Zero
# has no bluetooth
# Pi Zero W
systemctl disable bluetooth.service
```

The UART needs enabled for the kernel. E.g. set enable_uart in /boot/config.txt then reboot (I don't have a Pi Zero W yet, does raspi-config do this?).

```
dtoverlay=pi3-disable-bt
enable_uart=1
```

RST/CTS should be setup with the [RPiRtsCts] pogram built with the Pi toolchain durring [Headless Setup](#headless-setup) (is there another way?)

```
sudo ./bin/rpirtscts on
```

Use picocom to connect with the serial devices. Make sure the user (rsutherland is for myself) is in the dialout group.

```
sudo apt-get install picocom
sudo usermod -a -G dialout rsutherland
# restart the ssh login to gain the new group 
picocom -b 38400 /dev/ttyAMA0
```

If the RPUpi bus manger is loaded with [Remote] firmware, it needs to send a valid bootload address or there is a long timeout (30 second). 

[Remote]: https://github.com/epccs/RPUadpt/tree/master/Remote

On the controller board under the RPUpi, I am working on [PwrMgt] firmware. 

[PwrMgt]: https://github.com/epccs/RPUno/tree/master/PwrMgt

```
/1/id?
{"id":{"name":"PwrMgt","desc":"RPUno Board /w atmega328p and LT3652","avr-gcc":"4.9"}}
/1/iaddr 41
{"address":"0x29"}
# i2c command 3 will set the bootload address that is sent when DTR/RTS toggles on the bus manager with Remote firmware
/1/ibuff 3,49
{"txBuffer[2]":[{"data":"0x3"},{"data":"0x31"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x3"},{"data":"0x31"}]}
# i2c command 7 will clear the lockout bit on the bus manager with Remote firmware
/1/ibuff 7,0
{"txBuffer[2]":[{"data":"0x7"},{"data":"0x0"}]}
/1/iread? 2
# this will cause a the bus manage to see the nRTS active and start a bootload cycle.
# after the delay I can use the RS-422 again.
```


## Packages used for the AVR toolchain

```
sudo apt-get install gcc-avr binutils-avr gdb-avr avr-libc avrdude
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

Remember to enable CTS/RST befor using Avrdude see the [Headless Setup](#headless-setup).

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

One of my areas of interest is event capture (e.g. eCAP on BBB) but all evidence shows it is not supported with any operating system. The Sitara may be programed as a bare metal device to access the enhanced capture module, but then I still need an SBC running Linux. If Linux could be setup to ignore the eCAP hardware so the PRU can control it then serious questions about security would be raised. Linux needs to use the Sitara MMU to protect the systems memory space, but if the PRU can access that space it is the same as unprotected.


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

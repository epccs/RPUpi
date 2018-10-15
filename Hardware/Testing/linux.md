# Description

This is a list of notes regarding Raspbin (Linux) things for RPUpi.


# Table Of Contents:

1. Prepare SD Card
1. Headless Setup
1. Network Setup
1. SSH
1. Samba
1. Web Server
1. Python 3
1. Serial
1. Packages used for the AVR toolchain
1. SPI and I2C
1. WiFi Dropout
1. Package Updates
1. Editor


## Prepare SD Card

Download the latest [Raspbin] image.
        
[Raspbin]: https://www.raspberrypi.org/downloads/raspbian/

Download [Etcher] (note, I have only used this on Ubuntu)
        
[Etcher]: https://etcher.io/
        
Using Ubuntu 18.04 (on 17.04 I installed libgconf-2-4 to allow etcher to run). Etcher is an application image so it does not install anything it just needs permission to run.

```
sudo apt-get install libgconf-2-4
```

To update Etcher I just delete the old application image.


## Headless Setup

After [Etcher] has put the Raspbian image onto the SD card I mount it to the Ubuntu system. The easiest way to do this is just to unplug the card and plug it back in. It will have a boot partition and a system partition (e.g. /etc, /home...) which we can change.

First configure the WiFi (e.g. Edit the /etc/wpa_supplicant/wpa_supplicant.conf) for your network, I show some of my [Network Setup](#network-setup) bellow.

Next add an empty ssh file to the boot area. I do this with the touch command from a console on the Ubuntu computer I used to setup the SD card.

```
# on Ubuntu 18.04 the SD card automaticly mounts when pluged in at /media/username
cd /media/rsutherland
ls
# stretch mounts: rootfs boot
# To setup the Wi-Fi I can edit wpa_supplicant (see network-setup) in rootfs
sudo nano /media/rsutherland/rootfs/etc/wpa_supplicant/wpa_supplicant.conf
# to tell the startup system to run the SSH server I add an empty file in the boot mount
sudo touch /media/rsutherland/boot/ssh
```

Put it in the Pi and boot... On my network I can then ssh pi@raspberrypi.local with the default password "raspberry", I then change the password as well as the hostname.

```
# Well fish, I need to remove the old host and IP from the past setup 
[ssh-keygen -f "/home/rsutherland/.ssh/known_hosts" -R raspberrypi.local]
[ssh-keygen -f "/home/rsutherland/.ssh/known_hosts" -R 192.168.0.21]
[ssh-keygen -f "/home/rsutherland/.ssh/known_hosts" -R 192.168.0.22]
[ssh-keygen -f "/home/rsutherland/.ssh/known_hosts" -R pi1.local]
[ssh-keygen -f "/home/rsutherland/.ssh/known_hosts" -R 192.168.4.5]

ssh pi@raspberrypi.local

user: pi
password: raspberry

# change password
passwd

# Use the raspi-config tool to setup e.g. set the hostname: pi1, pi-bench, pi3.
# set Boot Options: choose to boot into a CLI (e.g. text console) for headless systems (mount SD on Ubuntu to change network)
# set local en_US.UTF-8
# set Interfaceing Options: Serial: turn off the login shell to the serial port, and enable the serial port hardware (e.g. /dev/ttyAMA0).
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
Connection to pi1.local closed.
# back to the system I started from
```

After reboot add some scripts for RPUpi [Shutdown] and [RPiRtsCts].

[Shutdown]: https://github.com/epccs/RPUpi/tree/master/Shutdown
[RPiRtsCts]: https://github.com/epccs/RPUpi/tree/master/RPiRtsCts

```
ssh pi1.local

# since I am loging in from rsutherland on another machine ssh will try to use that user name.
mkdir bin
mkdir Samba
sudo apt-get install git
cd Samba
mkdir git
cd git
git clone https://github.com/epccs/RPUpi
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
sudo shutdown -h 1
```

After a hault the Pi starts to reboot, but early in the cycle it starts to monitor BCM3 for a low which when seen will cause it to continue booting and [wake] up. Note that BCM3 is an I2C line and has a 1.8k pull-up. 

[wake]: https://www.raspberrypi.org/forums/viewtopic.php?p=733677#p733677


## Network setup 

It is your computer and your network, this is just an example. See [Wireless CLI Setup] for more.

[Wireless CLI Setup]: https://www.raspberrypi.org/documentation/configuration/wireless/wireless-cli.md

```
# scan, if you want to see other networks to connect
sudo iwlist wlan0 scan

# My WyFi Authentication Method is WPA2-Personal with AES type WEP encryption
# I edit the SD card from another machine e.g. using nano
sudo nano /media/rsutherland/rootfs/etc/wpa_supplicant/wpa_supplicant.conf
```

On Ubuntu the SD card mounts at /media/username. This is how I want to edit the /etc/wpa_supplicant/wpa_supplicant.conf file.

```
#country=GB
#ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
#update_config=1

# bellow are the settings I used
#country=US
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
 ssid="EPCCS4"
 psk="yourkeynotmynetworkey"
 key_mgmt=WPA-PSK
 priority=1
}

network={
 ssid="epccs3"
 psk="yourkeynotmynetworkey"
 key_mgmt=WPA-PSK
 priority=2
}

network={
 ssid="EPCCS2"
 psk="yourkeynotmynetworkey"
 key_mgmt=WPA-PSK
 priority=3
}
```

Now restart the network

```
# ifdown will kill the ssh session so make sure to start the network before issuing the command.
sudo ifdown wlan0; sudo ifup wlan0

# the avahi-daemon (installed by default) allows computers on the LAN to be found 
# by hostname (e.g. Zeroconf or Bonjour, Ubuntu 16.04 has it, but it does not work on 14.04 for me)
#sudo apt-get install avahi-daemon
#sudo insserv avahi-daemon
# see https://learn.adafruit.com/bonjour-zeroconf-networking-for-windows-and-linux

# also see WiFi Dropout (bellow) after getting updates
```


## SSH

SSH has to be installed by default on a headless machine.

```
#check (it has to be installed by default or the headless setup will not work)
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
# note if you have a private key (e.g. id_dsa or id_rsa file) 
# and you want to use it then place it in the .ssh folder now
[scp rsutherland@leek.local:.ssh/id_dsa .ssh/id_dsa]
[scp rsutherland@leek.local:.ssh/id_rsa .ssh/id_rsa]
~/bin/mkeys localhost
# that should have built the public (and if missing a new private) key 
# and added the public key to the authorized file 
# now try to log in, and it should not ask for a password since it used keys
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

Add the share to the /etc/samba/[smb.conf] file.

[smb.conf]: https://www.samba.org/samba/docs/current/man-html/smb.conf.5.html

```
[global]
# allow linking /var/www, /usr/lib/avr/include, /usr/lib/gcc/avr
# not secure? nothing about samba is secure 
# I would not run samba on a macine that needs to be secure
allow insecure wide links = yes

# add this to the very end of the file
[Samba]
path = /home/rsutherland/Samba
valid users = rsutherland
read only = no
wide links = yes
```

Restart Samba and check for errors.

```
sudo service smbd restart

#Check for errors
testparm

# my user name (rsutherland) on Windows can now map 
# to the share on the Pi Zero (computer name is pi1)
\\pi1\Samba
```

Note the Pi also shows the user home folders but Ubuntu did not, I will ignor those.


## Web Server

Use [apache2] to serve static or flat pages. 

[apache2]: https://packages.debian.org/stretch/apache2

```
sudo apt-get install apache2
# www-data is for the web server, and I can then be in its group (genius, yet simple)
sudo chown -R www-data:www-data /var/www
sudo chmod -R 775 /var/www
sudo usermod -a -G www-data rsutherland
# samba does not know about the new premission (hint: restart it)
```

Symlink to the web files so Samba can share and I can edit them on a Windows machine.

```
ln -s /var/www /home/rsutherland/Samba/www
```


## Python 3

Raspian stretch has Python 3 so this is not needed.

```
sudo apt-get install python3
```

## Serial 

Use raspi-config to turn off the login shell to the serial port, and enable the serial port hardware (e.g. /dev/ttyAMA0).

```
sudo raspi-config
```

Use raspi-config to disable the boot console.

I want to use /dev/ttyAMA0 since it has nRTS and nCTS functions. 

On a Pi Zero W the bluetooth is using the /dev/ttyAMA0 serial so it needs to be disabled, but I don't have a Zero W so am not sure what needs done.

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
# i2c command 3 will set the bootload address that is sent when DTR/RTS goes active to the bus manager with Remote firmware
/1/ibuff 3,49
{"txBuffer[2]":[{"data":"0x3"},{"data":"0x31"}]}
/1/iread? 2
{"rxBuffer":[{"data":"0x3"},{"data":"0x31"}]}
# i2c command 7 will clear the lockout bit on the bus manager with Remote firmware
/1/ibuff 7,0
{"txBuffer[2]":[{"data":"0x7"},{"data":"0x0"}]}
/1/iread? 2
# this will cause the bus manage to see the nRTS active and start the bootload cycle.
# after a delay I can use the RS-422 again.
```

This is sneaky mode operation on the Remote firmware. In sneaky mode the Pi Zero can set its bus manager lockout bit through the local controller board, it only works after the bus manager is powered up and while the bus has never been made active.


## Packages used for the AVR toolchain

```
sudo apt-get install git make gcc-avr binutils-avr gdb-avr avr-libc avrdude
```

Link the avr includes to the Samba share so I can use intelliSense with VSC from Windows.

```
#  link -s(ymbolic) <target> <linkname name>
ln -s /usr/lib/avr/include /home/rsutherland/Samba/lib/avr-libc/include
ln -s /usr/lib/gcc/avr /home/rsutherland/Samba/lib/gcc-avr
```

Setup intelliSense with Visual Studio Code 

I use a samba share so VSC can see the includes on my Win 10 machine. Then copy avr-libc includes from /usr/lib/avr/include, and gcc-avr includes from /usr/lib/gcc/avr/5.4.0/include.

c_cpp_properties.json in the .vscode folder.

```
{
    "configurations": [
        {
            "name": "AVR",
            "includePath": [
                "${workspaceRoot}",
                "Y:/lib/avr-libc/include",
                "Y:/lib/gcc-avr/5.4.0/include"
            ],
            "defines": [],
            "intelliSenseMode": "clang-x64",
            "browse": {
                "path": [
                    "${workspaceRoot}"
                ],
                "limitSymbolsToIncludedHeaders": true,
                "databaseFilename": ""
            },
            "cStandard": "c11",
            "cppStandard": "c++17"
        }
    ],
    "version": 3
}
```

I do not see a way to show VSC how to deal with macros passed from the Makefile (e.g. BAUD, MCU...) so I just turn off the suspected compile error detection with settings.json in the .vscode folder.

```
{
    "C_Cpp.errorSquiggles": "Disabled"
}
```


## SPI and I2C

Load an [RPUno] (ATmega328p) with [SpiSlv] firmware and use its command line to enable the AVR's SPI so we can test the interface between the Raspberry Pi [SPI] hardware and the AVR. Raspian needs its SPI master driver enabled with [raspi-config].

[RPUno]: https://github.com/epccs/RPUno/
[SpiSlv]: https://github.com/epccs/RPUno/tree/master/SpiSlv
[SPI]: https://www.raspberrypi.org/documentation/hardware/raspberrypi/spi/README.md
[raspi-config]: https://www.raspberrypi.org/documentation/configuration/raspi-config.md

Raspibin has an spi group setup in /etc/udev/rules.d/99-com.rules. I can add my user name to the spi group for the system to allow me to use the device.

``` 
sudo usermod -a -G spi rsutherland
# logout for the change to take
``` 

Compile spidev_test.c on the Pi with:

``` 
wget https://raw.githubusercontent.com/raspberrypi/linux/rpi-3.10.y/Documentation/spi/spidev_test.c
gcc -o spidev_test spidev_test.c
# run with
./spidev_test -s 500000 -D /dev/spidev0.0
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

Note: The output is offset a byte since it was held in the AVR and then echoed back durring the next transfer. 

I2C is enabled with [raspi-config] and has a i2c group to allow its use. At the moment the ATmega328pb toolchain is only on [RPUadpt].

[RPUadpt]: https://github.com/epccs/RPUadpt/

``` 
sudo apt-get install i2c-tools python3-smbus
sudo usermod -a -G i2c rsutherland
# logout for the change to take
i2cdetect 1
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-1.
I will probe address range 0x03-0x77.
Continue? [Y/n] Y
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- 2a -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
``` 

The devices at 2a is a ATmega328pb on [RPUadpt] ^6 running [BlinkLED].

[BlinkLED]: https://github.com/epccs/RPUadpt/tree/master/BlinkLED


## WiFi Dropout

I am using a [Realtek].

[Realtek]: https://www.adafruit.com/products/814

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
sudo shutdown -r 1
```


## Package Updates

```
# resynchronize
sudo apt-get update
# upgrade + handles dependencies
sudo apt-get dist-upgrade
# update the firmware (e.g. WiFi and other devices that are not part of Linux)
sudo apt-get install rpi-update
sudo rpi-update
# remove stale dependencies
sudo apt-get autoremove
sudo apt-get clean
```


## Editor

An editor that will take care of the tab character and no trailing spaces in a Makefile is helpful. I am using [Visual Studio Code] most of the time, but for small changes it is a little heavy. SciTE is an editor I also use, it has a SciTEUser.properties file that I set as follows:

[Visual Studio Code]: https://code.visualstudio.com/

```
tabbar.multiline=1
#toolbar.visible=1

statusbar.visible=1

# use space indentation for everything, but the makefile magic bellow
#tabsize=4
#indent.size=4
use.tabs=0

# when the Makefile pattern matches use tab
indent.size.$(file.patterns.make)=4
tab.size.$(file.patterns.make)=4
use.tabs.$(file.patterns.make)=1
```


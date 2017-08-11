# rpirtsrtc

[rpirtscts] makes a linux command for enabling the hardware flow control pins for the `ttyAMA0` serial port on a [Raspberry Pi].

[rpirtscts]: https://github.com/mholling/rpirtscts
[Raspberry Pi]: http://www.raspberrypi.org/


## Usage

Building the program:

```
sudo apt-get install git
# I like to put my git repos in a samba share
[mkdir Samba]
[cd Samba]
git clone git://github.com/epccs/RPUpi.git
```

Set the working directory, build the executable, and place it in the bin directory (a convenient place for that sort of stuff):

```
cd RPUpi/RPiRtsCts
make
cp rpirtscts ~/bin/rpirtscts
make clean
cd ~
chmod ugo+x ~/bin/rpirtscts
```

Only root can change /dev/mem, so the `rpirtscts` executable command need run with sudo, it takes a single option, either `on` or `off`, to enable or disable the flow control pins.

```
sudo ./bin/rpirtscts on
```


Finally, you will need to instruct the serial port driver to use the hardware flow control signals, e.g.:

```
stty -F /dev/ttyAMA0 crtscts
# normaly I use picocom
picocom -b 38400 /dev/ttyAMA0
```

('/dev/ttyAMA0` is the character device to the Raspberry Pi hardware UART.)

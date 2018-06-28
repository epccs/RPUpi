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

Follow these rc.local [recommendations].

[recommendations]: https://www.raspberrypi.org/documentation/linux/usage/rc-local.md

Startup with RTS handshacking on.

```
/home/rsutherland/bin/rpirtscts on &
exit 0
```

Now when I use picocom or avrdude it will pull RTS active.

```
picocom -b 38400 /dev/ttyAMA0
```


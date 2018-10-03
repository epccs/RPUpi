# WSGIdaemon

## ToDo

Don't echo the command, consumers probably don't want that.

Validate the JSON?


## Overview

Web Server Gateway Interface (WSGI) daemon for serial

The idea is to use the gateway to convert a http request into a command for the serial link, the commands I use resemble "/0/id?". The second character is an address on the serial bus (it is a multi-drop), the command is "id", and the "?" means it is a query (but everything has an echo and returns JSON so it's just adding overhead)


## Startup

run at a terminal command line

```
python3 WSGIdaemon.py
```

run in background

```
python3 WSGIdaemon.py &
exit 0
```

run at startup, place the script in /home/my_user_name/bin and make it executable, then tell the system to run it from /etc/rc.local

```
[su my_user_name]
[mkdir bin]
cd bin
wget https://raw.githubusercontent.com/epccs/RPUpi/master/WSGIdaemon/WSGIdaemon.py
chmod ugo+x WSGIdaemon.py
sudo nano /etc/rc.local
```

Follow these rc.local [recommendations].

[recommendations]: https://www.raspberrypi.org/documentation/linux/usage/rc-local.md


## Browser

Place url in a brower

```
http://localhost:8000/?addr=0&cmd=id&q=true
```

Arguments (arg1..arg5) have been added.

```
http://localhost:8000/?addr=0&cmd=iaddr&q=false&arg1=41
```

Which sends the command "/0/iaddr 41" and I should get back the JSON "{"address":"0x29"}". That requires an RPUadpt or RPUpi shield on the controler, and for the controler to run [i2c-debug] or software that includes it.

[i2c-debug]: https://github.com/epccs/RPUno/tree/master/i2c-debug


## Oher Referances

[PEP-3333] is a good overview, it is important to read things like that when encode and decode have pushed the wrong buttons. It is really unbelievable that this stuff works at all. 

[PEP-3333]: https://www.python.org/dev/peps/pep-3333/

* For WSGI see https://docs.python.org/3.7/library/wsgiref.html
* For CGI see https://docs.python.org/3.7/library/cgi.html
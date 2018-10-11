# WSGIdaemon

## ToDo

Don't echo the command, but for now, I want to see it.

Validate the JSON?


## Overview

Web Server Gateway Interface (WSGI) daemon for serial

The idea is to use the gateway to convert an HTTP request into a command for the serial link, the commands I use resemble "/0/id?". The second character is an address on the serial bus (it is a multi-drop), the command is "id", and the "?" means it is a query.

The daemon has now been set to allow the origin to be from everywhere, which means Cross-Origin Resource Sharing (CORS) is wide open. The reference I used to do this is over seven years old so if someone has a better idea please add an issue and suggest.


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

Place URL in a browser.

```
http://localhost:8000/?addr=0&cmd=id&q=true
```

Arguments (arg1..arg5) have been added.

```
http://localhost:8000/?addr=0&cmd=iaddr&q=false&arg1=41
```

Which sends the command "/0/iaddr 41" and I should get back the JSON "{"address":"0x29"}". That requires an RPUadpt or RPUpi shield on the controller, and for the controller to run [i2c-debug] or software that includes it.

[i2c-debug]: https://github.com/epccs/RPUno/tree/master/i2c-debug


## WSGI Python Referance

[PEP-3333] is a good overview, it is important to read things like that when encode and decode have pushed the wrong buttons. It is unbelievable that this stuff works at all. 

[PEP-3333]: https://www.python.org/dev/peps/pep-3333/

* For WSGI see https://docs.python.org/3.7/library/wsgiref.html
* For CGI see https://docs.python.org/3.7/library/cgi.html


## WSGI Python Frameworks 

It is not clear to me what these frameworks do, or when I would instantiate the device  (e.g. open the serial port) I want to act as a gateway for.

* For Flask see https://github.com/pallets/flask
* For Bottle see https://github.com/bottlepy/bottle

Flask has a lot a [documentation] and that is probably what I need to use it. If I do work on such a project I will start a separate repository for it. 

[documentation]: https://palletsprojects.com/p/flask/


## WSGI and Cross-Origin Resource Sharing (CORS)

Modern web browsers have implemented the W3C standard [CORS] that allows the browser to have a same-origin policy. This policy means the browser will only allow callbacks from URL's inside a page that has the same origin (scope), that is they have identical schemes, hosts, and ports ([RFC 6454]).

[CORS]: https://www.w3.org/TR/cors/
[RFC 6454]: https://tools.ietf.org/html/rfc6454

* Microsoft notes https://docs.microsoft.com/en-us/aspnet/core/security/cors?view=aspnetcore-2.1
* Mozilla notes https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
* Google notes https://cloud.google.com/storage/docs/cross-origin

CORS can be turned off in Firefox with this extension (this is for developer reference)

* Cors Everywhere https://github.com/spenibus/cors-everywhere-firefox-addon


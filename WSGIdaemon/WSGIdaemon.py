#!/usr/bin/env python3
# Web Server Gateway Interface (WSGI) deamon 
# use to serve request on a TCP/IP PORT for serial devices that use simple commands 
# at CLI run with
# python3 WSGIdaemon.py
# killing it is a PITA, I just close the terminal, yep that needs some work.
# to try it open a brower with url: http://localhost:8000/?addr=0&cmd=id&q=true

# For info on WSGI see https://docs.python.org/3.7/library/wsgiref.html
# For info on CGI see https://docs.python.org/3.7/library/cgi.html

# The goal is to hold a seril link open while a web server runs pages, e.g. use the WSGI daemon to access the serial link.
# it takes commands from the CGI query string and turns them into the format I am using e.g. "/0/id?" or "/0/adc 3,4,5" 

# I am using python3 so make sure the serial package is installed. 
# sudo apt-get install python3-serial 

import json
import serial
from time import sleep
from wsgiref.simple_server import make_server
from cgi import parse_qs, escape

# claim the serial link (e.g. it is a resourse that I will provide use of)
# hardwar serial on R-Pi is/dev/ttyAMA0
# FTDI and some other usb serial is /dev/ttyUSB0
# Arduino's ATmega16u2 is a modem... what? /dev/ttyACM0
device = "/dev/ttyUSB0"
sio = serial.Serial(device,38400, timeout=3)
print("claim serial link " + device + "\n")
# TBD only run one WSGI server with one thread

# A relatively simple WSGI application. 
def simple_app(environ, start_response):
    
    # Returns a dictionary from CGI in which the values are lists, it is the easy way
    query_string = parse_qs(environ['QUERY_STRING'])
    #given URL: http://localhost:8000/?addr=0&cmd=id&q=true
    #the QUERY_STRING is: addr=0&cmd=id&q=true
    addr = query_string.get('addr', [''])[0] # Returns the first addr value
    cmd_base = query_string.get('cmd', [''])[0] # Returns the first cmd value
    q = query_string.get('q', [''])[0] # Returns the first q value
    # TBD thre are arguments to add, but this is a start

    status = '200 OK'
    headers = [('Content-type', 'text/plain; charset=utf-8')]

    start_response(status, headers)

    #check for correctness of command
    if (addr == ''):
        addr = '0' # for default of /0/id?
    if ( (addr < '0') or (addr > '9') ):
        ret = [ ("ERR: addr must be in the range 0..9\n").encode('utf-8') + 
                (b"ERR: bad query_string \"addr=" + addr.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
        return ret
    if (cmd_base == ''):
        cmd_base = "id" # default
    if ( not (cmd_base.isalpha()) ):
        ret = [ ("ERR: cmd must be a string that isalpha\n").encode('utf-8') + 
                (b"ERR: bad query_string \"cmd=" + cmd_base.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
        return ret
    if (q == ''):
        q = 'true' # default needs a ? after the cmd_base 
    if ( not ( (q == 'true') or (q == 'false')) ):
        ret = [ ("ERR: q must be true or false\n").encode('utf-8') + 
                (b"ERR: bad query_string \"q=" + q.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
        return ret

    # note: if the command will not encode with ascii then use bytes e.g.
    # command = b"/"+bytes([192])+b"/id?"
    if (q == "true"):
        command =  "/"+addr+"/"+cmd_base+"?\n"
    else:
        command =  "/"+addr+"/"+cmd_base+"\n"
    sio.write(command.encode('utf-8')) # "/0/id?" is like the command I want to send on the serial link 
    sio_echo_cmd = sio.readline().strip() # my serial device echo's the command 
    sio_echo = sio.readline().strip() # and then outpus the JSON
    
    # format as bytestring suitable for transmission as HTTP response headers
    ret = [ (sio_echo_cmd + b"\n").decode().encode('utf-8') +
            (sio_echo + b"\n").decode().encode('utf-8') ]
    return ret


with make_server('', 8000, simple_app) as httpd:
    print("Serving on port 8000...")
    httpd.serve_forever()

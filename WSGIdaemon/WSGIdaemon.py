#!/usr/bin/env python3
# Copyright (C) 2018 Ronald S Sutherland
#
# This is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with the Arduino DigitalIO Library.  If not, see
# http://www.gnu.org/licenses/
#
# Web Server Gateway Interface (WSGI) daemon 
# use web server techniques to act as a gateway interface to a serial link 
#
# at CLI
# python3 WSGIdaemon.py
#
# set the host value (near bottom) for your machine
# to try the interface open a brower with url: http://host:8000/?addr=0&cmd=id&q=true

# For info on WSGI see https://docs.python.org/3.7/library/wsgiref.html
# For info on CGI see https://docs.python.org/3.7/library/cgi.html

# The goal is to hold a serial link open while client(s) make request to access the serial link.
# it takes parameters from the query string and turns them into the serail commands I use e.g. "/0/id?" or "/0/adc 3,4,5" 

# Using python3 so make sure the serial package is installed. 
# sudo apt-get install python3-serial 

import sys
sys.path.append('wsgiac') #allow import of Access Control
from accesscontrol import  AccessControlMiddleware
import json
import serial
from time import sleep
import wsgiref.simple_server
from cgi import parse_qs, escape
import os

# claim the serial link (e.g. it is a resourse that I will provide use of)
# hardwar serial on R-Pi is/dev/ttyAMA0
# FTDI and some other usb serial is /dev/ttyUSB0
# Arduino's ATmega16u2 is a modem... what? /dev/ttyACM0
if os.uname().machine == 'i686': # Ubuntu with RPUftdi
    device = "/dev/ttyUSB0"
if os.uname().machine == 'armv6l': # R-Pi's hardware UART
    device = "/dev/ttyAMA00"
sio = serial.Serial(device,38400, timeout=3)
print("serial link " + device)

# A relatively simple WSGI application. 
def simple_app(environ, start_response):
    
    # Returns a dictionary from CGI in which the values are lists, it is the easy way
    query_string = parse_qs(environ['QUERY_STRING'])
    #given URL: http://localhost:8000/?addr=0&cmd=id&q=true&arg1=1&arg2=2&arg3=3&arg4=4&arg5=5
    #the QUERY_STRING is: addr=0&cmd=id&q=true&arg1=1&arg2=2&arg3=3&arg4=4&arg5=5
    addr = query_string.get('addr', [''])[0] # Returns the first addr value
    cmd_base = query_string.get('cmd', [''])[0] # Returns the first cmd value
    q = query_string.get('q', [''])[0] # Returns the first q value
    arg1 = query_string.get('arg1', [''])[0] # Returns the first argument value
    arg2 = query_string.get('arg2', [''])[0] # Returns the first argument value
    arg3 = query_string.get('arg3', [''])[0] # Returns the first argument value
    arg4 = query_string.get('arg4', [''])[0] # Returns the first argument value
    arg5 = query_string.get('arg5', [''])[0] # Returns the first argument value

    #check for correctness of command
    if (addr == ''):
        addr = '0' # for default of /0/id?
    if ( not (addr.isalnum() and (len(addr) == 1) ) ):
        status = '400 Bad Request'
        headers = [('Content-type', 'text/plain; charset=utf-8')]
        start_response(status, headers)
        ret = [ ("ERR: addr must be a string of len == 1 that isalnum\n").encode('utf-8') + 
                (b"ERR: bad query_string \"addr=" + addr.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
        return ret
    if (cmd_base == ''):
        cmd_base = "id" # default
    if ( not (cmd_base.isalpha()) ):
        status = '400 Bad Request'
        headers = [('Content-type', 'text/plain; charset=utf-8')]
        start_response(status, headers)
        ret = [ ("ERR: cmd must be a string that isalpha\n").encode('utf-8') + 
                (b"ERR: bad query_string \"cmd=" + cmd_base.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
        return ret

    # start putting the command togather
    command =  "/"+addr+"/"+cmd_base

    if (q == ''):
        q = 'true' # default needs a ? after the cmd_base 
    if ( not ( (q == 'true') or (q == 'false')) ):
        status = '400 Bad Request'
        headers = [('Content-type', 'text/plain; charset=utf-8')]
        start_response(status, headers)
        ret = [ ("ERR: q must be true or false\n").encode('utf-8') + 
                (b"ERR: bad query_string \"q=" + q.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
        return ret

    # baggage from looking at GPIB commands for too many years
    if (q == "true"):
        command =  command+"?"

    if (len(arg1) >= 1):
        if ( not (arg1.isalnum() ) ):
            status = '400 Bad Request'
            headers = [('Content-type', 'text/plain; charset=utf-8')]
            start_response(status, headers)
            ret = [ ("ERR: arg1 must be a string that isalnum\n").encode('utf-8') + 
                    (b"ERR: bad query_string \"arg1=" + arg1.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
            return ret
        command =  command+" "+arg1
        
        #ignore arg2 if arg1 not given
        if (len(arg2) >= 1):
            if ( not (arg2.isalnum() ) ):
                status = '400 Bad Request'
                headers = [('Content-type', 'text/plain; charset=utf-8')]
                start_response(status, headers)
                ret = [ ("ERR: arg2 must be a string that isalnum\n").encode('utf-8') + 
                        (b"ERR: bad query_string \"arg2=" + arg2.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
                return ret
            command =  command+","+arg2

            #ignore arg3 if arg2 not given
            if (len(arg3) >= 1):
                if ( not (arg3.isalnum() ) ):
                    status = '400 Bad Request'
                    headers = [('Content-type', 'text/plain; charset=utf-8')]
                    start_response(status, headers)
                    ret = [ ("ERR: arg3 must be a string that isalnum\n").encode('utf-8') + 
                            (b"ERR: bad query_string \"arg3=" + arg3.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
                    return ret
                command =  command+","+arg3

                #ignore arg4 if arg3 not given
                if (len(arg4) >= 1):
                    if ( not (arg4.isalnum() ) ):
                        status = '400 Bad Request'
                        headers = [('Content-type', 'text/plain; charset=utf-8')]
                        start_response(status, headers)
                        ret = [ ("ERR: arg4 must be a string that isalnum\n").encode('utf-8') + 
                                (b"ERR: bad query_string \"arg4=" + arg4.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
                        return ret
                    command =  command+","+arg4

                    #ignore arg5 if arg4 not given
                    if (len(arg5) >= 1):
                        if ( not (arg5.isalnum() ) ):
                            status = '400 Bad Request'
                            headers = [('Content-type', 'text/plain; charset=utf-8')]
                            start_response(status, headers)
                            ret = [ ("ERR: arg5 must be a string that isalnum\n").encode('utf-8') + 
                                    (b"ERR: bad query_string \"arg5=" + arg5.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
                            return ret
                        command =  command+","+arg5

    sio.write((command+"\n").encode('utf-8')) # "/0/id?" is like the command I want to send on the serial link 
    sio_echo_cmd = b""
    sio_echo_cmd = sio.readline().strip() # my serial device echo's the command 
    if ( not (len(sio_echo_cmd) >= 1) ):
        status = '503 Service Unavailable'
        headers = [('Content-type', 'text/plain; charset=utf-8')]
        start_response(status, headers)
        ret = [ ("ERR: device did not echo command\n").encode('utf-8') + 
                (b"ERR: \"command=" + command.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
        return ret
    if ( not (sio_echo_cmd == command.encode('utf-8') ) ):
        status = '503 Service Unavailable'
        headers = [('Content-type', 'text/plain; charset=utf-8')]
        start_response(status, headers)
        ret = [ ("ERR: device command echo was bad\n").encode('utf-8') + 
                (b"ERR: \"command=" + command.encode('utf-8') + b"\"\n").decode().encode('utf-8') +
                (b"ERR: \"echo_cmd=" + sio_echo_cmd + b"\"\n").decode().encode('utf-8')]
        return ret
    sio_echo = sio.readline().strip() # and then outpus the JSON
    sio.write("\n".encode('utf-8')) # some commands (e.g. /0/adc 1) will keep outputing at timed intervals,  this should stop that
    if ( not (len(sio_echo) >= 1) ):
        status = '503 Service Unavailable'
        headers = [('Content-type', 'text/plain; charset=utf-8')]
        start_response(status, headers)
        ret = [ ("ERR: serial device found but ouput not returned\n").encode('utf-8') + 
                (b"ERR: \"command=" + command.encode('utf-8') + b"\"\n").decode().encode('utf-8') ]
        return ret
    try:
        json_object = json.loads(sio_echo)
    except ValueError as e:
        status = '503 Service Unavailable'
        headers = [('Content-type', 'text/plain; charset=utf-8')]
        start_response(status, headers)
        ret = [ ("ERR: serial device returned bad JSON\n").encode('utf-8') + 
                (b"ERR: \"command=" + command.encode('utf-8') + b"\"\n").decode().encode('utf-8') +
                (b"ERR: \"sio_echo=" + sio_echo + b"\"\n").decode().encode('utf-8')]
        return ret

    status = '200 OK'
    headers = [('Content-type', 'text/plain; charset=utf-8')]

    start_response(status, headers)
    
    # JSON is formated as bytestring suitable for transmission with HTTP response headers
    ret = [ (sio_echo + b"\n").decode().encode('utf-8') ]
    return ret

# Access-Control Middleware
# from https://github.com/ianb/wsgi-access-control
application = AccessControlMiddleware(
    simple_app, allow_origin='*',
    allow_methods=('GET','POST','PUT','DELETE','OPTIONS'),
    allow_headers=('X-Authorization', 'Authorization',
                   'X-If-Modified-Since', 'X-If-Unmodified-Since',
                   'Content-Type'),
    expose_headers=('X-Weave-Timestamp', 'X-Weave-Backoff',
                    'X-Weave-Alert', 'X-Weave-Records'))

port = 8000
#host  = ''
#host = 'localhost'
host = "192.168.4.8" # set your IP address if you see "OSError: [Errno 99] Cannot assign requested address"

# create server with settings server_class=WSGIServer, handler_class=WSGIRequestHandler
# seems that Python does a single thread and process for these settings, and that is what I want.
with wsgiref.simple_server.make_server(host, port, application) as httpd:
    print("host " + host + " is")
    print("serving on port " + str(port))
    httpd.serve_forever()

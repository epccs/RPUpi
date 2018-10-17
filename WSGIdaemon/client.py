#!/usr/bin/env python3
# Python as a web client

# http://docs.python-requests.org/en/master/
# sudo apt-get install python3-requests
import requests

# WSGIdaemon turns "?addr=1&cmd=id&q=true" into "/1/id?" and sends it on serial for AVR
# The AVR sends back a JSON reply: {'id': {'name': 'KNL', 'desc': 'RPUno (14140^9) Board /w atmega328p', 'avr-gcc': '5.4.0'}}
# which the WSGIdaemon sends back, and request can turn it right into a dictionary.
r = requests.get('http://192.168.0.7:8000/?addr=1&cmd=id&q=true')
if (r.status_code == 200):
    print( str(r.json()) )
    print( "id.name=" + str(r.json()["id"]["name"]))



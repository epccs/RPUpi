#!/usr/bin/env python
import RPi.GPIO as GPIO
import os

# RPUpi Shutdown switch goes to BCM6
shutdown_pin=6

GPIO.setmode(GPIO.BCM)

GPIO.setup(shutdown_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

try:
    GPIO.wait_for_edge(shutdown_pin, GPIO.FALLING)
    
    #edge detection should avoid repeated polling
    os.system("sudo shutdown -h now")
    #Send command to system to shutdown
except:
    pass

GPIO.cleanup() # shutdown should allow time for cleanup  


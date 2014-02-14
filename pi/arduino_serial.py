#!/usr/bin/env python

import serial

ser = serial.Serial('/dev/ttyACM0')

while True:
    if ser.inWaiting():
        try:
            print ser.readline()
        except:
            pass

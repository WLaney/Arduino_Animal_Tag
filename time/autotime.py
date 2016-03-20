#!/usr/bin/python3
"""autotime.py - Set the RTC through a serial port automatically.

This communicates with the sketch found in this folder; upload the
sketch, wait for it to ask for input, then run this script.
"""
import datetime
import sys

try:
	import serial
except ImportError:
	print("You need to install pyserial to use this script.")
	print("Get it with `python3 -m pip install pyserial`.")
	sys.exit(1)

if len(sys.argv) < 2:
	print("Usage: python3 autotime.py portname")
	print("That's the full path to the port, BTW")
	sys.exit(1)
	
portname = sys.argv[1]
with serial.Serial(portname) as s:
	s.port = portname
	s.open()
	#hh:mm:ss MM/DD/YYYY
	d = datetime.datetime.now()
	s.write(d.strftime("%H:%M:%S %m/%d/%Y"))

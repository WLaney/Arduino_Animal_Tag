#!/usr/bin/python3
import struct

print("Gyro Attempt:")
with open("gyro.dat", 'rb') as f:
	while True:
		data = f.read(6)
		if not data:
			break
		read = struct.unpack('<hhh', data)
		print("%3d %3d %3d" % read)

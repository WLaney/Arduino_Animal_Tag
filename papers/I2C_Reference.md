# I2C Slave Addresses

This contains the addresses of each I2C device attached to the Sharkduino.

> Could someone from the hardware team figure out what these pins are
> set to? I assume these are not set through the Arduino...

## DS1339B Real-Time Clock

Always 0xD0.

## FXAS21002 Gyroscope

If the SA0 pin is high, the address is 0x21. If not, the address is
0x20.

## MMA8451Q Accelerometer

If the SA0 pin is low, the address is 0x1C; if not, it is 0x1D.

## MS5803-14BA Pressure Sensor

If the CSB pin is low, the address is 0x76; if not, it is 0x77.

## Hexdigits.txt

This is an old reference sheet William used.

```
Accelerometer
What I want: 0001 1101 (0x1D)
What I got:  0001 1100 (0x1C)

Pressure:
0x76 (1110110) [note: started working when he changed I2C address from 0x77] 
[note 2: this corrosponds to ADDRESS_LOW in the code]

Temperature:
0x48 (1001000)

Gyroscope:
0x6B (1101011)
```

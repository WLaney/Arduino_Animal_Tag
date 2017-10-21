# Sharkduino EEPROM Layout

This document describes the EEPROM addresses used by a properly configured
Sharkduino tag. If you're using the tag's EEPROM, read this document
carefully; if you change the way the tag uses it, update the specification
so that it will remain useful.

EEPROM addresses are groupd by similar function; for example, all addresses related to tag configuration are contiguous. Do not put new sections close to old ones if possible; this allows those sections to be expanded further if necessary.

The ATmega328p (the Sharkduino's CPU) has 1024 bytes of EEPROM. The EEPROM memory has a specific life of 100,000 write/erase cycles, so be careful in how often you write.

## Configuration Section

This consists of tag-specific options set in `Configure` and used by
the `Animal_Tag` sketch.

byte  | val            | type
------|----------------|------
0-3   | `name       `  | char[4] (no \0)
4     | `orient     `  | bool
5     | `bias_x     `  | float
9     | `bias_y     `  | float
13    | `bias_z     `  | float
17    | `accel_scale`  | byte
18    | `gyro_scale `  | byte
19    | `sample_rate`  | byte
20    | `hq_accel   `  | byte
21-24 | `alarm_delay`  | long int (32-bit int)

val         | description
------------|------------
name        | Tag name (up to 4 characters). The tag's name is physically labeled on its underside, but the "v" and period are removed; e.g. "v2.1a" becomes "21a ".
orient      | Used only in prototype tags. This value should be 0.
bias_x      | Used only in prototype tags.
bias_y      | Used only in prototype tags.
bias_z      | Used only in prototype tags.
accel_scale | Accelerometer range. 0=2g's, 1=4g's, 2=g's.
gyro_scale  | Gyroscope range. 0=2000DPS, 1=1000DPS, 2=500DPS, 3=250DPS.
sample_rate | Rate at which data is collected. 0=12.5Hz, 1=25Hz, 2=50Hz.
hq_accel    | Increase accelerometer oversampling at the cost of precision.
alarm_delay | Start up delay, in seconds. The tag header is written at the end of the delay.

> This document is written in [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet).

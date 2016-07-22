.============.
| How to Use |
.============.
This is for Mac/Linux/UNIX users. If you're running Windows, you can
get it to work, but you need to download a C++ compiler (preferably
MinGW) and potentially alter the makefile.

 * Open a terminal in this directory
 * Run make
 * Use ./shark-parser (raw data filename)

If everything worked out right, you should get no complaints and have
two new CSVs: a header file with the name of the tag and a few other
things, and a data file with the actual tag data. From there, start
running it through the MATLAB scripts.

.================.
| Quasi-man Page |
.================.
shark-parser (in-file) (data-file) (header-file)
 
Convert a raw .srk file into two CSV files. data-file contains
the actual tag data, and follows this format:

 ax, ay, az, gx, gy, gz,         date_time, temp, pressure
   ,   ,   ,   ,   ,   , 1111/1/30 22:22:22,     ,
  0,  0,  0,  0,  0,  0,                   ,     ,
  0,  0,  0,  0,  0,  0,                   ,     ,
  0,  0,  0,  0,  0,  0,                   ,     ,
   ,   ,   ,   ,   ,   , 1111/1/30 22:22:40, 22.5, 0
  0,  0,  0,  0,  0,  0,                   ,     ,
  0,  0,  0,  0,  0,  0,                   ,     ,
  0,  0,  0,  0,  0,  0,                   ,     ,
                      [.....]
 
The first line of the file will only contain date and time; everything
else will be left blank. The file then alternates between multiple
lines of accelerometer and gyroscope data and a single line of
date-time, temp, and pressure data. Due to the way the raw data is
written, the data will NOT end with time data.

This odd import format is due to the way that data is read into the
tag. The tag writes the date and time infrequently, and when it does,
it does not directly correspond with a particular accelerometer/gyroscope
value. Therefore, further import code must be written (we've written
some in MATLAB) to estimate the timing of each accelerometer and
gyroscope read.

Accelerometer data is in g's. Gyroscope data is in degrees per second (dps); date and time
data follows a Y-M-D h:m:s format, and will lack leading zeroes.
temperature data is in degrees celsius, and pressure is in millibars.

header-file contains metadata (currently just the tag name and
gyroscope biases).
 name, xbias, ybias, zbias
     ,      ,      ,


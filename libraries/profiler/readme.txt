Simple Profiler
Original Code by William F. Dudley Jr. (http://www.dudley.nu/arduino_profiling)
Refactored by Ben Powell

This library implements a simple system for determining how much time
is being spent in different sections of your code. Code is split into
numerical "bins"; the Arduino, in turn, keeps track of how many milliseconds
are spent in the bin for every minute of processing. For more information,
see the link above.

This library is built for an 8MHz ATMega 328P, but can be modified for
other boards as well.

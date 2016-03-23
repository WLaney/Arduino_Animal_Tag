#include <SPI.h>
#include "ds3234.h"

// something for ds3234 compatibility
using uint_8 = unsigned char;
const int cs = 9; // chip select pin

/*
 * This is a utility for displaying and changing the RTC's current time/date.
 * Data can be entered manually, but for optimal results, wait for it to ask for input
 * and then run autotime.py.
 * 
 * The program will display the current time/date and then ask for a new one with a certain format.
 * If you supply that data, it will update the clock (enter a blank line if you don't want this) and
 * then display the new time.
 * 
 * It will then display the time every second.
 */

// Print the current time, ask to get the new time, then update the clock and print
// the updated time
void setup()
{
    // Initialization
    Serial.begin(9600);
    while (!Serial)
      ;
    
    DS3234_init(cs, DS3234_INTCN);
    SPI.setDataMode(SPI_MODE1); // we don't use the SD card, just the RTC
    Serial.println("Enter time as hh:mm:ss HH/DD/YYYY (include spaces, use zeroes):");
}

// Print the time once every second
void loop()
{
  static bool is_reading = true; // this is only initialized ocne
  if (is_reading) {
    while (Serial.available() > 0) {
      String input = Serial.readString();
      if (input.length() >= strlen("00:00:00 00/00/0000")) {
        set_time(input.c_str());
        is_reading = false;
      }
    }
  } else {
    print_time();
    delay(1000);
  }
}

// Set time from a format string.
void set_time(const char *s) {
  ts t;
  DS3234_get(cs, &t);
  sscanf(s, "%hhd:%hhd:%hhd %hhd/%hhd/%d", &(t.hour), &(t.min), &(t.sec),
            &(t.mon), &(t.mday), &(t.year));
  
  DS3234_set(cs, t);
}

// Print the current time from the RTC
void print_time() {
  // should only need 20 bytes, but we can go a little over
  const int buff_max = 24;
  ts t;
  DS3234_get(cs, &t);
  char buff[buff_max];
  snprintf(buff, buff_max, "%02d:%02d:%02d %02d/%02d/%d", t.hour,
            t.min, t.sec, t.mon, t.mday, t.year);
  Serial.println(buff);
}

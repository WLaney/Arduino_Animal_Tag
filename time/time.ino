#include <SPI.h>
#include "ds3234.h"

#define BUFF_MAX 256

using uint_8 = unsigned char;
const int cs = 10; // chip select pin

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
    SPI.setDataMode(SPI_MODE1);
    DS3234_init(cs, DS3234_INTCN);
    
    // Print current time
    Serial.print("Old time:");
    print_time();
    
    // Get current time as a string
    Serial.println("Enter time as hh:mm:ss MM/DD/YYYY:");
    while (!Serial.available())
      ;
    const char *s = Serial.readString().c_str();
    set_time(s);
    
    // Print new time
    Serial.print("New time: ");
    print_time();
}

// Print the time once every second
void loop()
{
  Serial.print("time: ");
  print_time();
  delay(1000);
}

// Set time from a format string (must be at least 15 chars)
void set_time(const char *s) {
  if (strlen(s) == 0) return;
  
  ts t;
  DS3234_get(cs, &t);
  sscanf(s, "%hhd:%hhd:%hhd %hhd/%hhd/%d", &(t.hour), &(t.min), &(t.sec),
            &(t.mon), &(t.mday), &(t.year));
  
  DS3234_set(cs, t);
}

// Print the current time from the RTC
void print_time() {
  ts t;
  DS3234_get(cs, &t);
  char buff[BUFF_MAX];
  snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
           t.mon, t.mday, t.hour, t.min, t.sec);
  Serial.println(buff);
}

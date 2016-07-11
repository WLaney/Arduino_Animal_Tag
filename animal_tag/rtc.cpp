#include "rtc.hpp"
#include "ds3234.h"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>

#define RTC_CS 9

inline void print_02d(File,int);

// Time data is on the heap to avoid overuse
// (does this actually help?)
ts *tme = NULL;

void rtc_setup() {
  DS3234_init(RTC_CS, DS3234_INTCN);
}

void rtc_update() {
  DBGLN("Read RTC");
  if (!tme)
    tme = new ts;
  DS3234_get(RTC_CS, tme);
}

void rtc_write(File sd) {
  if (!tme) {
    DBGSTR("Time not set; couldn't write\n");
    return;
  }
  DBGLN("Wrote time");
  // year-month-mday hour:min:sec
  print_02d(sd, tme->year);
  sd.print('-');
  print_02d(sd, tme->mon);
  sd.print('-');
  print_02d(sd, tme->mday);
  sd.print(' ');
  print_02d(sd, tme->hour);
  sd.print(':');
  print_02d(sd, tme->min);
  sd.print(':');
  print_02d(sd, tme->sec);
  delete tme;
  tme = NULL;
}

// Sleeker than ssprintf
inline void print_02d(File sd, int d) {
  if (d < 10)
    sd.print('0');
  sd.print(d);
}

#include "rtc.hpp"
#include "ds3234.h"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>

#define RTC_CS 9

// Time data is on the heap since
// it doesn't always need to be
// present (fragmentation?)
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
  sd.print(tme->year);
  sd.print('-');
  sd.print(tme->mon);
  sd.print('-');
  sd.print(tme->mday);
  sd.print(' ');
  sd.print(tme->hour);
  sd.print(':');
  sd.print(tme->min);
  sd.print(':');
  sd.print(tme->sec);
  delete tme;
  tme = NULL;
}

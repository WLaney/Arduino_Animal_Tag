#include "rtc.hpp"
#include "ds3234.h"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>

#define RTC_CS 9

ts tme;

void rtc_setup() {
  DS3234_init(RTC_CS, DS3234_INTCN);
}

void rtc_update() {
  DBGLN("Read RTC");
  DS3234_get(RTC_CS, &tme);
}

void rtc_write(File sd) {
  DBGLN("Wrote time");
  // year-month-mday hour:min:sec
  sd.print(tme.year);
  sd.print('-');
  sd.print(tme.mon);
  sd.print('-');
  sd.print(tme.mday);
  sd.print(' ');
  sd.print(tme.hour);
  sd.print(':');
  sd.print(tme.min);
  sd.print(':');
  sd.print(tme.sec);
}

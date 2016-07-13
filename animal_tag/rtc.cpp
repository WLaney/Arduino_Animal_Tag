#include "rtc.hpp"
#include "ds3234.h"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>

constexpr int rtc_cs = 9;

inline void print_02d(File,int);

// Time data is on the heap to avoid overuse
// (does this actually help?)
ts *tme = NULL;

void rtc_setup() {
  DS3234_init(rtc_cs, DS3234_INTCN);
}

void rtc_update() {
  DBGLN("Read RTC");
  if (!tme)
    tme = new ts;
  DS3234_get(rtc_cs, tme);
}

/*
 * Write raw ts to the SD card
 */
void rtc_write(File sd) {
  if (!tme) {
    DBGSTR("Time not set; couldn't write\n");
    return;
  }
  DBGLN("Wrote time");
  sd.write((byte *) tme, sizeof(ts));
  delete tme;
  tme = NULL;
}

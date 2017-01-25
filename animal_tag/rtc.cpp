#include <Arduino.h>
#include <SD.h>
#include "rtc.hpp"
#include "debug.h"
#include "DSRTCLib.h"

// Copied from ds3234.h
// TODO Replace with smaller ts instead of attempting backwards compatibility
struct ts {
    uint8_t sec;         /* seconds */
    uint8_t min;         /* minutes */
    uint8_t hour;        /* hours */
    uint8_t mday;        /* day of the month */
    uint8_t mon;         /* month */
    int year;            /* year */
    uint8_t wday;        /* day of the week */
    uint8_t yday;        /* day in the year */
    uint8_t isdst;       /* daylight saving time */
    uint8_t year_s;      /* year in short notation*/
};

constexpr byte int_pin = 2; // Interrupt pin (2 or 3)
constexpr byte int_num = 0; // Interrupt number (???)

DSRTCLib rtc(int_pin, int_num);

void rtc_setup() {
	rtc.start();
}

void rtc_update() {
  DBGLN("Read RTC");
  rtc.readTime();
}

/*
 * Write raw ts to the SD card
 */
void rtc_write(File sd) {
  DBGLN("Wrote time");
  ts t;
  t.sec  = rtc.getSeconds();
  t.min  = rtc.getMinutes();
  t.hour = rtc.getHours();
  t.mday = rtc.getDays();
  t.mon  = rtc.getMonths();
  t.year = rtc.getYears();
  // other fields are unused, but included in the parser for some reason
  sd.write((byte *) &t, sizeof(t));
}

/*
 * Identical to sprintf("%02d"), but leaner and restricted to
 * the last two digits of i.
 */
inline void print02d(char *s, byte i) {
  s[0] = '0' + (i % 100 / 10);
  s[1] = '0' + (i % 10);
}

/*
 * Writes the current time (as defined by the RTC) to a string as:
 *   YYYY-MM-DD_HH-MM-SS
 * The string must be long enough to this data. The RTC needs to be
 * updated before calling this.
 */
void rtc_print(char *s) {
  short y = rtc.getYears();
  s[0] = '0' + (y / 1000);
  s[1] = '0' + (y / 100 % 10);
  s[2] = '0' + (y % 100 / 10);
  s[3] = '0' + (y % 10);
  s[4] = '-';
  print02d(s + 5,  rtc.getMonths());  s[7]  = '-';
  print02d(s + 8,  rtc.getDays());    s[10] = '_';
  print02d(s + 11, rtc.getHours());   s[13] = '-';
  print02d(s + 14, rtc.getMinutes()); s[16] = '-';
  print02d(s + 17, rtc.getSeconds());
}

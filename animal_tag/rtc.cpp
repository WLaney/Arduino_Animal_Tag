#include <Arduino.h>
#include <SD.h>
#include "rtc.hpp"
#include "debug.h"
#include "DSRTCLib.h"

// Copied from ds3234.h
// TODO Try using #include, see if it bloats the code
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

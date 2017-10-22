#include <Arduino.h>
#include <SD.h>
#include "rtc.hpp"
#include "debug.h"
#include "DSRTCLib2.h"

int addr = 128; //current byte to write to in EEPROM

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
constexpr byte int_num = 0; // Interrupt number corresponding to that pin

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
 * Write raw ts to EEPROM
 */
void rtc_write_EEPROM() {
  ts t = get_time();
  EEPROM.put(addr, t);
  addr = addr + sizeof(t);
}

/*
 * Read ts from EEPROM
 */
ts rtc_read_EEPROM(int location) {
  ts readTime;
  EEPROM.get(location, readTime);
  return readTime;
}

ts get_time() {
  ts t;
  t.sec  = rtc.getSeconds();
  t.min  = rtc.getMinutes();
  t.hour = rtc.getHours();
  t.mday = rtc.getDays();
  t.mon  = rtc.getMonths();
  t.year = rtc.getYears();
  return t;
}

void print_time(ts t) {
  Serial.print(t.year); Serial.print('-');
  Serial.print(t.mon); Serial.print('-');
  Serial.print(t.mday); Serial.print(' ');
  Serial.print(t.hour); Serial.print(':');
  Serial.print(t.min); Serial.print(':');
  Serial.println(t.sec);
}

// Return difference between two times in seconds
long int time_diff() {
  ts new_time = get_time();
  ts old_time = rtc_read_EEPROM(addr - sizeof(ts));
  long int diff = new_time.sec - old_time.sec;
  diff += (new_time.min - old_time.min)   * 60;
  diff += (new_time.hour - old_time.hour) * 60 * 60;
  diff += (new_time.mday - old_time.mday) * 60 * 60 * 24;
  diff += (new_time.mon - old_time.mon)   * 60 * 60 * 24 * 31;
  diff += (new_time.year - old_time.year)  * 60 * 60 * 24 * 365;
  return diff;
}

/*
 * Put the tag to sleep for long periods of time.
 * Use n_delay for delays of under, say, 3 seconds
 */
void rtc_snooze(long int seconds) {
  rtc.snooze(seconds);
}

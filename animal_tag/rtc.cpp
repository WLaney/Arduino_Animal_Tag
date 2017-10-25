#include <Arduino.h>
#include <SD.h>
#include <EEPROM.h>
#include "rtc.hpp"
#include "debug.h"
#include "eeprom_buffer.hpp"
#include "DSRTCLib2.h"

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

ts copy_time();

// Store the last time at this address in EEPROM.
constexpr int addr = 256;

// If it takes longer than this amount of time (in seconds) to update,
// then log the time to EEPROM.
constexpr int heartbeat_max_time = 610;

// Number of skips to store; starts overriding old ones
// after it runs out of space
constexpr int heartbeat_max_skips = 5;

constexpr byte int_pin = 2; // Interrupt pin (2 or 3)
constexpr byte int_num = 0; // Interrupt number corresponding to that pin

DSRTCLib rtc(int_pin, int_num);

rom_buffer<ts> skip_buffer(addr + sizeof(ts), heartbeat_max_skips * 2);

void rtc_setup() {
  rtc.start();
}

void rtc_update() {
  DBGLN("Read RTC");
  rtc.readTime();
}

/*
   Write raw ts to the SD card
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
   Write raw ts to EEPROM
*/
void rtc_write_EEPROM() {
  ts t = copy_time();
  EEPROM.put(addr, t);
}

/*
   Read ts from EEPROM
*/
ts rtc_read_EEPROM(int location) {
  ts readTime;
  EEPROM.get(location, readTime);
  return readTime;
}

/*
   Copy time from the RTC library into a
   struct.
*/
ts copy_time() {
  ts t;
  t.sec  = rtc.getSeconds();
  t.min  = rtc.getMinutes();
  t.hour = rtc.getHours();
  t.mday = rtc.getDays();
  t.mon  = rtc.getMonths();
  t.year = rtc.getYears();
  return t;
}

/*
   Print a time to the serial port.
*/
void print_time(ts t) {
  DBG(t.year); DBG('-');
  DBG(t.mon);  DBG('-');
  DBG(t.mday); DBG(' ');
  DBG(t.hour); DBG(':');
  DBG(t.min);  DBG(':');
  DBG(t.sec);
}

/*
   Return the difference between the time
   in the RTC library and the time
   stored in EEPROM, in seconds.
*/
long int heartbeat_time_diff() {
  ts new_time = copy_time();
  ts old_time = rtc_read_EEPROM(addr);
  long int diff = new_time.sec - old_time.sec;
  diff += (new_time.min - old_time.min)   * 60;
  diff += (new_time.hour - old_time.hour) * 60 * 60;
  diff += (new_time.mday - old_time.mday) * 60 * 60 * 24;
  diff += (new_time.mon - old_time.mon)   * 60 * 60 * 24 * 31;
  diff += (new_time.year - old_time.year)  * 60 * 60 * 24 * 365;
  return diff;
}

/*
   Update the heartbeat sensor, writing the value
   to EEPROM if necessary
*/
void rtc_update_eeprom() {
  if (heartbeat_time_diff() > heartbeat_max_time) {
    skip_buffer.push(rtc_read_EEPROM(addr));
    skip_buffer.push(copy_time());
  }
  rtc_write_EEPROM();
}

bool is_time_valid(ts t) {
  return (t.sec >= 0 && t.sec <= 60)
      && (t.min >= 0 && t.min <= 60)
      && (t.hour >= 0 && t.hour < 24)
      && (t.mday > 0 && t.mday <= 31)
      && (t.mon > 0 && t.mon <= 12)
      && (t.year > 1900);
}

/*
   Print all skips to the Serial port
*/
void rtc_print_skips() {
  ts *skips = skip_buffer.read_all();
  for (int i = 0; i < skip_buffer.get_max_size(); i += 2) {
    if (is_time_valid(skips[i]) && is_time_valid(skips[i+1])) {
      print_time(skips[i]);
      DBG('\t');
      print_time(skips[i+1]);
      DBG('\n');
    }
  }
  delete[] skips;
}

/*
   Put the tag to sleep for long periods of time.
   Use n_delay for delays of under, say, 3 seconds
*/
void rtc_snooze(long int seconds) {
  rtc.snooze(seconds);
}

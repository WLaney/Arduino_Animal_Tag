#include <Arduino.h>
#include <SD.h>
#include <EEPROM.h>
#include "DSRTCLib2.h"

int addr = 128; //current byte to write to in EEPROM

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

DSRTCLib rtc(2, 0);

void setup() {
  Serial.begin(9600);
  rtc.start();
  rtc.readTime();
  ts currentTime = get_time();
  print_time(currentTime);
  delay(1000);
  // Write current time to EEPROM, then print it out
  rtc.readTime();
  rtc_write_EEPROM();
  print_time(rtc_read_EEPROM(128));
  // Write current time to EEPROM, wait 3 seconds,
  // and print the difference
  rtc.readTime();
  rtc_write_EEPROM();
  delay(3000);
  rtc.readTime();
  Serial.println(time_diff());
  // Write 3 times and read them all out
  rtc.readTime();
  rtc_write_EEPROM();
  delay(1000);
  rtc.readTime();
  rtc_write_EEPROM();
  delay(2000);
  rtc.readTime();
  rtc_write_EEPROM();
  print_time(rtc_read_EEPROM(addr - sizeof(ts)));
  print_time(rtc_read_EEPROM(addr - 2*sizeof(ts)));
  print_time(rtc_read_EEPROM(addr - 3*sizeof(ts)));
}

void loop() {
  
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

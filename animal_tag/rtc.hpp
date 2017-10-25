//rtc.h - Handles real-time clock sensor.
#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <SD.h>

void rtc_setup();
void rtc_update();
void rtc_write(File sd);
void rtc_snooze(long int time);

void rtc_update_eeprom();
// This doesn't need rtc_setup() to be run
void rtc_print_skips();

#endif

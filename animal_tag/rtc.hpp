//rtc.h - Handles real-time clock sensor.
#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <SD.h>

void rtc_setup();
void rtc_update();
void rtc_write(File sd);

#endif

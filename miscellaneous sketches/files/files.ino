#include <DSRTCLib.h>

char *filename = "YYYY-MM-DD_HH:MM:SS.SRK";

DSRTCLib rtc;

// Set the filename based on the date and time
void setup() {
	Serial.begin(9600);
	rtc.start();
	
	rtc.readTime();
	writeTime(rtc, filename);
	Serial.println(filename);
}

void loop() {
	
}

/*
 * Identical to sprintf("%02d"), but leaner and restricted to
 * the last two digits of the integer.
 */
inline void print02d(char *s, byte i) {
  s[0] = '0' + (i % 100 / 10);
  s[1] = '0' + (i % 10);
}

/*
 * Writes the current time (as defined by the RTC) to a string as:
 *   YYYY-MM-DD_HH:MM:SS
 * Obviously, the string must be long enough to this data.
 */
void writeTime(DSRTCLib rtc, char *c) {
  rtc.readTime();
  short y = rtc.getYears();
  c[0] = '0' + (y / 1000);
  c[1] = '0' + (y / 100 % 10);
  c[2] = '0' + (y % 100 / 10);
  c[3] = '0' + (y % 10);
  print02d(c + 5,  rtc.getMonths());
  print02d(c + 8,  rtc.getDays());
  print02d(c + 11, rtc.getHours());
  print02d(c + 14, rtc.getMinutes());
  print02d(c + 17, rtc.getSeconds());
}


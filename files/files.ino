#include <DSRTCLib.h>

char *filename = "XX-XX-XX_XX:XX:XX_DATA.SRK";

DSRTCLib rtc;

// Identical to printf("%02d"), but with strings
inline void print02d(char *s, int i) {
	s[0] = '0' + (char) (i % 100) / 10;
	s[1] = '0' + (char) (i % 10);
}

// Set the filename based on the date and time
void setup() {
	Serial.begin(9600);
	rtc.start();
	
	rtc.readTime();
	print02d(filename,    rtc.getYears());
	print02d(filename+3,  rtc.getMonths());
	print02d(filename+6,  rtc.getDays());
  print02d(filename+9,  rtc.getHours());
  print02d(filename+12, rtc.getMinutes());
  print02d(filename+15, rtc.getSeconds());
	
	Serial.println(filename);
}

void loop() {
	
}

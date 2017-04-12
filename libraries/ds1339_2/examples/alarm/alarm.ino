/*
 * Personal test to see if I understand alarms...
 */
#include <DSRTCLib2.h>
#include <avr/sleep.h>
const int INT_PIN = 2;

DSRTCLib rtc(INT_PIN, digitalPinToInterrupt(INT_PIN));

void setup() {
	pinMode(INT_PIN, INPUT);
	
	// Enable sleeping
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	
	Serial.begin(9600);
	rtc.start();
}

void loop() {
	rtc.snooze(2);
	// On wakeup, print message
	Serial.println(F("Hi!"));
}

void wakeup() {
	// Empty
}

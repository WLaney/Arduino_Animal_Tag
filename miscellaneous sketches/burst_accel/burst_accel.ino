/**
 * Test accelerometer burst-reading/writing sketch.
 */
#include "accel.hpp"
#include "debug.h"
#include <avr/sleep.h>

void setup() {
	
	//DBEGIN();
	Serial.begin(38400);
	bool accel_good = accel_setup(Accel::Range::G2,
	                              Accel::ODR::HZ_50,
	                              false);
	if (!accel_good) {
		DBGSTR("Accelerometer failed to initialize.\n");
		while (true)
			;
	}
	pinMode(3, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(3), wakeUp, FALLING);
}

void loop() {
	// Go to sleep
	Serial.end();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_mode();
	sleep_disable();
	Serial.begin(38400);
	
	if (accel_full()) {
		accel_print_all();
		accel_reset();
	} else {
		accel_read_all();
	}

}

void wakeUp() {
	
}

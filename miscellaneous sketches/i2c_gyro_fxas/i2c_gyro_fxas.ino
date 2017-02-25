/*
 * A test sketch that tries to use the FXAS gyroscope alongside a new
 * I2C library. Compare the gains! ... If there are any.
 */
#include "fxas_2.h"

#define PRINTSTR(s) Serial.print(F(s))

void setup() {
	Serial.begin(9600);
	bool start = FXAS2::begin(FXAS2::ODR::HZ_25,
	                          FXAS2::Range::DPS_250,
	                          false);
	if (start) {
		PRINTSTR("Startup successful!");
	} else {
		PRINTSTR("Couldn't find the gyroscope.");
		while (true)
			;
	}
}

void loop() {
	FXAS2::sample s;
	FXAS2::read(s);
	PRINTSTR("X: ");     Serial.print(FXAS2::sampleToDps(s.x));
	PRINTSTR("\tY: ");   Serial.print(FXAS2::sampleToDps(s.y));
	PRINTSTR("\tZ: "); Serial.println(FXAS2::sampleToDps(s.z));
	delay(80);
}

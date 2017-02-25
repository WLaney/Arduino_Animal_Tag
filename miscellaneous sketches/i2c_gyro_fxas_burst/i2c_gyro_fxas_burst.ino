/*
 * A test sketch that tries to use the FXAS gyroscope alongside a new
 * I2C library. Compare the gains! ... If there are any.
 */
#include "fxas_2.h"

#define PRINTSTR(s) Serial.print(F(s))

FXAS2::sample buffer[32];
long timeToPrint = 0;

void setup() {
	Serial.begin(9600);
	bool start = FXAS2::begin(FXAS2::ODR::HZ_25,
	                          FXAS2::Range::DPS_250,
	                          true);
	if (start) {
		PRINTSTR("Startup successful!");
	} else {
		PRINTSTR("Couldn't find the gyroscope.");
		while (true)
			;
	}
}

void loop() {
	long timeToRead;
	
	delay(40 * 32 - timeToPrint + 10);
	
	timeToRead = millis();
	FXAS2::readBurst(buffer, 32);
	timeToRead = millis() - timeToRead;
	
	timeToPrint = millis();
	for (auto &s : buffer) {
		PRINTSTR("X: ");     Serial.print(FXAS2::sampleToDps(s.x));
		PRINTSTR("\tY: ");   Serial.print(FXAS2::sampleToDps(s.y));
		PRINTSTR("\tZ: "); Serial.println(FXAS2::sampleToDps(s.z));
	}
	PRINTSTR("Read time (ms): ");
	Serial.println(timeToRead);
	timeToPrint = millis() - timeToPrint;
}

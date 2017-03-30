/*
 * Test the gyro's burst-reading.
 */
#include "fxas_2.h"

using namespace FXAS2;

constexpr byte bmax = bufferSize;
sample buffer[bmax];

void setup() {
	Serial.begin(9600);
	bool found = begin(ODR::HZ_12_5,
					   Range::DPS_250,
					   true);
	if (!found) {
		Serial.println(F("Gyroscope not found."));
		while (true)
			;
	}
}

void loop() {
	long time;
	delay(80 * bmax);

	Serial.println(F("Reading..."));
	time = millis();
	readBurst(buffer, bmax);
	time = millis() - time;

	for (int i = 0; i < bmax; i++) {
		sample s = buffer[i];
		// Serial.print(F("X: "));   Serial.print(s.x, HEX);
		// Serial.print(F("\tY: ")); Serial.print(s.y, HEX);
		// Serial.print(F("\tZ: ")); Serial.println(s.z, HEX);
		Serial.print(F("X: "));   Serial.print(sampleToDps(s.x));
		Serial.print(F("\tY: ")); Serial.print(sampleToDps(s.y));
		Serial.print(F("\tZ: ")); Serial.println(sampleToDps(s.z));
	}
	Serial.print(F("Read in "));
	Serial.print(time);
	Serial.println(F("ms"));
	
	for (int i = 0; i < bmax; i++) {
		buffer[i] = {0xDEAD, 0xDEAD, 0xDEAD};
	}
}

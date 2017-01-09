/*
 * Test the gyro's burst-reading.
 */
#include "Gyro_FXAS.h"

using namespace FXAS;

constexpr byte bmax = burst_buffer_max;
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
		// Floats
		Serial.print(F("X: "));   Serial.print(s2f(s.x));
		Serial.print(F("\tY: ")); Serial.print(s2f(s.y));
		Serial.print(F("\tZ: ")); Serial.println(s2f(s.z));
	}
	Serial.print(F("Read in "));
	Serial.print(time);
	Serial.println(F("ms"));
	
	for (int i = 0; i < bmax; i++) {
		buffer[i] = {-1, -1, -1};
	}
}

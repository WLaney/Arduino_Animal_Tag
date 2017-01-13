/*
 * Test the gyro's burst-reading.
 */
#include "Gyro_FXAS.h"

using namespace FXAS;

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

inline void print_abs(short s) {
	Serial.print((s < 0 ? -s : s), HEX);
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
		Serial.print(F("X: "));   print_abs(s.x);
		Serial.print(F("\tY: ")); print_abs(s.y);
		Serial.print(F("\tZ: ")); print_abs(s.z);
		Serial.println();
	}
	Serial.print(F("Read in "));
	Serial.print(time);
	Serial.println(F("ms"));
	
	for (int i = 0; i < bmax; i++) {
		buffer[i] = {0, 0, 0};
	}
}

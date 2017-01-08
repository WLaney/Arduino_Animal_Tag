/*
 * Try to start the FXAS gyro, return if successful.
 */
#include "Gyro_FXAS.h"

#define PRINTLNS(s) Serial.println(F(s)) 

void setup() {
	Serial.begin(9600);
	if (FXAS::begin(FXAS::ODR::HZ_12_5, FXAS::Range::DPS_250, false)) {
		PRINTLNS("Gyro connected successfully!");
	} else {
		PRINTLNS("Gyro could not connect.");
	}
}

void loop() {
	FXAS::sample s;
	FXAS::read(s);
	Serial.print("X: ");  Serial.print(FXAS::s2f(s.x), 4);
	Serial.print(" Y: "); Serial.print(FXAS::s2f(s.y), 4);
	Serial.print(" Z: "); Serial.println(FXAS::s2f(s.z), 4);
	delay(80);
}

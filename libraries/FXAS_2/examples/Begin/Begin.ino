/*
 * Try to start the FXAS gyro, return if successful.
 */
#include "fxas_2.h"

#define PRINTLNS(s) Serial.println(F(s)) 

void setup() {
	Serial.begin(9600);
	if (FXAS2::begin(FXAS2::ODR::HZ_12_5, FXAS2::Range::DPS_250, false)) {
		PRINTLNS("Gyro connected successfully!");
	} else {
		PRINTLNS("Gyro could not connect.");
	}
}

void loop() {
	FXAS2::sample s;
	FXAS2::read(s);
	Serial.print("X: ");  Serial.print(s.x);
	Serial.print(" Y: "); Serial.print(s.y);
	Serial.print(" Z: "); Serial.println(s.z);
	delay(80);
}

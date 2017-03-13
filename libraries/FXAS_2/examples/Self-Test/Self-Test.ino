/*
 * Run the FXAS's self-test function.
 */
#include "fxas_2.h"
#include <math.h>

#define PRINTLNS(s) Serial.println(F(s)) 

void setup() {
	Serial.begin(9600);
	if (FXAS2::beginSelfTest()) {
		PRINTLNS("Gyro connected successfully!");
	} else {
		PRINTLNS("Gyro could not connect.");
	}
}

void loop() {
	FXAS2::sample s;
	FXAS2::read(s);
	Serial.print("X: ");  Serial.print(abs(s.x));
	Serial.print(" Y: "); Serial.print(abs(s.y));
	Serial.print(" Z: "); Serial.println(abs(s.z));
	delay(80);
}

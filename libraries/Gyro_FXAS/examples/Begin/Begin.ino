/*
 * Try to start the FXAS gyro, return if successful.
 */
#include <Gyro_FXAS.h>

#define PRINTLNS(s) Serial.println(F(s)) 

void setup() {
	Serial.begin(9600);
	if (FXAS::begin()) {
		PRINTLNS("Gyro connected successfully!");
	} else {
		PRINTLNS("Gyro could not connect.");
	}
}

void loop() {
	
}

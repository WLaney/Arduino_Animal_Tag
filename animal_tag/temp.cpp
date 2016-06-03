#include "temp.hpp"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>
#include <Wire.h>

#define TMP102ADDRESS 0x48

// pretty obvious
float celsius;

void temp_setup() {}

void temp_update() {
    DBGLN("Read temp");
    Wire.requestFrom(TMP102ADDRESS, 2);
    byte MSB = Wire.read();
    byte LSB = Wire.read();
    int temperatureSum = ((MSB << 8) | LSB) >> 4;
    celsius = temperatureSum * 0.0625;
}

void temp_write(File sd) {
	DBGLN("Wrote temp");
	sd.print(celsius);
}

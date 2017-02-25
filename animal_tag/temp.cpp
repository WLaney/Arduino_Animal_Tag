#include "temp.hpp"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>
#include <I2C.h>

#define TMP102ADDRESS 0x48

float celsius;

void temp_setup() {
    Wire.begin();
}

void temp_update() {
    short temperatureSum;
    byte b[2];
    
    DBGSTR("Read temp\n");
    I2c.read(TMP102ADDRESS, 2, b);
    temperatureSum = ((b[0] << 8) | b[1]) >> 4;
    celsius = temperatureSum * 0.0625;
}

void temp_write(File sd) {
	DBGSTR("Wrote temp\n");
	sd.write((byte *) &celsius,4);
}

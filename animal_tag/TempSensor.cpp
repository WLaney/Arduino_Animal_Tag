#include "TempSensor.hpp"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>
#include <Wire.h>

void TempSensor::setup() { }

void TempSensor::read() {
    DBGLN("Read temp");
    Wire.requestFrom(tmp102Address, 2);
    byte MSB = Wire.read();
    byte LSB = Wire.read();
    int temperatureSum = ((MSB << 8) | LSB) >> 4;
    celsius = temperatureSum * 0.0625;
}

void TempSensor::flush(File sd) {
    DBGLN("Wrote temp");
    sd.print(celsius);
}

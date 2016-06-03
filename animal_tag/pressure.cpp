//#include "pressure.hpp"
//#include <Arduino.h>
//#include <SD.h>
//#include "SparkFun_MS5803_I2C.h"
//#include "debug.h"
//
//// Not sure of the difference between ADDRESS_LOW and
//// ADDRESS_HIGH; look this up later
//MS5803 sensor(ADDRESS_HIGH);
//
//float pressure;
//
//void pressure_setup() {
//  DBGLN("Setup pressure");
//  sensor.reset();
//  sensor.begin();
//}
//
//void pressure_update() {
//  DBGLN("Update pressure");
//  // Pressure is in units of 1 millibar.
//  // Could we store this in a more compact data structure?
//  // (Maximum water pressure of the Chesapeake Bay)
//  pressure = sensor.getPressure(ADC_256);
//}
//
//void pressure_write(File sd) {
//  DBGLN("Write pressure");
//  Serial.print(pressure);
//}

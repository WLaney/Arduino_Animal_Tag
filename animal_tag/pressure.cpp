//#include "pressure.hpp"
//#include <Arduino.h>
//#include <SD.h>
//#include "SparkFun_MS5803_I2C.h"
//#include "debug.h"
//
//// ADDRESS_LOW refers to the i2c address being
//// "pulled low"; ask William for more details
//MS5803 sensor(ADDRESS_LOW);
//
//// Pressure is in units of 1 millibar
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
//  pressure = sensor.getPressure(ADC_256);
//}
//
//void pressure_write(File sd) {
//  DBGLN("Write pressure");
//  Serial.print(pressure);
//}

// accel.hpp - Accelerometer access and buffering
#ifndef __ACCEL_HPP__
#define __ACCEL_HPP__
#include <Arduino.h>
#include <SD.h>

void accel_setup();
void accel_read();
bool accel_full();
byte accel_size();
void accel_write(File, byte);
void accel_reset();

#endif

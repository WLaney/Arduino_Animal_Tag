// accel.hpp - Accelerometer access and buffering
#ifndef __ACCEL_HPP__
#define __ACCEL_HPP__
#include <Arduino.h>
#include <SD.h>
#include <SFE_MMA8452Q.h>

void accel_setup(MMA8452Q_Scale, MMA8452Q_ODR);
void accel_read();
bool accel_full();
byte accel_size();
unsigned short accel_write_size();
float accel_scale();
void accel_write(File);
void accel_reset();

#endif

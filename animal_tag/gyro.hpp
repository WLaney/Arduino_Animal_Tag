#ifndef __GYRO_HPP__
#define __GYRO_HPP__

#include <Arduino.h>
#include <SD.h>
void gyro_setup();
void gyro_read_all();
byte gyro_size();
unsigned short gyro_write_size();
float gyro_scale();
void gyro_write(File);
void gyro_reset();

#endif

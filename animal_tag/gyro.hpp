#ifndef __GYRO_HPP__
#define __GYRO_HPP__

#include <Arduino.h>
#include <SD.h>
void gyro_setup();
void gyro_read_all();
byte gyro_size();
size_t gyro_write_size();
void gyro_write(File);
void gyro_reset();

#endif

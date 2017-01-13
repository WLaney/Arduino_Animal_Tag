#ifndef __GYRO_HPP__
#define __GYRO_HPP__

#include <Arduino.h>
#include <SD.h>

// Size of software buffer (samples)
constexpr byte gyro_buffer_size = 16;
// Size of hardware buffer (samples)
constexpr byte gyro_fifo_size = 32;

void gyro_setup();
void gyro_read_all();
void gyro_write(File);
void gyro_reset();

byte gyro_size();
unsigned short gyro_write_size();
float gyro_scale();

#endif

#include "gyro.hpp"
#include <Arduino.h>
#include <SD.h>
#include <Light_L3GD20.h>
#include "debug.h"

#define GYRO_BUFFER_SIZE 16

typedef Gyro::l3gd20Data_t gyro_data;

static gyro_data buffer[GYRO_BUFFER_SIZE];

void gyro_setup() {
  DBGSTR("Gyroscope buffer size: ");
  DBG(GYRO_BUFFER_SIZE);
  DBGSTR(" (+32 in hardware)\n");
	Gyro::begin();
}

void gyro_read_all() {
  DBGSTR("G-READ\n");
	Gyro::fifo_burst_read(buffer, GYRO_BUFFER_SIZE);
}

byte gyro_size() {
	return GYRO_BUFFER_SIZE;
}

void gyro_write(File sd, byte i) {
	gyro_data &d = buffer[i];
  float cx, cy, cz;
  cx = Gyro::s2f(d.x);
  cy = Gyro::s2f(d.y);
  cz = Gyro::s2f(d.z);

  sd.print(cx); sd.print('\t');
  sd.print(cy); sd.print('\t');
  sd.print(cz);
}

void gyro_reset() { }

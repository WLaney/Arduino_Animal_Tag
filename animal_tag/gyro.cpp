#include "gyro.hpp"
#include <Arduino.h>
#include <SD.h>
#include <Light_L3GD20.h>
#include "debug.h"

constexpr byte gyro_buffer_size = 16;
constexpr byte gyro_fifo_size = 32;

typedef Gyro::l3gd20Data_t gyro_data;

static gyro_data buffer[gyro_buffer_size];

void gyro_setup() {
  DBGSTR("Gyroscope buffer size: ");
  DBG(gyro_buffer_size);
  DBGSTR(" (+32 in hardware)\n");
	Gyro::begin();
}

void gyro_read_all() {
  DBGSTR("G-READ\n");
	Gyro::fifo_burst_read(buffer, gyro_buffer_size);
}

byte gyro_size() {
	return gyro_buffer_size;
}

/*
 * Print every value in the software and hardware buffer.
 * Currently, this leads to some data loss...
 */
void gyro_write(File sd) {
  for (byte i = 0; i < gyro_fifo_size + gyro_buffer_size; i += gyro_buffer_size) {
    for (byte j = 0; j < gyro_buffer_size; j++) {
      gyro_data &d = buffer[j];
      float cx, cy, cz;
      cx = Gyro::s2f(d.x);
      cy = Gyro::s2f(d.y);
      cz = Gyro::s2f(d.z);
    
      sd.print(cx); sd.print('\t');
      sd.print(cy); sd.print('\t');
      sd.println(cz);
    }
    // I think we're currently doing a spurious read here; be cautious
    gyro_read_all();
  }
}

void gyro_reset() {
  
}

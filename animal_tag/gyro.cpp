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
  DBG(gyro_buffer_size + gyro_fifo_size);
  DBGSTR(" (software + hardware)\n");
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
 * Write every value in the software and hardware buffer to the SD card.
 */
void gyro_write(File sd) {
  DBGSTR("Gyro write\n");
  for (byte i = 0; i < gyro_fifo_size + gyro_buffer_size; i += gyro_buffer_size) {
    sd.write((byte *) buffer, gyro_buffer_size * sizeof(gyro_data));
    // I think we're currently doing a spurious read here; be cautious
    gyro_read_all();
  }
}

void gyro_reset() {
  
}

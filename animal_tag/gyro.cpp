#include "gyro.hpp"
#include <Arduino.h>
#include <SD.h>
#include "Gyro_FXAS.h"
#include "debug.h"

static FXAS::sample buffer[gyro_buffer_size];

/*
 * Setup the gyroscope's internal buffer, do some
 * debugging reports
 */
void gyro_setup() {
  DBGSTR("Gyroscope buffer size: ");
  DBG(gyro_buffer_size + gyro_fifo_size);
  DBGSTR(" (software + hardware)\n");
  FXAS::begin(FXAS::ODR::HZ_25,
              FXAS::Range::DPS_250,
			  true); // enable burst-reading
}

/*
 * Read as many bytes as possible from the gyroscope's FIFO
 * into our buffer
 */
void gyro_read_all() {
  DBGSTR("G-READ\n");
  FXAS::readBurst(buffer, gyro_buffer_size);
}

/*
 * Size of the gyroscope's internal buffer
 */
byte gyro_size() {
	return gyro_buffer_size;
}

/*
 * Size, in bytes, of a single SD card write.
 */
unsigned short gyro_write_size() {
  return (gyro_buffer_size + gyro_fifo_size) * sizeof(FXAS::sample);
}

/*
 * Write every value in the software and hardware buffer to the SD card.
 */
void gyro_write(File sd) {
  DBGSTR("Gyro write\n");
  for (byte i = 0; i < gyro_fifo_size + gyro_buffer_size; i += gyro_buffer_size) {
    sd.write((byte *) buffer, gyro_buffer_size * sizeof(FXAS::sample));
    // I think we're currently doing a spurious read here; be cautious
    gyro_read_all();
  }
}

/*
 * Return the gyroscope's scale. This is written to the header
 */
float gyro_scale() {
  switch (FXAS::currentRange) {
    case FXAS::Range::DPS_2000: return 2000.0; break;
    case FXAS::Range::DPS_1000: return 1000.0; break;
    case FXAS::Range::DPS_500:  return  500.0; break;
    case FXAS::Range::DPS_250:  return  250.0; break;
  }
}

/*
 * Reset the gyro buffer so that it can be used again. This is currently
 * not used, but may be later
 */
void gyro_reset() {
  
}

/*
 * Set the gyroscope to active or standby mode.
 */
void gyro_set_active(bool active) {
	if (active) {
		FXAS::active();
	} else {
		FXAS::standby();
	}
}

/*
 * Get the gyroscope's current status: active (true) or standy.
 */
bool gyro_is_active() {
	return FXAS::isActive;
}

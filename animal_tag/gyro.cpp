#include "gyro.hpp"
#include <Arduino.h>
#include <SD.h>
#include "fxas_2.h"
#include "debug.h"

static FXAS2::sample buffer[gyro_buffer_size];

/*
 * Setup the gyroscope's internal buffer, do some
 * debugging reports
 */
void gyro_setup(FXAS2::Range range, FXAS2::ODR odr) {
  DBGSTR("Gyroscope buffer size: ");
  DBG(gyro_buffer_size + gyro_fifo_size);
  DBGSTR(" (software + hardware)\n");
  if (range > FXAS2::Range::DPS_250) {
    DBGSTR("ERROR: INVALID GYROSCOPE RANGE");
  }
  FXAS2::begin(odr, range, true); // enable burst-reading
}

/*
 * Read as many bytes as possible from the gyroscope's FIFO
 * into our buffer
 */
void gyro_read_all() {
  if (gyro_is_active) {
    DBGSTR("G-READ\n");
    FXAS2::readBurst(buffer, gyro_buffer_size);
  } else {
    DBGSTR("ERROR: inactive gyroscope read!\n");
  }
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
  return (gyro_buffer_size + gyro_fifo_size) * sizeof(FXAS2::sample);
}

/*
 * Write every value in the software and hardware buffer to the SD card.
 */
void gyro_write(File sd) {
  if (gyro_is_active) {
    DBGSTR("Gyro write\n");
    for (byte i = 0; i < gyro_fifo_size + gyro_buffer_size; i += gyro_buffer_size) {
      sd.write((byte *) buffer, gyro_buffer_size * sizeof(FXAS2::sample));
      // I think we're currently doing a spurious read here; be cautious
      gyro_read_all();
    }
  } else {
    DBGSTR("ERROR: inactive gyro write!\n");
  }
}

/*
 * Return the gyroscope's scale. This is written to the header
 */
float gyro_scale() {
  switch (FXAS2::currentRange) {
    case FXAS2::Range::DPS_2000: return 2000.0; break;
    case FXAS2::Range::DPS_1000: return 1000.0; break;
    case FXAS2::Range::DPS_500:  return  500.0; break;
    case FXAS2::Range::DPS_250:  return  250.0; break;
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
		FXAS2::active();
	} else {
		FXAS2::standby();
	}
}

/*
 * Get the gyroscope's current status: active (true) or standy.
 */
bool gyro_is_active() {
	return FXAS2::isActive;
}

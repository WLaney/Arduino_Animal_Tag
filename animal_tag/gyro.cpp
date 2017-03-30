#include "gyro.hpp"
#include <Arduino.h>
#include <SD.h>
#include "fxas_2.h"
#include "debug.h"

// Size of software buffer (samples)
constexpr signed char buffer_s = 32;
// Size of hardware buffer (samples)
constexpr signed char buffer_h = 32;

static FXAS2::sample buffer[buffer_s];
static signed char buffer_i;

/*
 * Setup the gyroscope's internal buffer, do some
 * debugging reports
 */
void gyro_setup(FXAS2::Range range, FXAS2::ODR odr) {
  DBGSTR("Gyroscope buffer size: ");
  DBG(buffer_s + buffer_h);
  DBGSTR(" (software + hardware)\n");
  FXAS2::begin(odr, range, true); // enable burst-reading
}

/*
 * Read as many bytes as possible from the gyroscope's FIFO
 * into our buffer.
 */
void gyro_read_all() {
	if (!gyro_is_active()) {
		DBGSTR("ERROR: G. not active\n");
		return;
	}
	if (buffer_i >= buffer_s) {
		DBGSTR("ERROR: G.FULL\n");
		return;
	}
	DBGSTR("G.read\n");
	byte reads_left = buffer_s - buffer_i;
	byte reads = (reads_left < 32) ? reads_left : 32;
	
	FXAS2::readBurst(buffer, reads);
	buffer_i += reads;
}

/*
 * Write every value in the software and hardware buffer to the SD card.
 * If half is true, only write half of the buffer. (Used with downsampled accelerometer)
 */
void gyro_write(File sd, bool half) {
	if (!gyro_is_active()) {
		DBGSTR("ERROR: G. not active");
	}
	// Write software buffer
	sd.write((byte *) buffer, sizeof(buffer));
	// Write hardware buffer
	gyro_reset();
	gyro_read_all();
	sd.write((byte *) buffer, sizeof(buffer) >> half);
}

/*
 * Get the software buffer's fullness
 */
bool gyro_full() {
	return buffer_i >= buffer_s;
}

/*
 * Reset the gyro buffer so that it can be used again. This is currently
 * not used, but may be later
 */
void gyro_reset() {
  buffer_i = 0;
#if CLEAR_BUFFERS
  for (int i=0; i<buffer_s; i++) {
    buffer[i] = {0, 0, 0};
  }
#endif
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
 * Size, in bytes, of a single SD card write.
 */
unsigned short gyro_write_size() {
  return (buffer_s + buffer_h) * sizeof(FXAS2::sample);
}

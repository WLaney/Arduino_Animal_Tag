/**
 * Handles reading from and storing accelerometer data.
 * This is currently a wrapper for a Packed Buffer, which
 * means that I should either move the code into animal_tag.ino
 * or the SFE code into this file.
 */
#include "accel.hpp"
#include "debug.h"
#include <Accel_1Q_2.h>
#include <Arduino.h>
#include <SD.h>

// The number of samples in the accelerometer's FIFO buffer. Don't change this.
constexpr signed char buffer_h = 32;
// The number of samples in our software buffer. This can be changed,
// but it affects RAM consumption and read/write frequency.
constexpr signed char buffer_s = 32;

static Accel::sample_raw buffer[buffer_s];
static signed char buffer_i;
static bool downscale = false;
static float scale = 8.0;

bool accel_setup(Accel::Range rng, Accel::ODR odr, bool down) {
	DBGSTR("A.buffer (soft & hardware): "); DBGLN(buffer_s + buffer_h);
	bool good = Accel::begin_standby(odr, rng);
	if (!good)
		return false;
	
	downscale = down;
	Accel::set_fifo(Accel::FIFO_Mode::FILL);
	//Accel::set_fifo_interrupt(true);
	Accel::set_active(true);
	accel_reset();
	switch (rng) {
		case Accel::Range::G2: scale = 2.0F; break;
		case Accel::Range::G4: scale = 4.0F; break;
		case Accel::Range::G8: scale = 8.0F; break;
		default:
		DBGSTR("ERROR: A.SCALE INVALID\n");
	}
	return true;
}

void accel_read_all() {
	if (buffer_i >= buffer_s) {
		DBGSTR("ERROR: A.FULL\n");
		return;
	}
	DBGSTR("A.read\n");
	byte reads_max  = 32 >> downscale;
	byte reads_left = buffer_s - buffer_i;
	byte reads = (reads_left < reads_max) ? reads_left : reads_max;
	
	if (downscale) {
		Accel::read_burst_dsmp(&buffer[buffer_i], reads);
	} else {
		Accel::read_burst(&buffer[buffer_i], reads);
	}
	
	buffer_i += reads;
}

unsigned short accel_write_size() {
	byte h = buffer_h >> downscale;
	return sizeof(Accel::sample_raw) * (buffer_s + h);
}

// Write raw data to the SD card
// Assumes that the software and hardware buffers are both full
void accel_write(File sd) {
	// write software buffer first, since it's the oldest
	sd.write((byte *) buffer, sizeof(buffer));
	// write the hardware buffer next
	accel_reset();
	accel_read_all();
	sd.write((byte *) buffer, sizeof(buffer) >> downscale);
}

void accel_reset() {
	buffer_i = 0;
#if CLEAR_BUFFERS
	for (int i=0; i<buffer_s; i++) {
		buffer[i] = {0, 0, 0};
	}
#endif
}

int accel_n_samples() {
	return Accel::fifo_n_samples();
}

bool accel_full() {
	return buffer_i >= buffer_s;
}

float accel_scale() {
	return scale;
}

bool accel_downscaled() {
	return downscale;
}

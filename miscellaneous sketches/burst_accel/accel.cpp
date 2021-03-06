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
constexpr signed char buffer_s = 64;

static Accel::sample_raw buffer[buffer_s];
static byte buffer_i;
static bool downscale = false;
static float scale = 8.0;

bool accel_setup(Accel::Range rng, Accel::ODR odr, bool down) {
	DBGSTR("A.buffer (soft & hardware): "); DBGLN(buffer_s + buffer_h);
	bool good = Accel::begin_standby(odr, rng);
	if (!good)
		return false;
	
	downscale = down;
	Accel::set_fifo(Accel::FIFO_Mode::FILL);
	Accel::set_fifo_interrupt(true);
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
	byte reads_max  = (downscale ? 16 : 32);
	byte reads_left = buffer_s - buffer_i;
	byte reads = (reads_left < reads_max) ? reads_left : reads_max;
	
	if (downscale) {
		Accel::read_burst_dsmp(&buffer[buffer_i], reads / 2);
	} else {
		Accel::read_burst(&buffer[buffer_i], reads);
	}
	
	buffer_i += reads;
	
}

bool accel_full() {
	return buffer_i >= buffer_s;
}

float accel_scale() {
	return scale;
}

unsigned short accel_write_size() {
	byte h = downscale ? buffer_h / 2 : buffer_h;
	return sizeof(Accel::sample_raw) * (buffer_s + h);
}

// Write raw data
void accel_write(File sd) {
	sd.write((byte *) buffer, sizeof(buffer));
	accel_reset();
	signed char left = (downscale ? buffer_h / 2 : buffer_h);
	while (left > 0) {
		left -= buffer_s;
		accel_read_all();
		sd.write((byte *) buffer, sizeof(buffer));
		accel_reset();
	}
}

void accel_reset() {
	buffer_i = 0;
	for (int i=0; i<buffer_s; i++) {
		buffer[i] = {0, 0, 0};
	}
}

/*
 * Simulates writing data to the SD card
 */
void accel_print_all() {
	byte reads = 0;
	Accel::sample s;
	for (auto &sr : buffer) {
		s = Accel::parse_raw(sr);
		DBG(s.x); DBG('\t');
		DBG(s.y); DBG('\t');
		DBGLN(s.z);
	}
	buffer_i = 0;
	signed char left = (downscale ? buffer_h / 2 : buffer_h);
	while (left > 0) {
		left -= buffer_s;
		accel_read_all();
		for (auto &sr : buffer) {
			s = Accel::parse_raw(sr);
			DBG(s.x); DBG('\t');
			DBG(s.y); DBG('\t');
			DBGLN(s.z);
		}
		reads++;
		accel_reset();
	}
	DBGSTR("Reads: "); DBGLN(reads);
}

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

constexpr int buffer_s = 16;
constexpr int buffer_h = 32;
static Accel::sample_raw buffer[buffer_s];
static byte buffer_i;

float scale = 8.0;

void accel_setup(Accel::Range rng, Accel::ODR odr) {
	DBGSTR("A.buffer (soft & hardware): "); DBGLN(buffer_s + buffer_h);
	Accel::begin_standby(odr, rng);
	Accel::set_fifo(true);
	Accel::set_active(true);
	accel_reset();
	switch (rng) {
		case Accel::Range::G2: scale = 2.0F; break;
		case Accel::Range::G4: scale = 4.0F; break;
		case Accel::Range::G8: scale = 8.0F; break;
		default:
		DBGSTR("ERROR: A.SCALE INVALID\n");
	}
}

void read_all() {

}

unsigned short accel_write_size() {
	return sizeof(buffer);
}

float accel_scale() {
	return scale;
}

// Write raw data
void accel_write(File sd) {
	DBGSTR("a.write\n");
	sd.write((byte *) buffer, sizeof(buffer));
}

void accel_reset() {
	buffer_i = 0;
}

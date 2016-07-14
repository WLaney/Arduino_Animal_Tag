/**
 * Handles reading from and storing accelerometer data.
 * This is currently a wrapper for a Packed Buffer, which
 * means that I should either move the code into animal_tag.ino
 * or the SFE code into this file.
 */
#include "accel.hpp"
#include "debug.h"
#include "PackedBuffer.h"
#include <Arduino.h>
#include <SD.h>
#include <SFE_MMA8452Q.h>

// Number of accelerometer "packs" of two reads.
constexpr int accel_buffer_packs = 24;
constexpr float scale = 8.0;

static MMA8452Q accel;
// specialization defined in PackedBuffer.cpp
PackedBuffer<24> buffer;

void accel_setup() {
  DBGSTR("Accelerometer buffer: ");
  DBG(accel_buffer_packs);
  DBGSTR(" (x2)\n");
	accel.init(SCALE_8G, ODR_12);
}

void accel_read() {
  accel.read();
  if (!buffer.push(accel.x, accel.y, accel.z)) {
    DBGSTR("ERROR: ACCEL FULL");
  }
  DBGSTR("a.read\n");
}

bool accel_full() {
  return buffer.full();
}

inline float s2f(short s) {
  return (float) s / (float)(1<<11) * scale;
}

unsigned short accel_write_size() {
  return buffer.write_size();
}

float accel_scale() {
  return scale;
}

// write data as "{x}\t{y}\t{z}\n"
void accel_write(File sd) {
  DBGSTR("Accel write\n");
  buffer.write_all(sd);
}

void accel_reset() {
  buffer.reset();
}

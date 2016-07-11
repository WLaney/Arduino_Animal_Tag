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

constexpr int accel_buffer_size = 48;
constexpr float scale = 8.0;

static MMA8452Q accel;

typedef struct {
	short x, y, z;
} accel_data;

static accel_data buffer[accel_buffer_size];
static byte buffer_i=0;

void accel_setup() {
  DBGSTR("Accelerometer buffer: ");
  DBGLN(accel_buffer_size);
	accel.init(SCALE_8G, ODR_12);
}

void accel_read() {
  if (accel_full()) {
    DBGSTR("ERROR: ACCEL FULL");
  }
  DBGSTR("a.read\n");
	accel.read();
	accel_data &d = buffer[buffer_i];
	d.x = accel.x;
	d.y = accel.y;
	d.z = accel.z;
	buffer_i++;
}

bool accel_full() {
	return buffer_i == accel_buffer_size;
}

byte accel_size() {
	return accel_buffer_size;
}

inline float s2f(short s) {
  return (float) s / (float)(1<<11) * scale;
}

// write data as "{x}\t{y}\t{z}\n"
void accel_write(File sd) {
  for (byte i = 0; i < accel_size(); i++) {
    float cx, cy, cz;
    accel_data &d = buffer[i];
    cx = s2f(d.x);
    cy = s2f(d.y);
    cz = s2f(d.z);
    
    sd.print(cx, 3); sd.write('\t');
    sd.print(cy, 3); sd.write('\t');
    sd.println(cz, 3);
  }
}

void accel_reset() {
	buffer_i = 0;
}

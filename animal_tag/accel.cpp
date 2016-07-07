#include "accel.hpp"
#include <Arduino.h>
#include <SD.h>
#include <SFE_MMA8452Q.h>
#include "debug.h"

#define ACCEL_BUFFER_SIZE 48
#define SCALE 8.0

static MMA8452Q accel;

typedef struct {
	short x, y, z;
} accel_data;

static accel_data buffer[ACCEL_BUFFER_SIZE];
static byte buffer_i=0;

void accel_setup() {
	accel.init(SCALE_8G, ODR_12);
}

void accel_read() {
  DBGSTR("a.read\n");
	accel.read();
	accel_data &d = buffer[buffer_i];
	d.x = accel.x;
	d.y = accel.y;
	d.z = accel.z;
	buffer_i++;
}

bool accel_full() {
	return buffer_i == ACCEL_BUFFER_SIZE;
}

byte accel_size() {
	return ACCEL_BUFFER_SIZE;
}

// write data as "{x}\t{y}\t{z}"
inline float s2f(short);
void accel_write(File sd, byte i) {
  DBGSTR("a.write\n");
	float cx, cy, cz;
	accel_data &d = buffer[i];
	cx = s2f(d.x);
	cy = s2f(d.y);
	cz = s2f(d.z);
	
	sd.print(cx); sd.write('\t');
	sd.print(cy); sd.write('\t');
	sd.print(cz);
}

void accel_reset() {
	buffer_i = 0;
}

inline float s2f(short s) {
	return (float) s / (float)(1<<11) * SCALE;
}

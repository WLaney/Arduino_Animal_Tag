#include "buffer.hpp"
#include "SFE_MMA8452Q.h"
#include "Adafruit_L3GD20.h"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>

#define BUFFER_SIZE (256 / sizeof(buffer_data))
#define SCALE 8.0

typedef struct buffer_data_s {
  float gx, gy, gz;
  short ax, ay, az;
} buffer_data;

// Buffer to hold accelerometer data until needed
buffer_data buffer[BUFFER_SIZE];
size_t buffer_index = 0;

// accelerometer data is in G's
MMA8452Q accel;
// gyroscope data is in degrees per second
Adafruit_L3GD20 gyro;

inline float axis_to_f(short s) {
  // culled from SFE_MMA8452Q code
  return (float) s / (float) (1 << 11) * SCALE;
}

void buffer_setup() {
  DBGLN("buffer setup");
  accel.init(SCALE_8G, ODR_6);
  gyro.begin(gyro.L3DS20_RANGE_250DPS);
}

// Read data into the buffer.
// If this fills, we get buffer overflow
void buffer_update() {
  DBGLN("buffer update");
  accel.read();
  gyro.read();
  buffer_data &d = buffer[buffer_index];
  d.gx = gyro.data.x;
  d.gy = gyro.data.y;
  d.gz = gyro.data.z;
  d.ax = accel.x;
  d.ay = accel.y;
  d.az = accel.z;
  buffer_index++;
}

// This invalidates the buffer after it's called
void buffer_write(File sd) {
  DBGLN("buffer write");
  for (size_t i = 0; i < BUFFER_SIZE; i++) {
    buffer_data d = buffer[i];
    sd.print(axis_to_f(d.ax));
    sd.print('\t');
    sd.print(axis_to_f(d.ay));
    sd.print('\t');
    sd.print(axis_to_f(d.az));
    sd.print('\t');
    
    sd.print(d.gx);
    sd.print('\t');
    sd.print(d.gy);
    sd.print('\t');
    sd.print(d.gz);
    sd.println();
  }
  buffer_index = 0;
}

bool buffer_needs_write() {
  return buffer_index == BUFFER_SIZE;
}

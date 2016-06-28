#include "buffer.hpp"
#include "SFE_MMA8452Q.h"
#include "Light_L3GD20.h"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>

#ifdef DEBUG
  #define BUFFER_SIZE (410 / sizeof(buffer_data))
#else
  #define BUFFER_SIZE (440 / sizeof(buffer_data))
#endif

#define SCALE 8.0

typedef struct buffer_data_s {
#ifdef USE_GYRO
  Gyro::l3gd20Data_t gdata;
#endif
  short ax, ay, az;
} buffer_data;

// Buffer holds data until needed
buffer_data buffer[BUFFER_SIZE];
size_t buffer_index = 0;

// accelerometer data is in G's
MMA8452Q accel;
// gyroscope data is in degrees per second

inline float axis_to_f(short s) {
  // culled from SFE_MMA8452Q code
  return (float) s / (float) (1 << 11) * SCALE;
}

void buffer_setup() {
  DBG("Buffer size is: ");
  DBGLN(BUFFER_SIZE);
  accel.init(SCALE_8G, ODR_12);
#ifdef USE_GYRO
  Gyro::begin();
#endif
}

// Read data into the buffer.
// If this fills, we get buffer overflow
void buffer_update() {
  if (buffer_needs_write()) {
    DBGSTR("ERROR: Buffer full but not flushed\n");
    return;
  } else {
    DBGSTR("buffer update\n");
  }
  buffer_data &d = buffer[buffer_index];

#ifdef USE_GYRO
  Gyro::read(&d.gdata);
#endif

  accel.read();
  d.ax = accel.x;
  d.ay = accel.y;
  d.az = accel.z;
  buffer_index++;
}

// This invalidates the buffer after it's called
void buffer_write(File sd) {
  DBGSTR("buffer write\n");
  for (size_t i = 0; i < BUFFER_SIZE; i++) {
    buffer_data d = buffer[i];
    sd.print(axis_to_f(d.ax),3);
    sd.print('\t');
    sd.print(axis_to_f(d.ay),3);
    sd.print('\t');
    sd.print(axis_to_f(d.az),3);

#ifdef USE_GYRO
    sd.print('\t');
    sd.print(Gyro::s2f(d.gdata.x));
    sd.print('\t');
    sd.print(Gyro::s2f(d.gdata.y));
    sd.print('\t');
    sd.print(Gyro::s2f(d.gdata.z));
#else
    for (char i=0; i<3; i++) {
      sd.print(F("\t3.1415")); // dummy data
    }
#endif

    sd.println();
  }
  buffer_index = 0;
}

bool buffer_needs_write() {
  return buffer_index == BUFFER_SIZE;
}

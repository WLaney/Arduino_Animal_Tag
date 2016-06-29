#include "buffer.hpp"
#include "SFE_MMA8452Q.h"
#include "Light_L3GD20.h"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>

#define ACCEL_SIZE (200 / sizeof(accel_data))
#define GYRO_SIZE  (200 / sizeof(Gyro::l3gd20Data_t))
#define GYRO_FIFO_SIZE 31 // ugh. sick.
#define SCALE 8.0

typedef struct {
  short x, y, z;
} accel_data;

// The buffers hold data until they are written to the SD card.
// Since data is received asynchronously, and since values are
// written directly from the sensor to an array, we use two
// separate buffers.
accel_data adata[ACCEL_SIZE];
byte aind = 0;
byte areads = 0;
// The gyro is slightly faster, so we give it
// more of the allotted buffer size.
Gyro::l3gd20Data_t gdata[GYRO_SIZE];
byte gind = 0;

// accelerometer data is in G's
MMA8452Q accel;
// gyroscope data is in degrees per second

inline float axis_to_f(short s) {
  // culled from SFE_MMA8452Q code
  return (float) s / (float) (1 << 11) * SCALE;
}

void buffer_setup() {
  accel.init(SCALE_8G, ODR_12);
  Gyro::begin();
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
  // update accel
  accel.read();
  accel_data &ad = adata[aind];
  ad.x = accel.x;
  ad.y = accel.y;
  ad.z = accel.z;
  // if gryo needs read, burst read it
  if (++areads == GYRO_FIFO_SIZE) {
    Gyro::fifo_burst_read(&gdata[gind], GYRO_FIFO_SIZE);
    gind += GYRO_FIFO_SIZE;
  }
}

// This invalidates the buffer after it's called
void buffer_write(File sd) {
  DBGSTR("buffer write\n");
  char ai = 0;
  char gi = 0;
  bool gf = false;
  bool af = false;
  while (!(gf && af)) {
    if (!af) {
      accel_data &ad = adata[ai];
      sd.print(axis_to_f(ad.x), 3);
      sd.print('\t');
      sd.print(axis_to_f(ad.y), 3);
      sd.print('\t');
      sd.print(axis_to_f(ad.z), 3);
      ai++;
      af = (ai == ACCEL_SIZE);
    }
    if (!gf) {
      Gyro::l3gd20Data_t &gd = gdata[gi];
      sd.print(Gyro::s2f(gd.x), 3);
      sd.print('\t');
      sd.print(Gyro::s2f(gd.y), 3);
      sd.print('\t');
      sd.print(Gyro::s2f(gd.z), 3);
      gi++;
      gf = (gi == ACCEL_SIZE);
    }
    sd.println();
  }
  gind = 0;
  aind = 0;
}

bool buffer_needs_write() {
  return aind >= ACCEL_SIZE ||
         gind >= GYRO_SIZE;
}

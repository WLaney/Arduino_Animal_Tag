#include "AccelSensor.hpp"
#include "SFE_MMA8452Q.h"
#include "debug.h"
#include <Arduino.h>
#include <SD.h>

float AccelSensor::axis_to_f(short s) {
  // culled from SFE_MMA8452Q code
  return (float) s / (float) (1 << 11) * scale;
}

void AccelSensor::setup() {
  accel.init(SCALE_8G, ODR_6);
  reset();
}

void AccelSensor::read() {
  DBGLN("read accel");
  accel.read();
  accel_data d;
  d.x = accel.x;
  d.y = accel.y;
  d.z = accel.z;
  buffer.push(d);
}

void AccelSensor::flush(File sd) {
  DBGLN("flush accel");
  accel_data *p = buffer.get_front();
  accel_data *e = p + buffer.get_size();
  while (p < e) {
    accel_data d = *p;
    sd.print(d.x);
    sd.print('\t');
    sd.print(d.y);
    sd.print('\t');
    sd.println(d.z);
    p++;
  }
}

bool AccelSensor::needsFlush() {
  return buffer.is_full();
}

void AccelSensor::reset() {
  buffer.reset();
}

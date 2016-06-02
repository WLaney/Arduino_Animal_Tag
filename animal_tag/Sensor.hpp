#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <SD.h>

class Sensor {
  virtual void setup() = 0;
  virtual void read()  = 0;
  virtual void flush(File sd) = 0;
};

#endif

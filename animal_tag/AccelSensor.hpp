#ifndef AccelSensor_H
#define AccelSensor_H

#include <Arduino.h>
#include <SD.h>
#include "Buffer.cpp"
#include "Sensor.hpp"
#include "SFE_MMA8452Q.h"

class AccelSensor: Sensor {
private:
  struct accel_data {
    short x: 12;
    short y: 12;
    short z: 12;
  };
  
  Buffer<accel_data, 256 / sizeof(accel_data)> buffer;
  const float scale = 8.0;
  
  MMA8452Q accel;
  
  inline float axis_to_f(short s);
  
 public:
	void setup();
	void read();
	void flush(File sd);
	bool needsFlush();
	void reset();
	
};

#endif

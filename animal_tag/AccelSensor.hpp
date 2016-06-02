#ifndef AccelSensor_H
#define AccelSensor_H

#include "Sensor.hpp"
#include "SFE_MMA8452Q.h"
#include <Arduino.h>
#include <SD.h>

class AccelSensor: Sensor {
private:
  struct accel_data {
    short x: 12;
    short y: 12;
    short z: 12;
  };
  
  const static int buff_max_length = 256 / sizeof(accel_data);
  const float scale = 8.0;
  
  MMA8452Q accel;
  accel_data buff[buff_max_length];
  size_t buff_length = 0;
  
  inline float axis_to_f(short s);
  
 public:
	void setup();
	void read();
	void flush(File sd);
	bool needsFlush();
	void reset();
	
};

#endif

#ifndef TempSensor_H
#define TempSensor_H

#include "Sensor.hpp"
#include <Arduino.h>
#include <SD.h>

class TempSensor : Sensor {
private:
  const int tmp102Address = 0x48;
  float celsius;
public:
	void setup();
	void read();
	void flush(File sd);
};

#endif

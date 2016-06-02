#ifndef RTCSensor_H
#define RTCSensor_H

#include "ds3234.h"
#include "Sensor.hpp"
#include <Arduino.h>
#include <SD.h>

class RTCSensor : Sensor {
private:
	const int cs = 9;
	ts t;
public:
	void setup();
	void read();
	void flush(File sd);
};

#endif

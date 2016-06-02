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
    accel_data *v = &buff[buff_length];
    v->x = accel.x;
    v->y = accel.y;
    v->z = accel.z;
    
    buff_length++;
}

void AccelSensor::flush(File sd) {
    for (accel_data &d : buff) {
      DBGLN("wrote accel");
      
      sd.print(axis_to_f(d.x));
      sd.print('\t');
      sd.print(axis_to_f(d.y));
      sd.print('\t');
      sd.println(axis_to_f(d.z));
    }
}

bool AccelSensor::needsFlush() {
	return (buff_length >= buff_max_length);
}

void AccelSensor::reset() {
	buff_length = 0;
}

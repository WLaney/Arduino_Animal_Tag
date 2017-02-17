#include "tag.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>

Tag::Tag(bool update) {
	if (update) this->update();
}

Tag::Tag(char *name, bool orient, float bias_x, float bias_y, float bias_z,
         ACCEL_SCALE accel_scale, GYRO_SCALE gyro_scale, SAMPLE_RATE sample_rate) {
	strncpy(this->name, name, 4);
	this->orient = orient;
	this->bias_x = bias_x;
	this->bias_y = bias_y;
	this->bias_z = bias_z;
	this->accel_scale = accel_scale;
	this->gyro_scale = gyro_scale;
	this->sample_rate = sample_rate;
}

Tag::Tag(Tag &other) {
	strncpy(name, other.name, 4);
	orient = other.orient;
	bias_x = other.bias_x;
	bias_y = other.bias_y;
	bias_z = other.bias_z;
	accel_scale = other.accel_scale;
	gyro_scale  = other.gyro_scale;
	sample_rate = other.sample_rate;
}

void Tag::update() {
  name[0] = EEPROM[0];
  name[1] = EEPROM[1];
  name[2] = EEPROM[2];
  name[3] = EEPROM[3];
  orient  = EEPROM[4];
  EEPROM.get(5,  bias_x);
  EEPROM.get(9,  bias_y);
  EEPROM.get(13, bias_z);
  accel_scale = static_cast<ACCEL_SCALE>(EEPROM.read(17));
  gyro_scale  = static_cast<GYRO_SCALE>(EEPROM.read(18));
  sample_rate = static_cast<SAMPLE_RATE>(EEPROM.read(19));
}

void Tag::write() {
  EEPROM.put(0,  name[0]);
  EEPROM.put(1,  name[1]);
  EEPROM.put(2,  name[2]);
  EEPROM.put(3,  name[3]);
  EEPROM.put(4,  orient);
  EEPROM.put(5,  bias_x);
  EEPROM.put(9,  bias_y);
  EEPROM.put(13, bias_z);
  EEPROM.put(17, static_cast<byte>(accel_scale));
  EEPROM.put(18, static_cast<byte>(gyro_scale));
  EEPROM.put(19, static_cast<byte>(sample_rate));
}

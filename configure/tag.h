#ifndef __TAG_H__
#define __TAG_H__
#include <Arduino.h>

/*
Class representing tag configuration data.
Can be updated from or written to the actual tag's EEPROM.

EEPROM address map:

byte  | val         | type
------|-------------|------
0-3   | name        | char[4] (no \0)
4     | orient      | bool
5     | bias_x      | float
9     | bias_y      | float
13    | bias_z      | float
17    | accel_scale | byte
18    | gyro_scale  | byte
19    | sample_rate | byte
20    | hq_accel    | byte
21-24 | alarm_delay | long int (32-bit int)
*/

class Tag {
public:
	enum ACCEL_SCALE { ACCEL_2G, ACCEL_4G, ACCEL_8G, };
	enum GYRO_SCALE  { GYRO_2000DPS, GYRO_1000DPS, GYRO_500DPS, GYRO_250DPS};
	enum SAMPLE_RATE { ODR_12_5_HZ, ODR_25_HZ, ODR_50_HZ };

	char name[4];
	bool orient;
	float bias_x;
	float bias_y;
	float bias_z;
	ACCEL_SCALE accel_scale;
	GYRO_SCALE  gyro_scale;
	SAMPLE_RATE sample_rate;
	byte accel_hq;
	unsigned long startup_delay;
	
	Tag(bool update=false);
	Tag(Tag &);
	Tag(char*, bool, float, float, float, ACCEL_SCALE, GYRO_SCALE, SAMPLE_RATE, byte, unsigned long);
	
	void update();
	void write();
};

#endif // __TAG_H__

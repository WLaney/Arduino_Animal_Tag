/*
 * Wrapper namespace; access tag configuration.
 */

#ifndef __TAG_H__
#define __TAG_H__
#include <Arduino.h>

class Tag {
public:
	char name[4];
	bool orient;
	float bias_x;
	float bias_y;
	float bias_z;
	
	Tag(bool update=false);
	Tag(Tag &);
	Tag(char*, bool, float, float, float);
	
	void update();
	void write();
};

#endif // __TAG_H__

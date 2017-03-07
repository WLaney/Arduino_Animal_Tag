#include "Accel_1Q.h"

#define PRINTSTR(s) Serial.print(F(s))

void setup() {
	Serial.begin(9600);
	bool success = Accel::begin(Accel::ODR::HZ_12_5,
	               Accel::Range::G4);
    if (success) {
		PRINTSTR("Accelerometer initialized\n");
	} else {
		PRINTSTR("Accelerometer could not be initialized\n");
	}
}

void loop() {
	delay(80);
	Accel::sample_raw out_r;
	Accel::sample out_s;
	out_r = Accel::read_raw();
	out_s = Accel::parse_raw(out_r);
	PRINTSTR("Xr: "); Serial.print(out_r.x);
	PRINTSTR("\tYr: "); Serial.print(out_r.y);
	PRINTSTR("\tZr: "); Serial.print(out_r.z);
	PRINTSTR("\nX: "); Serial.print(out_s.x);
	PRINTSTR("\tY: "); Serial.print(out_s.y);
	PRINTSTR("\tZ: "); Serial.println(out_s.z);
}

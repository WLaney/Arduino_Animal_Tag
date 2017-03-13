#include "fxas_2.h"
#include <Narcoleptic.h>

constexpr long brate = 9600;

inline void n_delay(long);

void setup() {
	Serial.begin(brate);
	FXAS2::begin(FXAS2::ODR::HZ_25,
	            FXAS2::Range::DPS_250,
	            false);
}

void loop() {
	if (FXAS2::isActive) {
		Serial.print("Standby");
		FXAS2::standby();
	} else {
		Serial.print("Active");
		FXAS2::active();
		n_delay(FXAS2::timeToActive());
	}
	Serial.println(": reading data");
	for (int i=0; i<10; i++) {
		FXAS2::sample s;
		FXAS2::read(s);
		Serial.print(FXAS2::sampleToDps(s.x)) ; Serial.write(' ');
		Serial.print(FXAS2::sampleToDps(s.y)) ; Serial.write(' ');
		Serial.println(FXAS2::sampleToDps(s.z));
		n_delay(40);
	}
}

inline void n_delay(long ms) {
	Serial.end();
	Narcoleptic.delay(ms);
	Serial.begin(brate);
}

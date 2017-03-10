#include <Accel_1Q.h>

Accel::sample_raw buffer[32];

void setup() {
	Serial.begin(9600);
	bool good = Accel::begin(
		Accel::ODR::HZ_12_5,
		Accel::Range::G2,
		Accel::FIFO_Mode::STOP
	);
	if (!good) {
		Serial.println(F("Accelerometer couldn't be found."));
	}
}

void loop() {
	for (byte i=0; i<32; i++) {
		buffer[i] = {0, 0, 0};
	}
	Accel::read_burst(buffer, 32);
	for (auto &sr : buffer) {
		Accel::sample s = Accel::parse_raw(sr);
		Serial.print(F("X: ")); Serial.print(s.x);
		Serial.print(F("\tY: ")); Serial.print(s.y);
		Serial.print(F("\tZ: ")); Serial.println(s.y);
	}
	delay(long{1000 / 12.5 * 32 - 1000});
}

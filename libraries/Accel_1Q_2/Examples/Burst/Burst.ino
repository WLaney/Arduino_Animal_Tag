#include <Accel_1Q_2.h>

Accel::sample_raw buffer[32];

void setup() {
	Serial.begin(9600);
	bool good = Accel::begin_standby(
		Accel::ODR::HZ_12_5,
		Accel::Range::G2
	);
	if (!good) {
		Serial.println(F("Accelerometer couldn't be found."));
	}
	Accel::set_fifo(Accel::FIFO_Mode::FILL);
	Accel::set_active(true);
}

void loop() {
	byte samples_read;
	delay(long{1000 / 12.5 * 32});
	for (byte i=0; i<32; i++) {
		buffer[i] = {0, 0, 0};
	}
	samples_read = Accel::read_burst(buffer, 32);
	Serial.print(samples_read);
	Serial.println(F(" samples read"));
	for (auto &sr : buffer) {
		Accel::sample s = Accel::parse_raw(sr);
		Serial.print(F("Xr: ")); Serial.print(sr.x);
		Serial.print(F("\tYr: ")); Serial.print(sr.y);
		Serial.print(F("\tZr: ")); Serial.print(sr.z);
		Serial.print(F("\tX: ")); Serial.print(s.x);
		Serial.print(F("\tY: ")); Serial.print(s.y);
		Serial.print(F("\tZ: ")); Serial.println(s.z);
	}
	Serial.println();
}

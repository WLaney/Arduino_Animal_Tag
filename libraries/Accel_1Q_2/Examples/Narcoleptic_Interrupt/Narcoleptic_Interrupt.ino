/*
 * In this demo, the accelerometer interrupt 1 pin should be connected to pin 3
 * on the Arduino.
 * 
 * This differs from the previous sketch in that the Arduino powers
 * down in between thingamajigs.
 */
#include <Accel_1Q_2.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

Accel::sample_raw buffer[32];

void setup() {
	Serial.begin(38400);
	
	pinMode(3, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(3), on_fifo_full, FALLING);
	
	bool res = Accel::begin_standby(
		Accel::ODR::HZ_12_5,
		Accel::Range::G2
	);
	if (!res) {
		Serial.println(F("Couldn't start accelerometer"));
		while (true)
			;
	}
	Accel::set_fifo(Accel::FIFO_Mode::FILL);
	Accel::set_fifo_interrupt(true);
	Accel::set_active(true);
}

void loop() {	
	// Put the Arduino asleep until on_fifo_full() wakes it up
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_mode();
	sleep_disable();
	Serial.begin(38400);
	byte samples_read;
	for (byte i=0; i<32; i++) {
		buffer[i] = {0xFFFF, 0xFFFF, 0xFFFF};
	}
	samples_read = Accel::read_burst(buffer, 32);
	Serial.print(samples_read);
	Serial.println(F(" samples read"));
	for (auto &sr : buffer) {
		Accel::sample s = Accel::parse_raw(sr);
		Serial.print(F("Xr: "));   Serial.print(sr.x);
		Serial.print(F("\tYr: ")); Serial.print(sr.y);
		Serial.print(F("\tZr: ")); Serial.print(sr.z);
		Serial.print(F("\tX: "));  Serial.print(s.x);
		Serial.print(F("\tY: "));  Serial.print(s.y);
		Serial.print(F("\tZ: "));  Serial.println(s.z);
	}
	Serial.println();
	Serial.end();
}

void on_fifo_full() {
	
}

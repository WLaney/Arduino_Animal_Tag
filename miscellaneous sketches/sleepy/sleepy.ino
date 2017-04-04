/*
 * Low-power sketch based on Nick Gammon's gammon.com.au/power
 * This sketch doesn't save power as effectively as it could, but accurately represents
 * the Sharkduino's power consumption during sleep mode.
 *
 * I think we could low power further by disabling unused Arduino features, like the brownout
 * detector.
 */
#include <avr/sleep.h>

void setup() {
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
}

void loop() {}

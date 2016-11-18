/*
 * A quick and dirty comparison of the FXAS and LGD320 boards.
 * Outputs the readings for each board using the same settings
 * (25Hz, 250DPS).
 */
#include "Adafruit_L3GD20.h"
#include "fxas.h"

#define ERROR(s) {Serial.println(F(s)); while (true);}

Adafruit_L3GD20 l3gd20 = Adafruit_L3GD20();
FXAS_Gyro fxas = FXAS_Gyro();

void setup() {
  Serial.begin(9600);
  // Data output rate and low-power mode set in source file
  if (!l3gd20.begin(Adafruit_L3GD20::L3DS20_RANGE_250DPS, 0x6B)) {
    ERROR("Could not start l3gd20.")
  }
  // Same thing here
  if (!fxas.begin()) {
    ERROR("Could not start FXAS.");
  }
  Serial.println(F("Good to go!"));
  Serial.println(F("L3GD20       | FXAS"));
  Serial.println(F("-------------|----------"));
}

void loop() {
  l3gd20.read();
  Serial.print(l3gd20.data.x, 5); Serial.print(F(", "));
  Serial.print(l3gd20.data.y, 5); Serial.print(F(", "));
  Serial.println(l3gd20.data.z, 5);
  delay(1000.0 / 25);
}

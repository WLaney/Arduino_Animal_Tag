/*
 * A quick and dirty comparison of the FXAS and LGD320 boards.
 * Outputs the readings for each board using the same settings
 * (25Hz, 250DPS).
 */
#include "Adafruit_L3GD20.h"

Adafruit_L3GD20 l3gd20 = Adafruit_L3GD20();

void setup() {
  Serial.begin(9600);
  // I don't know if the output data rate is set correctly
  // here, and since this is a separate branch I will probably
  // just edit the original library to suit my needs
  if (!l3gd20.begin(Adafruit_L3GD20::L3DS20_RANGE_250DPS, 0x6B)) {
    Serial.println(F("Could not start L3GD20."));
    while (true) ;
  }
  Serial.println(F("Ready to fly!"));
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

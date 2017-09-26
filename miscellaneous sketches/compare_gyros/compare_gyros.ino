/*
 * Displays side-by-side output of the L3GD20 and
 * FXAS21002C gyroscopes' readings.
 */
#include <Adafruit_L3GD20.h>
#include <Gyro_FXAS.h> 

#define DSTR(s) Serial.print(F(s))

Adafruit_L3GD20 gyro_l3gd20;
FXAS::sample fxas_data;

void setup() {
  FXAS::begin(FXAS::ODR::HZ_25, FXAS::Range::DPS_250, false);
  gyro_l3gd20.begin();

  Serial.begin(38400);
  DSTR("Range: 250DPS\n");
  DSTR("ODR: 25Hz\n");
  DSTR("          FXAS                  L3GD20\n");
}

void loop() {
  long timer = millis();
  FXAS::read(fxas_data);
  gyro_l3gd20.read();

  Serial.print(FXAS::sampleToDps(fxas_data.x)); Serial.write(' ');
  Serial.print(FXAS::sampleToDps(fxas_data.y)); Serial.write(' ');
  Serial.print(FXAS::sampleToDps(fxas_data.z)); Serial.write('\t');
  
  Serial.print(gyro_l3gd20.data.x); Serial.write(' ');
  Serial.print(gyro_l3gd20.data.y); Serial.write(' ');
  Serial.println(gyro_l3gd20.data.z);
  timer = millis() - timer;
  timer -= 40;
  if (timer > 0) {
    delay(timer);
  } else {
    DSTR("The serial monitor can't read the data quickly enough!");
  }
}

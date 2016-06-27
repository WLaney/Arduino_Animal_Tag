#include <Adafruit_L3GD20.h>

Adafruit_L3GD20 gyro;

void setup() {
  Serial.begin(9600);
  gyro.begin(gyro.L3DS20_RANGE_250DPS);
}

void loop() {
  gyro.read();
  Serial.print(gyro.data.x);
  Serial.print('\t');
  Serial.print(gyro.data.y);
  Serial.print('\t');
  Serial.println(gyro.data.z);
  delay(70);
}

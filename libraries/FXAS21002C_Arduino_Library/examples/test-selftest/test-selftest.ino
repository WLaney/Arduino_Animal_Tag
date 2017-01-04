/*
 * Run the gyroscope's self-test function. When run, the sensor's data registers will output
 * test values that should be within a certain range. If not, your chip has issues.
 *
 *     Min: 7000
 * Typical: 16000
 *     Max: 25000
 */
#include "FXAS21002C.h"

FXAS21002C sensor(0x20);

void setup() {
    Serial.begin(9600);
    Serial.println("Gyro self-test values:");
    sensor.initSelfTest();
}

void loop() {
  sensor.getGres();
  sensor.readGyroData();
  
  Serial.print("X: ");
  Serial.print((int)sensor.gyroData.x);
  Serial.print(" Y: ");
  Serial.print((int)sensor.gyroData.y);
  Serial.print(" Z: ");
  Serial.println((int)sensor.gyroData.z);

  delay(500);
}

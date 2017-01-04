#include <Wire.h>
#include <FXAS21002C.h>

FXAS21002C sensor = FXAS21002C(0x20); // SA0=1 0x21

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Initialize the FXAS21002C
  sensor.init();
  
  // See if the ODR and Rate are their expected values
  sensor.standby();
  byte odr = (sensor.readReg(FXAS21002C_H_CTRL_REG1) & 0x1C) >> 2; //bits 4:2
  byte range = sensor.readReg(FXAS21002C_H_CTRL_REG0) & 0x01; //bit 0
  Serial.print("ODR: ");
  switch (odr) {
    case 0: Serial.print("800");  break;
    case 1: Serial.print("400");  break;
    case 2: Serial.print("200");  break;
    case 3: Serial.print("100");  break;
    case 4: Serial.print("50");   break;
    case 5: Serial.print("25");   break;
    case 6: Serial.print("12.5"); break;
    case 7: Serial.print("12.5"); break;
  }
  Serial.println("Hz");

  Serial.print("Range: ");
  switch (range) {
    case 0: Serial.print("2000"); break;
    case 1: Serial.print("1000"); break;
    case 2: Serial.print("500"); break;
    case 3: Serial.print("250"); break;
  }
  Serial.println("DPS");
  sensor.active();
}

void loop() {
  sensor.getGres();
  // Query the sensor
  sensor.readGyroData();
  
  // Print out the data
  // Gyroscope
  Serial.print("Gyroscope ");
  Serial.print("X: ");
  Serial.print((int)sensor.gyroData.x);
  Serial.print(" Y: ");
  Serial.print((int)sensor.gyroData.y);
  Serial.print(" Z: ");
  Serial.println((int)sensor.gyroData.z);

  delay(100);
}

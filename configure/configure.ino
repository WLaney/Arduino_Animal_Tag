/*
 * Write configuration data to the device's EEPROM - its name, its
 * sensor orientation, etc.
 * 
 * Tag EEPROM is of the format:
 * 0x0..0x3 - Device name, e.g. "SHRK".
 * 0x4      - Sensor orientation.
 *            * 0 - Accelerometer and gyroscope have the same orientation.
 *            * 1 - Gyroscope needs rotation (see MATLAB sketch).
 */
#include <EEPROM.h>
#include <Light_L3GD20.h>
#define PRINTSTR(S) Serial.print(F(S))

const int gyro_calibration_reads=12*60;

void setup() {
  char name[5];
  byte orient;
  float gx = 0;
  float gy = 0;
  float gz = 0;
  
  Serial.begin(9600);
  PRINTSTR("Device Configuration\n\n");
  PRINTSTR("Enter a 4-letter name for the device:\n");
  while (Serial.available() < 4)
    ;
  Serial.readBytes(name, 4);
  name[4] = '\0';
  Serial.println(name);
  
  bool valid = question("Is your device an early prototype with an incorrect orientation? (y/n)\n");
  orient = (byte)valid;

  PRINTSTR("We don't currenty use the calibration feature. Skipping...");
  gx = 0;
  gy = 0;
  gz = 0;
/*
  if (question("Would you like to calibrate the gyroscope? (y/n)\n")) {
    PRINTSTR("Keep the serial monitor open! Otherwise, the sketch will restart!");
    PRINTSTR("Lay the device on a flat surface. The test will begin in 5 seconds.\n");
    delay(5000);
    PRINTSTR("Alright. Go do something else for minute or two.\n");
    
    Gyro::begin();
    Gyro::l3gd20Data_t gd;
    for (int i=1; i<=gyro_calibration_reads; i++) {
      Gyro::read(&gd);
      float ri = 1.0 / i;
      gx = gx + (Gyro::s2f(gd.x) - gx) * ri;
      gy = gy + (Gyro::s2f(gd.y) - gy) * ri;
      gz = gz + (Gyro::s2f(gd.z) - gz) * ri;
      delay(80);
    }
    if (orient) {
      float temp = gx;
      gx = gy;
      gy = temp;
    }

    PRINTSTR("Done calibrating.\n");
    PRINTSTR("X: ") ; Serial.print(gx) ; Serial.write('\n');
    PRINTSTR("Y: ") ; Serial.print(gy) ; Serial.write('\n');
    PRINTSTR("Z: ") ; Serial.print(gz) ; Serial.write('\n');
  } else {
    PRINTSTR("Assuming bias of (0,0,0)...");
    gx = 0.0;
    gy = 0.0;
    gz = 0.0;
  }
*/
  PRINTSTR("Writing values to EEPROM...\n");
  EEPROM.put(0, name[0]);
  EEPROM.put(1, name[1]);
  EEPROM.put(2, name[2]);
  EEPROM.put(3, name[3]);
  EEPROM.put(4, orient);
  EEPROM.put(5, gx);
  EEPROM.put(9, gy);
  EEPROM.put(13, gz);
  PRINTSTR("Done.\n");
}

void loop() {

}

// Ask a (y/n) question and return the response
bool question(const char *msg) {
  Serial.print(msg);
  while (true) {
    while (!Serial.available())
      ;
    switch (Serial.read()) {
      case 'Y':
      case 'y':
        PRINTSTR("Yes\n");
		return true;
        break;
      case 'N':
      case 'n':
        PRINTSTR("No\n");
		return false;
        break;
      default:
        PRINTSTR("Invalid input.\n");
    }
  }
}


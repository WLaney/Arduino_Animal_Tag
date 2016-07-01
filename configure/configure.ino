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
#define PRINTSTR(S) Serial.print(F(S))

void setup() {
  char name[5];
  char orient;
  
  Serial.begin(9600);
  PRINTSTR("Device Configuration\n\n");
  PRINTSTR("Enter a 4-letter name for the device:\n");
  while (Serial.available() < 4)
    ;
  Serial.readBytes(name, 4);
  name[4] = '\0';
  Serial.println(name);
  
  PRINTSTR("Is your device an early prototype with an incorrect orientation? (y/n)\n");
  bool valid=false;
  while (!valid) {
    while (!Serial.available())
      ;
    switch (Serial.read()) {
      case 'Y':
      case 'y':
        PRINTSTR("Yes\n");
        valid = true;
        orient = 1;
        break;
      case 'N':
      case 'n':
        PRINTSTR("No\n");
        valid = true;
        orient = 0;
        break;
      default:
        PRINTSTR("Incorrect input.\n");
    }
  }

  PRINTSTR("Writing values to EEPROM...\n");
  EEPROM.put(0, name[0]);
  EEPROM.put(1, name[1]);
  EEPROM.put(2, name[2]);
  EEPROM.put(3, name[3]);
  EEPROM.put(4, orient);
  PRINTSTR("Done.\n");
}

void loop() {
  // put your main code here, to run repeatedly:

}

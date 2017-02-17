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
#include "tag.h"
#define PRINTSTR(S) Serial.print(F(S))

Tag tag;

void setup() {
  Serial.begin(9600);
  PRINTSTR("Device Configuration\n\n");
  tag = Tag(true);
}

void loop() {
  PRINTSTR(
    "a) Print current device configuration\n" \
    "b) Set device name\n" \
    "c) Set device orientation (for old tags)\n" \
    "d) Calibrate gyroscope (not implemented)\n" \
    "e) Write changes\n\n" \
  );
  while (!Serial.available())
    ;
  char result = Serial.read();
  // Somewhat janky downcase() method
  if (result < 'a') result = (result - 'A') + 'a';
  switch (result) {
  case 'a':
    print_configuration();
    break;
  case 'b':
    set_name();
    break;
  case 'c':
    set_orientation();
    break;
  case 'd':
    calibrate();
    break;
  case 'e':
    write_changes();
    break;
  default:
    PRINTSTR("Sorry, didn't quite catch that...");
  }
  Serial.println('\n');
}

// Update the current settings here
void get_configuration() {
  tag.update();
}

// Print the tag's current values
void print_configuration() {
  Tag current(true);
  PRINTSTR("Name:   "); Serial.write(tag.name, 4); Serial.write('\n');
  PRINTSTR("Orient: "); Serial.println(tag.orient);
  PRINTSTR("X Bias: "); Serial.println(tag.bias_x);
  PRINTSTR("Y Bias: "); Serial.println(tag.bias_y);
  PRINTSTR("Z Bias: "); Serial.println(tag.bias_z);
  PRINTSTR("");
}

void set_name() {
  PRINTSTR("Enter a 4-letter name for the device:\n");
  while (Serial.available() < 4)
    ;
  Serial.readBytes(tag.name, 4);
  Serial.println(tag.name);
}

void set_orientation() {
  bool valid = question("Is your device an early prototype with an incorrect orientation? (y/n)\n");
  tag.orient = (byte)valid;
}

void calibrate() {
  PRINTSTR("We don't currenty use the calibration feature. Skipping...\n");
}

void write_changes() {
  PRINTSTR("Writing values to EEPROM...\n");
  tag.write();
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


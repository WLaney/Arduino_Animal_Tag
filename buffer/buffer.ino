#include "eeprom_buffer.hpp"

rom_buffer<int> buffer(600, 3);

void setup() {
	Serial.begin(9600);
	
	buffer.push(0);
	buffer.push(1);
	buffer.push(2);
  read_all_for_reals();

  buffer.push(3);
  buffer.push(4);
  buffer.push(5);
  read_all_for_reals();
}

void loop() {
	
}

void read_all_for_reals() {
  int *vals = buffer.read_all();
  for (int i=0; i<3; i++) {
    Serial.println(vals[i]);
  }
  delete vals;
}


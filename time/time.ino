#include "ds3234.h"

const int pin = 9;
inline int prompt(const char *msg);

void setup() {
  Serial.begin(9600);
  DS3234_init(pin, DS3234_INTCN);
  
  Serial.println(F("Set the time? (y/n)"));
  while (!Serial.available())
    ;
  String s = Serial.readString();
  if (s.charAt(0) == 'y') {
    // Write data to the clock
    ts t;
    t.year = prompt("Year:");
    t.mon  = prompt("Month:");
    t.mday = prompt("Day of the month:");
    t.wday = prompt("Day of the week: ");
    t.hour = prompt("Hour: ");
    t.min  = prompt("Minute: ");
    t.sec  = prompt("Second: ");
    DS3234_set(pin, t);
  } else {
    Serial.println(F("'Kay..."));
  }
}

void loop() {
  // Read data from the clock
  ts t;
  DS3234_get(pin, &t);
  Serial.print(t.mon);   Serial.print('-');
  Serial.print(t.mday);  Serial.print('-');
  Serial.print(t.year);
  Serial.print(' ');
  Serial.print(t.hour); Serial.print(':');
  Serial.print(t.min);  Serial.print(':');
  Serial.println(t.sec);
  delay(1000);
}

inline int prompt(const char *msg) {
  int i;
  Serial.println(msg);
  while (!Serial.available())
    ;
  i = Serial.parseInt();
  Serial.println(i);
  return i;
}

#include <SD.h>
#include "ds3234.h"

void setup() {
  Serial.begin(9600);
  Serial.println(sizeof(int));
  ts tme;
  tme.sec = 0;
  tme.min = 1;
  tme.hour = 2;
  tme.mday = 3;
  tme.mon = 4;
  tme.year = 1<<24;
  tme.wday = 6;
  tme.yday = 7;
  tme.isdst = 8;
  tme.year_s = 9;

  print_bytes(tme);
  
  if (!SD.begin(10)) {
    Serial.println(F("Could not initialize SD card"));
  }
  File f = SD.open("TEST.TXT", FILE_WRITE);
  if (f) {
    f.write((byte *) &tme, sizeof(tme));
    f.close();
    Serial.println(F("Wrote to SD card."));
  } else {
    Serial.println(F("Could not open TEST.TXT for writing"));
  }
}

void loop() { }

template<typename T> inline void print_bytes(T val) {
  Serial.print("sizeof(val): ");
  Serial.println(sizeof(T));
  char *cb = (char *) &val;
  for (int i = 0; i < sizeof(T); i++) {
    Serial.print(i, HEX);
    Serial.print(":\t");
    Serial.println(cb[i], HEX);
  }
}


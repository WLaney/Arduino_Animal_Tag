#include <SD.h>
#include <math.h>

struct header_data {
  char name[4];            // Name of device [4 chars]
  byte orient;             // Orientation
  float gx, gy, gz;        // Gyroscope Bias
  unsigned short aws, gws; // Accelerometer and Gyroscope write size
  unsigned short period;   // Long-term write period
  float as, gs;            // Accelerometer and gyroscope scale
};

void setup() {
  Serial.begin(9600);
  // Create struct, write conspicuous data to it
  header_data hd;
  hd.name[0] = 'A';
  hd.name[1] = 'B';
  hd.name[2] = 'C';
  hd.name[3] = 'D';
  hd.orient = 255;
  hd.gx = 4.0;
  hd.gy = 4.0;
  hd.gz = 4.0;
  hd.aws = 2;
  hd.gws = 3;
  hd.period = 4;
  hd.as = 8.0;
  hd.gs = 8.0;
  // Print the struct
  print_bytes(hd);
  // Make a file and print to it
  if (!SD.begin(10)) {
    Serial.println(F("Could not initialize SD card."));
    return;
  }
  delay(1000);
  File f= SD.open("TEST.TXT", FILE_WRITE);
  if (f) {
    f.write((byte *) &hd, sizeof(hd));
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


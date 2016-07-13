
#include <math.h>

struct header_data {
  char name[4];            // Name of device [4 chars]
  byte orient;             // Orientation
  float gx, gy, gz;        // Gyroscope Bias
  unsigned short aws, gws; // Accelerometer and Gyroscope write size
  unsigned short period;
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
  hd.gx = INFINITY;
  hd.gy = -INFINITY;
  hd.gz = INFINITY;
  hd.aws = 4;
  hd.gws = 5;
  hd.period = 6;
  hd.as = INFINITY;
  hd.gs = -INFINITY;
  // Print the struct
  print_bytes(hd);
}

void loop() {
  
}

template<typename T> inline void print_bytes(T val) {
  Serial.print("sizeof(val): ");
  Serial.println(sizeof(T));
  char *cb = (char *) &val;
  for (int i = 0; i < sizeof(T); i++) {
    Serial.print(i);
    Serial.print(":\t");
    Serial.println(cb[i], HEX);
  }
}


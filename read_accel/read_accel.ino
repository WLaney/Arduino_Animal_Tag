#include <SFE_MMA8452Q.h>

MMA8452Q accel;

void setup() {
  Serial.begin(9600);
  accel.init(SCALE_8G, ODR_12);
}

void loop() {
  while (!accel.available())
    ;
  accel.read();
  Serial.print(accel.cx, 3);
  Serial.write('\t');
  Serial.print(accel.cy, 3);
  Serial.write('\t');
  Serial.println(accel.cz, 3);
}

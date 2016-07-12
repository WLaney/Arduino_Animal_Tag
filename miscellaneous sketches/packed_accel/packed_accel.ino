#include <SFE_MMA8452Q.h>
#include "PackedBuffer.h"

// PackedBuffer.cpp
extern template class PackedBuffer<40>;

MMA8452Q accel;
PackedBuffer<40> buff;

void setup() {
  Serial.begin(9600);
  accel.init(SCALE_8G, ODR_12);
}

void loop() {
  while (!buff.full()) {
    while (!accel.available())
      ;
    accel.read();
    buff.push(accel.x, accel.y, accel.z);
  }
  buff.print_all();
  buff.reset();
}

#include "fxas.h"
#include <Wire.h>
#include <Arduino.h>

bool FXAS_Gyro::begin() {
  // Check if tag is responding
  byte who = read8(WHO_AM_I);
  if (who != who_am_i_value) {
    Serial.println(who, HEX);
    return false;
  }
  return true;
}

void FXAS_Gyro::read() {
  
}

// Copied, pasted from Adafruit library
byte FXAS_Gyro::read8(registers reg) {
  byte value;
  Wire.beginTransmission(address);
  Wire.write((byte) reg);
  Wire.endTransmission();
  Wire.requestFrom(address, (byte)1);
  value = Wire.read();
  Wire.endTransmission();
  return value;
}

void FXAS_Gyro::write8(byte reg, byte value) {
  Wire.beginTransmission(address);
  Wire.write((byte)reg);
  Wire.write(value);
  Wire.endTransmission();
}

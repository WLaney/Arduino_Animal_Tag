/*************************************************** 
  This is an example for the Adafruit Triple-Axis Gyro sensor

  Designed specifically to work with the Adafruit L3GD20 Breakout 
  ----> https://www.adafruit.com/products/1032

  These sensors use I2C or SPI to communicate, 2 pins (I2C) 
  or 4 pins (SPI) are required to interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Kevin "KTOWN" Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h> 
#include <Light_L3GD20.h>

void setup() 
{
  Serial.begin(9600);
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!Gyro::begin())
  {
    Serial.println("Oops ... unable to initialize the L3GD20. Check your wiring!");
    while (1);
  }
}

void loop() 
{
  int16_t data[3];
  Gyro::read(data);
  Serial.print(Gyro::s2f(data[0]));   Serial.print(" ");
  Serial.print(Gyro::s2f(data[1]));   Serial.print(" ");
  Serial.println(Gyro::s2f(data[2])); Serial.print(" ");
  delay(100);
}

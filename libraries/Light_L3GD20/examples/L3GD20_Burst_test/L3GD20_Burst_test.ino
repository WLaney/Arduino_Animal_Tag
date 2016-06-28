/*
 * Burst read test. This example polls the gyroscope once every 32 reads... right?
 * 
 * Please enable the #L3GD20_USE_FIFO option in the library to use this code.
 */
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

Gyro::l3gd20Data_t data[31];

void setup() 
{
  Serial.begin(9600);
  
  // Try to initialise and warn if we couldn't detect the chip
  if (Gyro::begin()) {
    Serial.println(F("Working fine..."));
  } else {
    Serial.println(F("Oops ... unable to initialize the L3GD20. Check your wiring!"));
    while (1);
  }
}

void loop() 
{
  long tme = millis();
  byte fifo_src = Gyro::get_fifo_src();
  byte reads = fifo_src & 0x1F;
  Serial.print("Reads filled (?): "); Serial.println(reads);
  Serial.print("Empty bit:        "); Serial.println((fifo_src & 0x20) >> 5);
  Serial.print("Overrun bit:      "); Serial.println((fifo_src & 0x40) >> 6);
  Serial.print("Threshold bit:    "); Serial.println((fifo_src & 0x80) >> 7);
  
  Serial.println(F("Reading burst..."));
  for (byte i=0; i<reads-1; i+=5) {
    Gyro::fifo_burst_read(&data[i], 5);
  }

  for (byte i=0; i<Gyro::buffer_size; i++) {
    Serial.print(Gyro::s2f(data[i].x));   Serial.print(" ");
    Serial.print(Gyro::s2f(data[i].y));   Serial.print(" ");
    Serial.println(Gyro::s2f(data[i].z));
  }
  Serial.print(millis() - tme);
  Serial.println(" Milliseconds elapsed");
}

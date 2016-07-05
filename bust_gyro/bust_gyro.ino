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
#include <Narcoleptic.h>

Gyro::l3gd20Data_t data[31];

#define PRINTSTR(S) Serial.print(F(S))

void setup() 
{
  Serial.begin(38400);
  
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
  // Clear the array with a suspicious value so that we can
  // figure out which cells were overwritten
  for (byte i = 0; i < 31; i++) {
    data[i].x = 0x00000000;
    data[i].y = 0x00000000;
    data[i].z = 0x00000000;
  }
  byte reads = Gyro::fifo_get_length();
  PRINTSTR("Length: "); Serial.println(reads);
  
  PRINTSTR("Reading...\n");
  long bytes_read;
  long tme = millis();
  bytes_read = Gyro::fifo_burst_read(data, 31);
  Serial.print(millis() - tme);
  PRINTSTR("ms to read gyro\n");
  Serial.print(bytes_read);
  Serial.println(" read");

  for (byte i=0; i<Gyro::buffer_size; i++) {
    Serial.print(Gyro::s2f(data[i].x)); Serial.print(' ');
    Serial.print(Gyro::s2f(data[i].y)); Serial.print(' ');
    Serial.print(Gyro::s2f(data[i].z)); Serial.print('\t');
    
    Serial.print(data[i].x, HEX); Serial.print(' ');
    Serial.print(data[i].y, HEX); Serial.print(' ');
    Serial.println(data[i].z, HEX);
  }

  Serial.end();
  Narcoleptic.delay(2400);
  Serial.begin(38400);
}

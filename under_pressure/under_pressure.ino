#include <SparkFun_MS5803_I2C.h>
#include <Wire.h>

/*
 * Get the current temperature, in Celsius.
 */
constexpr size_t temp102_address = 0x48;
float get_temp102() {
  Wire.requestFrom(temp102_address, 2);
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  short temperatureSum = ((MSB << 8) | LSB) >> 4;
  return temperatureSum * 0.0625;
}

MS5803 pressure(ADDRESS_LOW);

void setup() {
  Wire.begin();
  pressure.begin();
}

void loop() {
  float temp_102 = get_temp102();
  float temp_prs = pressure.getTemperature(CELSIUS, ADC_4096);
  Serial.print("TEMP102:  "); Serial.println(temp_102);
  Serial.print("Pressure: "); Serial.println(temp_prs);
}

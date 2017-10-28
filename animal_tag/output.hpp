/*
 * Functions for writing data to the SD card from the sensors.
 */
#include <Arduino.h>

struct header_data
{
  char name[4];            // Name of device [4 chars, no null]
  byte orient;             // Orientation
  float gx, gy, gz;        // Gyroscope Bias
  unsigned short aws, gws; // Accelerometer and Gyroscope write size
  unsigned short period;   // How many short-term reads per long read
  float as, gs;            // Accelerometer and gyroscope scale
  byte sample_rate;        // Samples per second
  byte hq_accel;           // Accelerometer oversampling
  long alarm_delay;        // Startup timer delay
};

// Setup the SD card. Returns if successful.
bool output_setup();
// Write some configuration data to the SD card. Returns if successful.
bool output_write_header(header_data &);
// Write data to the SD card. If use_long is true, then long-term data is also written.
void output_write_data(bool use_long);

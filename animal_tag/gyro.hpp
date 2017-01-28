#ifndef __GYRO_HPP__
#define __GYRO_HPP__

#include <Arduino.h>
#include <SD.h>

// Size of software buffer (samples)
constexpr byte gyro_buffer_size = 16;
// Size of hardware buffer (samples)
constexpr byte gyro_fifo_size = 32;

void gyro_setup();

/*
 * Copy the gyroscope's internal FIFO buffer into the Arduino's
 * software buffer. This prepares it for writing.
 * 
 * If the gyro is not in active mode, this will print a warning and
 * do nothing.
 */
void gyro_read_all();

/*
 * Write both the software buffer and the current contents of the hardware
 * buffer to a file, in that order.
 * 
 * If the gyroscope is not in active mode, this will print a warning and
 * do nothing.
 */
void gyro_write(File f);

/*
 * Flush the gyroscope's buffer. Call this after writing.
 */
void gyro_reset();

/*
 * Set the gyroscope into standby or active mode.
 * Standby mode uses less power, but doesn't collect data and can't
 * be read from. Active mode is the opposite.
 * 
 * Switching from standby to active mode incurs a substantial delay of
 * up to 140ms, but the Arduino sleeps through this.
 */
void gyro_set_active(bool);
bool gyro_is_active();

byte gyro_size();
unsigned short gyro_write_size();
float gyro_scale();

#endif

#ifndef __GYRO_HPP__
#define __GYRO_HPP__

#include <Arduino.h>
#include <SD.h>
#include "fxas_2.h"

void gyro_setup(FXAS2::Range, FXAS2::ODR);

/*
 * Copy the gyroscope's internal FIFO buffer into the Arduino's
 * software buffer. This prepares it for writing.
 * 
 * If the gyro is not in active mode, this will print a warning and
 * do nothing.
 */
void gyro_read_all();

/*
 * Return if the gyroscope's software buffer is full.
 * 
 * If gyro_full() and the FIFO buffer is full, then data is ready to be
 * written to the SD card.
 */
bool gyro_full();

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

unsigned short gyro_write_size();
float gyro_scale();

#endif

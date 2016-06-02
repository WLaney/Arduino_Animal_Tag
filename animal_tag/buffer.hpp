/**
 * This interfaces with the "buffered" sensors (accelerometer and
 * gyroscope), holding their data so that it can be written to the
 * SD card in bursts.
 */
#ifndef BUFFER_H
#define BUFFER_H
#include <Arduino.h>
#include <SD.h>

void buffer_setup();
void buffer_update();
void buffer_write(File sd);
bool buffer_needs_write();

#endif

#ifndef TEMP_H
#define TEMP_H

#include <Arduino.h>
#include <SD.h>

void temp_setup();
void temp_update();
void temp_write(File sd);

#endif

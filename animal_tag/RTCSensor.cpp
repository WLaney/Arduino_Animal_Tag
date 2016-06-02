#include "ds3234.h"
#include "RTCSensor.hpp"
#include <Arduino.h>
#include <SD.h>
#include "debug.h"

void RTCSensor::setup() {
	DS3234_init(cs, DS3234_INTCN);
}

void RTCSensor::read() {
    DBGLN("Read RTC");
    DS3234_get(cs, &t);
}

void RTCSensor::flush(File sd) {
    DBGLN("Wrote time");
    
    sd.print(t.year);
    sd.print('-');
    sd.print(t.mon);
    sd.print('-');
    sd.print(t.mday);
    sd.print(' ');
    sd.print(t.hour);
    sd.print(':');
    sd.print(t.min);
    sd.print(':');
    sd.print(t.sec);
}

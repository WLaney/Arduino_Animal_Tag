#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>

// F() is a macro that designates a string to be stored in PROGMEM.
// Since it would otherwise take up global variable space, this is great.

#define DEBUG // Comment this line out to remove Serial prints
#ifdef DEBUG
  #define DBEGIN() Serial.begin(9600)
  #define DEND()   Serial.end()
  #define DBG(s)   Serial.print(F(s))
  #define DBGLN(s) Serial.println(F(s))
#else
  #define DBEGIN()
  #define DEND()
  #define DBG(s)
  #define DBGLN(s) 
#endif

#endif

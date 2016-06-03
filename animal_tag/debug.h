#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>

#define DEBUG // Comment this line out to remove Serial prints
#ifdef DEBUG
  #define DBEGIN()  Serial.begin(9600)
  #define DEND()    Serial.end()
  #define DBG(s)    Serial.print(s)
  #define DBGLN(s)  Serial.println(s)
  #define DBGSTR(s) Serial.println(F(s))
#else
  #define DBEGIN()
  #define DEND()
  #define DBG(s)
  #define DBGLN(s)
  #define DBGSTR(s)
#endif

#endif

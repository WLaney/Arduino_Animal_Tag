#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>

// If set to 1, this will clear the software buffers to 0 after each write.
// This can help catch buffer errors, and uses a minimal amount
// of power.
#define CLEAR_BUFFERS 1

// Comment this line out to remove Serial prints.
// This frees up space and may reduce power usage.
#define DEBUG
#ifdef DEBUG
  #define DBEGIN()  Serial.begin(9600)
  #define DEND()    Serial.end()
  #define DBG(s)    Serial.print(s)
  #define DBGLN(s)  Serial.println(s)
  #define DBGSTR(s) Serial.print(F(s))
#else
  #define DBEGIN()
  #define DEND()
  #define DBG(s)
  #define DBGLN(s)
  #define DBGSTR(s)
#endif

#endif

#include <Narcoleptic.h>

void setup() {
  
}

void loop() {
  Serial.begin(9600);
  Serial.println("delay()");
  Serial.println("=======");
  for (byte i = 0; i < 5; i++) {
    Serial.println("Hoy, small fry!");
    delay(1000);
  }

  Serial.println("Narcoleptic.delay()");
  Serial.println("===================");
  Serial.end();
  for (byte i = 0; i < 5; i++) {
    Serial.begin(9600);
    Serial.println("Hoy, small fry!");
    Serial.end();
    Narcoleptic.delay(1000);
  }
}

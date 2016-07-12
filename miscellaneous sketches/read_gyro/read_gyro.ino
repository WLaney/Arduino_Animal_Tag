#include <Light_L3GD20.h>
#include <Narcoleptic.h>

const byte blinkPin = 5;
auto pinState = LOW;

void setup() {
  Serial.begin(9600);
  Gyro::begin();

  pinMode(blinkPin, OUTPUT);
}

void loop() {
  Gyro::l3gd20Data_t d;
  Gyro::read(&d);
  Serial.print(d.x);
  Serial.print('\t');
  Serial.print(d.y);
  Serial.print('\t');
  Serial.println(d.z);

  pinState = (pinState == LOW) ? HIGH : LOW;
  digitalWrite(blinkPin, pinState);

  Serial.end();
  Narcoleptic.delay(70);
  Serial.begin(9600);
}

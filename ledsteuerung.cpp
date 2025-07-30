
#include <Arduino.h>
#include "ledsteuerung.h"

const int ledPin = LED_BUILTIN;  // Meist Pin 13

void initLED() {
  pinMode(ledPin, OUTPUT);
}

void blinkenMit1Hz() {
  digitalWrite(ledPin, HIGH);  // LED an
  delay(1000);                 // 1 Sekunde warten
  digitalWrite(ledPin, LOW);   // LED aus
  delay(1000);                 // 1 Sekunde warten
}

void setLED() {
  digitalWrite(ledPin, HIGH);  // LED an
}

void resetLED() {
  digitalWrite(ledPin, LOW);  // LED an
}
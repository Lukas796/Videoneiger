#include <Arduino.h>
#include "axis_control.h"

const int pins[] = {53, 52, 51, 50};
const int anzahlPins = 4;
static int aktuellerPin = 0;

void initPins() {
  for (int i = 0; i < anzahlPins; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
}

void schalteNaechstenPinMit1sDelay() {
  // Alle Pins auf LOW setzen
  for (int i = 0; i < anzahlPins; i++) {
    digitalWrite(pins[i], LOW);
  }

  // Aktuellen Pin auf HIGH setzen
  digitalWrite(pins[aktuellerPin], HIGH);

  delay(1000); // 1 Sekunde warten

  // Nächsten vorbereiten
  aktuellerPin++;
  if (aktuellerPin >= anzahlPins) {
    aktuellerPin = 0;
  }
}
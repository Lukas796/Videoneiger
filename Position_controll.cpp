// ---- Includes & Externe Deklarationen ----
#include "UART.h"   
extern int xPos;  
extern int yPos;  

// ---- Pin-Definitionen ----const int pins[] = {53, 52, 51, 50};
const int PIN_BTN_UP    = 53;
const int PIN_BTN_DOWN  = 52;
const int PIN_BTN_LEFT  = 51;
const int PIN_BTN_RIGHT = 50;

// ---- Steuerungs-Intervalle & Hysterese ----
const unsigned long CONTROL_INTERVAL = 50;  // ms zwischen zwei Regelaktionen

const int X_HYST_HIGH =  100;   // x > +5 → links drücken
const int X_HYST_LOW  = -100;   // x < -5 → rechts drücken
const int Y_HYST_HIGH =  100;   // y > +5 → unten drücken
const int Y_HYST_LOW  = -100;   // y < -5 → oben drücken

unsigned long lastControlTime = 0;





// ---- Arduino-Setup ----
void setup() {
  // Button-Pins als Ausgang
  pinMode(PIN_BTN_UP,    OUTPUT);
  pinMode(PIN_BTN_DOWN,  OUTPUT);
  pinMode(PIN_BTN_LEFT,  OUTPUT);
  pinMode(PIN_BTN_RIGHT, OUTPUT);

  // Sicherstellen: alle Buttons inaktiv
  digitalWrite(PIN_BTN_UP,    LOW);
  digitalWrite(PIN_BTN_DOWN,  LOW);
  digitalWrite(PIN_BTN_LEFT,  LOW);
  digitalWrite(PIN_BTN_RIGHT, LOW);

  // UART für x/y-Empfang starten (Passende Baudrate wählen)
  Serial.begin(115200);
}

// Drückt einen Button-Pin für die angegebene Dauer (ms)
void pressButton(int pin, int duration) {
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
}


// Haupt-Regelfunktion mit Hysterese
void regulatePosition(int x, int y) {
  unsigned long now = millis();
  if (now - lastControlTime < CONTROL_INTERVAL) return;
  lastControlTime = now;

  // X-Achse regeln
  if      (x >  X_HYST_HIGH) pressButton(PIN_BTN_LEFT,  50);
  else if (x <  X_HYST_LOW)  pressButton(PIN_BTN_RIGHT, 50);

  // Y-Achse regeln
  if      (y >  Y_HYST_HIGH) pressButton(PIN_BTN_DOWN,  50);
  else if (y <  Y_HYST_LOW)  pressButton(PIN_BTN_UP,    50);
}
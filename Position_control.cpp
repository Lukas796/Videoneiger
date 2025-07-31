// ---- Position_Control.ino ----

#include "UART.h"        // oder "UART.h", je nachdem, wie ihr eure Header-Datei nennt
extern int xPos;          // aus USART.cpp
extern int yPos;          // aus USART.cpp

// ---- Pin-Definitionen ----
const int PIN_BTN_UP    = 51;
const int PIN_BTN_DOWN  = 53 ;
const int PIN_BTN_LEFT  = 50;
const int PIN_BTN_RIGHT = 52;

// ---- Steuerungs-Intervalle & Hysterese ----
const unsigned long CONTROL_INTERVAL = 1000;   // ms zwischen zwei Regel­aktionen

const int X_HYST_HIGH =  100;   // x > +100 → links drücken
const int X_HYST_LOW  = -100;   // x < -100 → rechts drücken
const int Y_HYST_HIGH =  100;   // y > +100 → unten drücken
const int Y_HYST_LOW  = -100;   // y < -100 → oben drücken

unsigned long lastControlTime = 0;

// Drückt einen Button-Pin für 'duration' Millisekunden
void pressButton(int pin, int duration) {
  digitalWrite(pin, HIGH);
  //uart_sendText("Button aktiv");
  delay(duration);
  digitalWrite(pin, LOW);
}

// Zweipunkt-Regel mit Hysterese – verwendet direkt xPos/yPos
void regulatePosition() {
  unsigned long now = millis();
  if (now - lastControlTime < CONTROL_INTERVAL) return;
  lastControlTime = now;

  if (xPos > X_HYST_HIGH) {
    // Positivabweichung nach links korrigieren
    if (xPos > 1000) {
      pressButton(PIN_BTN_LEFT, 900);
    }
    else if (xPos > 500) {
      pressButton(PIN_BTN_LEFT, 700);
    }
    else {
      pressButton(PIN_BTN_LEFT, 500);
    }
  }
  else if (xPos < X_HYST_LOW) {
    // Negativabweichung nach rechts korrigieren
    int err = -xPos;  // Betrag
    if (err > 1000) {
      pressButton(PIN_BTN_RIGHT, 900);
    }
    else if (err > 500) {
      pressButton(PIN_BTN_RIGHT, 700);
    }
    else {
      pressButton(PIN_BTN_RIGHT, 500);
    }
  }

  // Y-Achse regeln
  if (yPos > Y_HYST_HIGH) {
    // Positivabweichung nach unten korrigieren
    if (yPos > 400) {
      pressButton(PIN_BTN_DOWN, 900);
    }
    else if (yPos > 200) {
      pressButton(PIN_BTN_DOWN, 700);
    }
    else {
      pressButton(PIN_BTN_DOWN, 500);
    }
  }
  else if (yPos < Y_HYST_LOW) {
    // Negativabweichung nach oben korrigieren
    int err = -yPos;
    if (err > 400) {
      pressButton(PIN_BTN_UP, 900);
    }
    else if (err > 200) {
      pressButton(PIN_BTN_UP, 700);
    }
    else {
      pressButton(PIN_BTN_UP, 500);
    }
  }
}


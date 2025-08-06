// ---- Position_Control.ino ----

#include "UART.h"        // oder "UART.h", je nachdem, wie ihr eure Header-Datei nennt
extern volatile int16_t xPos;
extern volatile int16_t yPos;

// ---- Pin-Definitionen ----
const int PIN_BTN_UP    = 51;
const int PIN_BTN_DOWN  = 53;
const int PIN_BTN_LEFT  = 50;
const int PIN_BTN_RIGHT = 52;


const int X_HYST_HIGH =  170;   // x > +100 → links drücken  --> good with Velocity 6 of V on PT500
const int X_HYST_LOW  = -150;   // x < -100 → rechts drücken --> good with Velocity 6 of H on PT500
const int Y_HYST_HIGH =  100;   // y > +100 → unten drücken
const int Y_HYST_LOW  = -100;   // y < -100 → oben drücken

// Statusvariablen
enum Axis { AXIS_X, AXIS_Y };
Axis currentAxis = AXIS_X;

// Merkt, ob gerade eine Taste gedrückt ist
int activePin = -1;


void pressAndHold(int pin) {
  if (activePin != pin) {
    // Falls vorher eine andere Taste gedrückt war -> loslassen
    if (activePin != -1) {
      digitalWrite(activePin, LOW);
    }
    // Neue Taste drücken
    digitalWrite(pin, HIGH);
    activePin = pin;
  }
}

void releaseButton() {
  if (activePin != -1) {
    digitalWrite(activePin, LOW);
    activePin = -1;
  }
}

void regulateContinuous() {
  if (currentAxis == AXIS_X) {
    // X-Achse regeln
    if (xPos > X_HYST_HIGH) {
      pressAndHold(PIN_BTN_LEFT);
    } 
    else if (xPos < X_HYST_LOW) {
      pressAndHold(PIN_BTN_RIGHT);
    } 
    else {
      // Zielbereich erreicht -> Taste loslassen und auf Y wechseln
      releaseButton();
      currentAxis = AXIS_Y;
    }
  } 
  else if (currentAxis == AXIS_Y) {
    // Y-Achse regeln
    if (yPos > Y_HYST_HIGH) {
      pressAndHold(PIN_BTN_DOWN);
    } 
    else if (yPos < Y_HYST_LOW) {
      pressAndHold(PIN_BTN_UP);
    } 
    else {
      // Zielbereich erreicht -> Taste loslassen und auf X wechseln
      releaseButton();
      currentAxis = AXIS_X;
    }
  }
}

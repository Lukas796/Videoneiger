// ---- Position_Control.ino ----
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Position_control.h"
#include "uart.h"
#include "spi.h"
#include "UART.h"        // oder "UART.h", je nachdem, wie ihr eure Header-Datei nennt
volatile int16_t xPos;
volatile int16_t yPos;

// ---- Pin-Definitionen ----
const int PIN_BTN_UP    = 51;
const int PIN_BTN_DOWN  = 53;
const int PIN_BTN_LEFT  = 50;
const int PIN_BTN_RIGHT = 52;


const int X_HYST_HIGH =  170;   // x > +100 → links drücken  --> good with Velocity 6 of V on PT500
const int X_HYST_LOW  = -170;   // x < -100 → rechts drücken --> good with Velocity 6 of H on PT500
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

uint8_t computeWiper(int16_t pos, int16_t hyst) {
    int16_t absPos = (pos < 0) ? -pos : pos;
    if (absPos <= hyst) return 128;

    int32_t range = MAX_POS - hyst;
    int32_t adj   = absPos - hyst;
    if (adj > range) adj = range;

    // skaliere [0…range] → [128…255]
    return (uint8_t)(128 + (adj * 127) / range);
}

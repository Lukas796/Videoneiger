#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Position_control.h"
#include "uart.h"
#include "spi.h"
#include "UART.h"        

// === Konfiguration ===
#define X_END_BEREICH 700
#define Y_END_BEREICH 350

#define HYSTERESE 90


const uint8_t POTI_POS_STOP = 128;

const uint8_t POTI_POS_VEL_8 = 0;
const uint8_t POTI_POS_VEL_7 = 10;
const uint8_t POTI_POS_VEL_6 = 20;
const uint8_t POTI_POS_VEL_5 = 30;
const uint8_t POTI_POS_VEL_4 = 40;
const uint8_t POTI_POS_VEL_3 = 50;
const uint8_t POTI_POS_VEL_2 = 60;
const uint8_t POTI_POS_VEL_1 = 70;

const uint8_t POTI_NEG_VEL_8 = 250;
const uint8_t POTI_NEG_VEL_7 = 240;
const uint8_t POTI_NEG_VEL_6 = 230;
const uint8_t POTI_NEG_VEL_5 = 220;
const uint8_t POTI_NEG_VEL_4 = 210;
const uint8_t POTI_NEG_VEL_3 = 200;
const uint8_t POTI_NEG_VEL_2 = 190;
const uint8_t POTI_NEG_VEL_1 = 180;

volatile int16_t xPos;
volatile int16_t yPos;

// === Potiwerte als Arrays für bessere Zuordnung ===
volatile uint8_t potiPosVel[8];
volatile uint8_t potiNegVel[8];

uint8_t getPotiPosValue(uint8_t velIndex) {
  switch (velIndex) {
    case 1: return POTI_POS_VEL_1;
    case 2: return POTI_POS_VEL_2;
    case 3: return POTI_POS_VEL_3;
    case 4: return POTI_POS_VEL_4;
    case 5: return POTI_POS_VEL_5;
    case 6: return POTI_POS_VEL_6;
    case 7: return POTI_POS_VEL_7;
    case 8: return POTI_POS_VEL_8;
    default: return POTI_POS_STOP;
  }
}

uint8_t getPotiNegValue(uint8_t velIndex) {
  switch (velIndex) {
    case 1: return POTI_NEG_VEL_1;
    case 2: return POTI_NEG_VEL_2;
    case 3: return POTI_NEG_VEL_3;
    case 4: return POTI_NEG_VEL_4;
    case 5: return POTI_NEG_VEL_5;
    case 6: return POTI_NEG_VEL_6;
    case 7: return POTI_NEG_VEL_7;
    case 8: return POTI_NEG_VEL_8;
    default: return POTI_POS_STOP;
  }
}

// === Berechnungsfunktionen ===
uint8_t calculatePotiValueX(int xPos) {
    if (abs(xPos) < HYSTERESE) {
        return POTI_POS_STOP;
    }

    int maxRange = X_END_BEREICH - HYSTERESE;
    int relValue = abs(xPos) - HYSTERESE;
    int step = maxRange / 8;

    int speedLevel = relValue / step;
    if (speedLevel > 7) speedLevel = 7;

    if (xPos > 0) {
        return potiPosVel[speedLevel];
    } else {
        return potiNegVel[speedLevel];
    }
}

uint8_t calculatePotiValueY(int yPos) {
    if (abs(yPos) < HYSTERESE) {
        return POTI_POS_STOP;
    }

    int maxRange = Y_END_BEREICH - HYSTERESE;
    int relValue = abs(yPos) - HYSTERESE;
    int step = maxRange / 8;

    int speedLevel = relValue / step;
    if (speedLevel > 7) speedLevel = 7;

    if (yPos > 0) {
        return potiPosVel[speedLevel];
    } else {
        return potiNegVel[speedLevel];
    }
}
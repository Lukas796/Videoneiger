#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Position_control.h"
#include "uart.h"
#include "spi.h"
#include "UART.h"        

// === Konfiguration ===
#define X_END_BEREICH 700
#define Y_END_BEREICH 250

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
const uint8_t potiPosVel[8] = {
    POTI_POS_VEL_3,
    POTI_POS_VEL_3,
    POTI_POS_VEL_3,
    POTI_POS_VEL_4,
    POTI_POS_VEL_5,
    POTI_POS_VEL_6,
    POTI_POS_VEL_7,
    POTI_POS_VEL_8
};

const uint8_t potiNegVel[8] = {
    POTI_NEG_VEL_3,
    POTI_NEG_VEL_3,
    POTI_NEG_VEL_3,
    POTI_NEG_VEL_4,
    POTI_NEG_VEL_5,
    POTI_NEG_VEL_6,
    POTI_NEG_VEL_7,
    POTI_NEG_VEL_8
};
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
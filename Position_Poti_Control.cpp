#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Position_control.h"
#include "uart.h"
#include "spi.h"
#include "UART.h"        


volatile int16_t xPos;
volatile int16_t yPos;


uint8_t calculatePotiValueX(int xPos) {
    if (abs(xPos) < 90) {
        return 128; // Stillstand
    }

    int speed = min(8, abs(xPos) / 100); // Geschwindigkeit von 1 bis 8
    if (xPos < 0) {
        // Bewegung nach links → Poti-Wert 180–255
        return 180 + (speed - 1) * 10; // z.B. speed 1 → 180, speed 8 → 250
    } else {
        // Bewegung nach rechts → Poti-Wert 0–80
        return 80 - (speed - 1) * 10; // z.B. speed 1 → 80, speed 8 → 10
    }
}

uint8_t calculatePotiValueY(int yPos) {
    if (abs(yPos) < 90) {
        return 128; // Stillstand
    }

    int speed = min(8, abs(yPos) / 30); // Geschwindigkeit von 1 bis 8
    if (yPos < 0) {
        // Bewegung nach unten → Poti-Wert 180–255
        return 180 + (speed - 1) * 10;
    } else {
        // Bewegung nach oben → Poti-Wert 0–80
        return 80 - (speed - 1) * 10;
    }
}
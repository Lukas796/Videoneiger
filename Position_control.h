#ifndef Position_control
#define Position_control

#include <Arduino.h>
#include <stdint.h>
void regulateContinuous();

// Hysterese- und Maximalwerte
#define HYST_X   10
#define HYST_Y   10
#define MAX_POS  1000   // beispielsweise ±512

// Globale Positionswerte (werden in .cpp definiert)
extern volatile int16_t xPos;
extern volatile int16_t yPos;

// API
uint8_t computeWiper(int16_t pos, int16_t hyst);

#endif // POSITION_CONTROL_H

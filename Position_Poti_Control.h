#ifndef Position_Poti_control
#define Position_Poti_control



// Globale Positionswerte (werden in .cpp definiert)
extern volatile int16_t xPos;
extern volatile int16_t yPos;

// === Globale Arrays ===
extern volatile uint8_t potiPosVel[8];
extern volatile uint8_t potiNegVel[8];

// === Funktionsprototypen ===
uint8_t getPotiPosValue(uint8_t velIndex);
uint8_t getPotiNegValue(uint8_t velIndex);

// API
uint8_t calculatePotiValueX(int xPos);
uint8_t calculatePotiValueY(int yPos);

#endif // Position_Poti_control_H
#ifndef Position_Poti_control
#define Position_Poti_control



// Globale Positionswerte (werden in .cpp definiert)
extern volatile int16_t xPos;
extern volatile int16_t yPos;

// API
uint8_t calculatePotiValueX(int xPos);
uint8_t calculatePotiValueY(int yPos);

#endif // Position_Poti_control_H
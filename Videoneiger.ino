#include "ledsteuerung.h"
#include "axis_control.h"
#include "UART.h"
#include "Position_control.h"



void setup() {
  initLED();
  USART_Init(115200);
  USART_SendString("UART bereit!");
  // Zeilenumbruch (CR+LF)
  USART_SendData('\r');
  USART_SendData('\n');
  initPins();
}

void loop() {
  uart_get_positions();   // aktualisiert xPos und yPos aus dem seriellen Puffer
  regulateContinuous();   // kontinuierliche Regelung 
}

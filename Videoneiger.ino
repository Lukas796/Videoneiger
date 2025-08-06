
#include "axis_control.h"
#include "UART.h"
#include "Position_control.h"

void setup() {
  uart_init(115200);
  uart_sendText("UART bereit!");
  initPins();
}

void loop() {
  uart_get_positions();   // aktualisiert xPos und yPos aus dem seriellen Puffer
 // regulatePosition();     // nutzt die neuen Werte
}

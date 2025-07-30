#include "ledsteuerung.h"
#include "axis_control.h"
#include "UART.h"

void setup() {
  initLED();
  uart_init(115200);
  uart_sendText("UART bereit!");
}

void loop() {
 // uart_test_programm();
  uart_get_positions();
}
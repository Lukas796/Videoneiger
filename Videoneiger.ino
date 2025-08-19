#include "ledsteuerung.h"
#include "axis_control.h"
#include "UART.h"
#include "Position_control.h"  
#include "Position_Poti_control.h"  
#include "SPI.h"
#include <util/delay.h>
#include <avr/interrupt.h>


void setup() {
  initLED();
  SPI_Init();
  USART_Init(115200);
  USART_SendString("UART bereit!");
  USART_SendData('\r');
  USART_SendData('\n');
  initPins();
  sei();
}

void loop() {
  uart_get_positions();
  
  //uint8_t wX = computeWiper(xPos, HYST_X);
  uint8_t potiX = calculatePotiValueX(xPos);
  SPI_Transfer(true,  0x00, potiX);

  //uint8_t wY = computeWiper(yPos, HYST_Y);
  uint8_t potiY = calculatePotiValueY(yPos);
  SPI_Transfer(false, 0x00, potiY);

  //   // Automatischer Sweep von 0 bis 255
  // const uint8_t w = 90;             // 0…255, hier Mittelstellung
  // SPI_Transfer(true,  0x00, w);      // X-Poti
  // SPI_Transfer(false, 0x00, w);

  // // Kurze Pause und erneuter Sweep
  // _delay_ms(1000);

}


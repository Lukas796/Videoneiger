#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stdbool.h>

/* 
   init: richtet MOSI (PB3), SCK (PB5), CS_X (PB2) und CS_Y (PB1) als Ausgänge ein.
   transfer: wählt X- oder Y-Pot, sendet command + data via SPI-Bit-Bang.
*/
void SPI_Init(void);
void SPI_Transfer(bool selectX, uint8_t command, uint8_t data);

#endif // SPI_H
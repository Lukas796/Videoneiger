#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"

/* 
   PB3 = MOSI
   PB5 = SCK
   PB2 = CS für X-Pot
   PB1 = CS für Y-Pot
*/

void SPI_Init(void)
{
    // alle relevanten Pins als Ausgang setzen
    DDRB |= (1<<PB3);  // MOSI
    DDRB |= (1<<PB5);  // SCK
    DDRB |= (1<<PB2);  // CS_X
    DDRB |= (1<<PB1);  // CS_Y

    // Idle-Zustand: MOSI=0, SCK=0, CS_X=1, CS_Y=1
    PORTB &= ~(1<<PB3);
    PORTB &= ~(1<<PB5);
    PORTB |=  (1<<PB2);
    PORTB |=  (1<<PB1);
}

// sendet ein Byte MSB-first per Bit-Bang
static void SPI_WriteByte(uint8_t value)
{
    for (int8_t i = 7; i >= 0; --i) {
        // Clock low
        PORTB &= ~(1<<PB5);

        // Datenbit auf MOSI
        if (value & (1<<i)) PORTB |=  (1<<PB3);
        else                PORTB &= ~(1<<PB3);

        _delay_us(1);

        // Clock high → Slave liest Bit
        PORTB |=  (1<<PB5);
        _delay_us(1);
    }
}

void SPI_Transfer(bool selectX, uint8_t command, uint8_t data)
{
    // CS aktivieren (low)
    if (selectX) PORTB &= ~(1<<PB2);
    else         PORTB &= ~(1<<PB1);

    // Bytes senden
    SPI_WriteByte(command);
    SPI_WriteByte(data);

    // CS wieder deaktivieren (high)
    if (selectX) PORTB |= (1<<PB2);
    else         PORTB |= (1<<PB1);
}
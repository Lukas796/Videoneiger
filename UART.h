#ifndef UART_H
#define UART_H

#include <Arduino.h>

extern volatile int16_t xPos;
extern volatile int16_t yPos;

void USART_Init(unsigned long baud);
void USART_SendData(uint8_t data);
void USART_SendString(const char *str);
bool uart_data_available(void);
uint8_t uart_read_data(void);
void uart_read_line(char *buffer, uint8_t max_length);
void uart_get_positions(void);
void uart_wait_for_config(void);


//bool uart_available();
//String uart_readLine();
//void uart_sendText(const String& text);

void uart_test_programm ();
#endif
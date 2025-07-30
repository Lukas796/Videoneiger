#ifndef UART_H
#define UART_H

#include <Arduino.h>

extern int xPos;
extern int yPos;

void uart_init(unsigned long baudrate);
bool uart_available();
String uart_readLine();
void uart_sendText(const String& text);

void uart_test_programm ();
void uart_get_positions();
#endif
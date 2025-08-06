#include "uart.h"
#include "ledsteuerung.h"

int xPos = 0;
int yPos = 0;

void uart_init(unsigned long baudrate) {
  Serial.begin(baudrate);
  Serial.setTimeout(20); // kurze Timeout-Zeit (standard ist 1000ms!)
  while (!Serial) {
    ; // Warten bis Serial bereit ist 
  }
}

bool uart_available() {
  return Serial.available() > 0;
}

String uart_readLine() {
  if (!uart_available()) return "";
  String input = Serial.readStringUntil('\n');
  input.trim(); // Zeilenumbruch und Leerzeichen entfernen
  return input;
}

void uart_sendText(const String& text) {
  Serial.println(text);
}


void uart_get_positions() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');

    // Extrahiere X und Y (optional)
    int x = msg.substring(msg.indexOf("X:") + 2, msg.indexOf(",Y:")).toInt();
    int y = msg.substring(msg.indexOf("Y:") + 2).toInt();
   
    // Speichere in die globalen Variablen
    xPos = x;
    yPos = y;
  
    Serial.println(":X = " + String(x) + ", Y = " + String(y));
  }
}
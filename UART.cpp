#include "uart.h"
#include "ledsteuerung.h"
#include <avr/interrupt.h>
#include <avr/io.h>


int xPos = 0;
int yPos = 0;

// === UART Ringpuffer ===
#define USART_BUFFER_SIZE 64
volatile uint8_t usart_rx_buffer[USART_BUFFER_SIZE];
volatile uint8_t usart_rx_head = 0;
volatile uint8_t usart_rx_tail = 0;


void USART_Init(unsigned long baud) {
  uint16_t ubrr = F_CPU/16/baud - 1;    // UBRR-Wert berechnen
  UBRR0H = (uint8_t)(ubrr >> 8);        // obere 8 Bit
  UBRR0L = (uint8_t)ubrr;               // untere 8 Bit

  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);// 8 Datenbits, kein Parity, 1 Stopbit
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);  // TX und RX aktiv
}


//bool uart_available() {
//  return Serial.available() > 0;
//}

//String uart_readLine() {
//  if (!uart_available()) return "";
//  String input = Serial.readStringUntil('\n');
//  input.trim(); // Zeilenumbruch und Leerzeichen entfernen
//  return input;
//}

//void uart_sendText(const String& text) {
//  Serial.println(text);
//}

void USART_SendData(uint8_t data) {													// Sendet ein einzelnes Byte über die USART-Schnittstelle  
	while (!(UCSR0A & (1 << UDRE0)));												// Wartet, bis das USART-Datenregister leer ist (bereit zur Übertragung)  
	UDR0 = data;																	// Schreibt das Datenbyte in das Datenregister, wodurch die Übertragung gestartet wird  
}  

// String senden  
void USART_SendString(const char *str) {											// Sendet einen nullterminierten String über USART  
	while (*str) {																	// Solange das aktuelle Zeichen nicht der Nullterminator ist  
		USART_SendData(*str++);														// Sendet das Zeichen und inkrementiert den Zeiger auf den nächsten Buchstaben  
	}  
}

// --- ISR: füllt Ringpuffer bei UART-Empfang ---
ISR(USART0_RX_vect) {
  uint8_t data = UDR0;  // liest UART-Datenregister
  uint8_t next = (usart_rx_head + 1) % USART_BUFFER_SIZE;
  if (next != usart_rx_tail) {
    usart_rx_buffer[usart_rx_head] = data;
    usart_rx_head = next;
  }
}

// --- Prüfen, ob Daten im Puffer sind ---
bool uart_data_available() {
  return (usart_rx_head != usart_rx_tail);
}

// --- Ein Byte aus Ringpuffer lesen ---
uint8_t uart_read_data() {
  if (usart_rx_head == usart_rx_tail) return 0;
  uint8_t data = usart_rx_buffer[usart_rx_tail];
  usart_rx_tail = (usart_rx_tail + 1) % USART_BUFFER_SIZE;
  return data;
}

// --- Liest eine Zeile bis '\n' in buffer (incl. '\n') ---
void uart_read_line(char *buffer, uint8_t max_length) {
  uint8_t i = 0;
  char c;
  do {
    while (!uart_data_available())
      ;  // blockiere bis Zeichen da
    c = uart_read_data();
    if (c == '\r') continue;  // CR überspringen
    if (i < max_length - 1) buffer[i++] = c;
  } while (c != '\n');
  buffer[i - 1] = '\0';  // LF durch Terminator ersetzen
}


//void uart_test_programm() {
//  if (uart_available()) {
//    String cmd = uart_readLine();
//
//   if (cmd == "LED ON") {
//      setLED();
//      uart_sendText("LED an");
//    } else if (cmd == "LED OFF") {
//      resetLED();
//      uart_sendText("LED aus");
//    } else {
//      uart_sendText("Unbekannter Befehl: " + cmd);
//    }
//  }
//}

static const int testPoints[4][2] = {
  { -300, 0 },  // links
  { 0, 300 },   // oben
  { 300, 0 },   // rechts
  { 0, -300 }   // unten
};

// Merker für den nächsten Testpunkt
static uint8_t testIndex = 0;


void uart_get_positions() {
  // Empfangs-Modus
  if (uart_data_available()) {
    char msg[64];
    uart_read_line(msg, sizeof(msg));


    //Serial.println("Empfangen: " + msg);

    int x = 0, y = 0;
    // Pattern "X:%d,Y:%d" erkennen
    if (sscanf(msg, "X:%d,Y:%d", &x, &y) == 2) {
      xPos = x;
      yPos = y;

      if (x >= 250) setLED();
      else          resetLED();

      char buf[12];                              // Puffer für Zahlen als String

      // Text vor der Zahl
      USART_SendString("X = ");

      // x in ASCII umwandeln und senden
      itoa(x, buf, 10);                          // Basis 10
      USART_SendString(buf);

      // Zwischen­text
      USART_SendString(", Y = ");

      // y in ASCII umwandeln und senden
      itoa(y, buf, 10);
      USART_SendString(buf);

      // Zeilenumbruch (CR+LF)
      USART_SendData('\r');
      USART_SendData('\n');
    }
  }
  //else{
  //xPos = testPoints[testIndex][0];
  //yPos = testPoints[testIndex][1];
  //testIndex = (testIndex + 1) % 4;
  //}
}
#include "uart.h"
#include "ledsteuerung.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "position_control.h"
#include "Position_Poti_control.h"  

// === UART Ringpuffer ===
#define USART_BUFFER_SIZE 64
volatile uint8_t usart_rx_buffer[USART_BUFFER_SIZE];
volatile uint8_t usart_rx_head = 0;
volatile uint8_t usart_rx_tail = 0;

unsigned long lastUpdateTime = 0;
const unsigned long timeoutDuration = 500; // Zeit in ms ohne neue Daten → Stillstand

void USART_Init(unsigned long baud) {
  // Double-Speed aktivieren für höhere Baudraten
  UCSR0A = (1 << U2X0);

  // Baudraten-Berechnung im Double-Speed-Modus
  uint16_t ubrr = F_CPU/8/baud - 1;
  UBRR0H = ubrr >> 8;
  UBRR0L = ubrr & 0xFF;

  // Frame Format: 8N1
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  // TX, RX und RX-Complete-Interrupt aktivieren
  UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

  sei(); // Globale Interrupts freigeben
}


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

static const int testPoints[4][2] = {
  { -300, 0 },  // links
  { 0, 300 },   // oben
  { 300, 0 },   // rechts
  { 0, -300 }   // unten
};

// Merker für den nächsten Testpunkt
static uint8_t testIndex = 0;


void parseConfigLine(const char* line) {
  if (strncmp(line, "POS_TABLE:", 10) == 0) {
    const char* data = line + 10;
    for (int i = 0; i < 8; i++) {
      potiPosVel[i] = getPotiPosValue(atoi(data));
      data = strchr(data, ',');
      if (!data && i < 7) break;
      data++;
    }
  } else if (strncmp(line, "NEG_TABLE:", 10) == 0) {
    const char* data = line + 10;
    for (int i = 0; i < 8; i++) {
      potiNegVel[i] = getPotiNegValue(atoi(data));
      data = strchr(data, ',');
      if (!data && i < 7) break;
      data++;
    }
  }
}

// void debugPrintTables() {
//   char buf[32];

//   USART_SendString("DEBUG_POS: ");
//   for (int i = 0; i < 8; i++) {
//     itoa(potiPosVel[i], buf, 10);
//     USART_SendString(buf);
//     if (i < 7) USART_SendString(",");
//   }
//   USART_SendString("\r\n");

//   USART_SendString("DEBUG_NEG: ");
//   for (int i = 0; i < 8; i++) {
//     itoa(potiNegVel[i], buf, 10);
//     USART_SendString(buf);
//     if (i < 7) USART_SendString(",");
//   }
//   USART_SendString("\r\n");

//   USART_SendString("CONFIG_OK\r\n");
// }

// void uart_wait_for_config() {
//   char line[64];
//   bool posReceived = false;
//   bool negReceived = false;
//   bool configDone = false;

//   //USART_SendString("UART bereit!\n");  // ⚠️ Damit OrangePi auf Start warten kann

//   while (!configDone) {
//     uart_read_line(line, sizeof(line));

//     USART_SendString("Empfangen: ");
//     USART_SendString(line);
//     USART_SendString("\r\n");

//     if (strncmp(line, "POS_TABLE:", 10) == 0) {
//       parseConfigLine(line);
//       posReceived = true;
//       USART_SendString("ACK_POS\r\n");
//     } 
//     else if (strncmp(line, "NEG_TABLE:", 10) == 0) {
//       parseConfigLine(line);
//       negReceived = true;
//       USART_SendString("ACK_NEG\r\n");

//       // 🟡 Debug-Ausgabe der eingelesenen Werte
//       debugPrintTables();
//     } 
//   }
// }

void uart_wait_for_config() {
  char line[64];
  bool posReceived = false;
  bool negReceived = false;

  while (!(posReceived && negReceived)) {
    uart_read_line(line, sizeof(line));

    USART_SendString("Empfangen: ");
    USART_SendString(line);
    USART_SendString("\r\n");

    if (strncmp(line, "POS_TABLE:", 10) == 0) {
      parseConfigLine(line);  // setzt potiPosVel[]
      posReceived = true;
      USART_SendString("ACK_POS\r\n");
    } 
    else if (strncmp(line, "NEG_TABLE:", 10) == 0) {
      parseConfigLine(line);  // setzt potiNegVel[]
      negReceived = true;
      USART_SendString("ACK_NEG\r\n");
    }
  }

  // 🔽 Ausgabe beider Tabellen, sobald beide empfangen wurden
  char buf[32];

  USART_SendString("DEBUG_POS: ");
  for (int i = 0; i < 8; i++) {
    itoa(potiPosVel[i], buf, 10);
    USART_SendString(buf);
    if (i < 7) USART_SendString(",");
  }
  USART_SendString("\r\n");

  USART_SendString("DEBUG_NEG: ");
  for (int i = 0; i < 8; i++) {
    itoa(potiNegVel[i], buf, 10);
    USART_SendString(buf);
    if (i < 7) USART_SendString(",");
  }
  USART_SendString("\r\n");

  USART_SendString("CONFIG_OK\r\n");
}

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

      lastUpdateTime = millis();  // ⏱ Zeit merken

      char buf[12];                              // Puffer für Zahlen als String

      // Text vor der Zahl
      USART_SendString("X:");

      // x in ASCII umwandeln und senden
      itoa(x, buf, 10);                          // Basis 10
      USART_SendString(buf);

      // Zwischen­text
      USART_SendString(",Y:");

      // y in ASCII umwandeln und senden
      itoa(y, buf, 10);
      USART_SendString(buf);

      // Zeilenumbruch (CR+LF)
      USART_SendData('\r');
      USART_SendData('\n');
    }
  }
  // ⏱ Timeout-Check (auch wenn keine Daten verfügbar sind)
  unsigned long now = millis();
  if (now - lastUpdateTime > timeoutDuration) {
    xPos = 0;
    yPos = 0;
  }
}
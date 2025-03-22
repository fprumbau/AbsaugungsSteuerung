#include <Wire.h>
#include <SPI.h>
#include <SX126XLT.h>  // SX12XX-LoRa Bibliothek
#include "SSD1306Wire.h"

// Pin-Definitionen für Heltec WiFi LoRa 32 V3 (aus Forum-Posting)
#define SDA_OLED 17
#define SCL_OLED 18
#define RST_OLED 21
#define SS_LoRa 8
#define SCK_LoRa 9
#define MOSI_LoRa 10
#define MISO_LoRa 11
#define RST_LoRa 12
#define BUSY_LoRa 13
#define DIO1_LoRa 14
#define SW_LoRa -1

// OLED-Display initialisieren
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);

// LoRa-Instanz
SX126XLT LT;

// Status-Variablen
String status = "ON";
String scrollText = "LoRa-Test";
int scrollOffset = 0;

void VextON() {
  pinMode(36, OUTPUT);  // Vext auf GPIO 36
  digitalWrite(36, LOW);
}

void VextOFF() {
  pinMode(36, OUTPUT);
  digitalWrite(36, HIGH);
}

void displayReset() {
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);  // Reset aktivieren
  delay(10);                    // Längeres Timing
  digitalWrite(RST_OLED, HIGH); // Reset deaktivieren
  delay(10);                    // Wartezeit nach Reset
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("AbsaugungsSteuerung startet...");

  // Heltec-spezifische Display-Steuerung
  VextON();
  displayReset();

  // I2C initialisieren
  Serial.println("I2C initialisieren...");
  Wire.begin(SDA_OLED, SCL_OLED);
  Serial.println("I2C initialisiert");

  // Display initialisieren
  Serial.println("Display initialisieren...");
  if (!display.init()) {
    Serial.println("OLED-Initialisierung fehlgeschlagen!");
    status = "ERR";
    scrollText = "OLED-Fehler";
  } else {
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    Serial.println("OLED initialisiert");
    // Test-Anzeige direkt nach init
    display.clear();
    display.drawString(0, 0, "Test");
    display.display();
    delay(1000);  // 1 Sekunde warten
  }

  // SPI initialisieren
  Serial.println("SPI initialisieren mit Heltec-Pins...");
  SPI.begin(SCK_LoRa, MISO_LoRa, MOSI_LoRa, SS_LoRa);
  Serial.println("SPI initialisiert");

  // LoRa initialisieren
  Serial.println("LoRa konfigurieren...");
  if (!LT.begin(SS_LoRa, RST_LoRa, BUSY_LoRa, DIO1_LoRa, SW_LoRa, DEVICE_SX1262)) {
    Serial.println("LoRa-Initialisierung fehlgeschlagen!");
    status = "ERR";
    scrollText = "LoRa-Fehler";
  } else {
    Serial.println("LoRa initialisiert erfolgreich!");
  }
}

void loop() {
  Serial.println("Loop Start...");

  // Minimaler Display-Test
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  if (!display.drawString(0, 0, "ON")) {
    Serial.println("Display-Fehler bei Status");
  }
  display.setFont(ArialMT_Plain_10);
  String timeText = "Time: " + String(millis() / 1000) + "s";
  if (!display.drawString(0, 50, timeText)) {
    Serial.println("Display-Fehler bei Zeit");
  }
  display.display();

  Serial.println("Display aktualisiert...");
  delay(200);
}
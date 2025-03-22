#include <Wire.h>
#include <LoRa.h>
#include "SSD1306Wire.h"
#include "pins_arduino.h"

// Pin-Definitionen für Heltec WiFi LoRa 32 V3.2
#define TASTER_PIN 0  // Druckschalter an GPIO 0

// OLED-Display initialisieren (integriert im Heltec-Board)
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);

// Status-Variablen
String status = "ON";  // Startstatus
String scrollText = "Warte auf LoRa - Bereit";  // Initialer Dummy-Text
int scrollOffset = 0;

void VextON() {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF() {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}

void displayReset() {
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, HIGH);
  delay(1);
  digitalWrite(RST_OLED, LOW);
  delay(1);
  digitalWrite(RST_OLED, HIGH);
  delay(1);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("AbsaugungsSteuerung startet...");

  // Heltec-spezifische Display-Steuerung
  VextON();
  displayReset();

  // Display initialisieren
  if (!display.init()) {
    Serial.println("OLED-Initialisierung fehlgeschlagen!");
    while (1) delay(100);
  }
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);  // Große Schrift für Status
  Serial.println("OLED initialisiert");

  // LoRa initialisieren
  if (!LoRa.begin(868E6)) {  // 868 MHz für Europa
    Serial.println("LoRa-Initialisierung fehlgeschlagen!");
    while (1) delay(100);
  }
  Serial.println("LoRa initialisiert");

  pinMode(TASTER_PIN, INPUT_PULLUP);
}

void loop() {
  // LoRa-Nachricht empfangen
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String message = "";
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }
    status = "ABS";  // Beispiel: Bei Empfang auf "ABS" wechseln
    scrollText = "Empfangen: " + message + " - RSSI: " + String(LoRa.packetRssi());
    scrollOffset = 0;  // Scroll-Reset bei neuer Nachricht
    Serial.println("LoRa-Nachricht: " + message);
  }

  // Display aktualisieren
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, status);  // Hauptstatus groß

  display.setFont(ArialMT_Plain_10);  // Kleinere Schrift für Textzeile
  int textWidth = display.getStringWidth(scrollText);
  if (textWidth > 128) {  // Scrollen, wenn Text zu lang
    display.drawString(-scrollOffset, 50, scrollText);
    scrollOffset = (scrollOffset + 1) % (textWidth + 128);  // Kontinuierlicher Scroll
  } else {
    display.drawString(0, 50, scrollText);  // Statisch, wenn kurz genug
  }
  display.display();

  delay(200);  // 5 Hz Aktualisierungsrate
}
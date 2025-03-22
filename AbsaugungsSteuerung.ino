#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include "SSD1306Wire.h"
#include "pins_arduino.h"

// Pin-Definitionen für Heltec WiFi LoRa 32 V3.2
#define TASTER_PIN 0  // Druckschalter an GPIO 0
#define LORA_NSS 18   // NSS (Chip Select)
#define LORA_RST 14   // Reset
#define LORA_DIO0 26  // DIO0 (Interrupt)
#define LORA_SCK 5    // SPI SCK
#define LORA_MISO 19  // SPI MISO
#define LORA_MOSI 27  // SPI MOSI

// OLED-Display initialisieren
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);

// Status-Variablen
String status = "ON";
String scrollText = "Warte auf LoRa - Bereit";
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

  // I2C explizit initialisieren
  Serial.println("I2C initialisieren...");
  Wire.begin(SDA_OLED, SCL_OLED);
  Serial.println("I2C initialisiert");

  // Display initialisieren
  if (!display.init()) {
    Serial.println("OLED-Initialisierung fehlgeschlagen!");
    status = "ERR";
    scrollText = "OLED-Fehler";
  } else {
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    Serial.println("OLED initialisiert");
  }

  // LoRa-Pins komplett auskommentiert
  /*
  Serial.println("Setze NSS...");
  pinMode(LORA_NSS, OUTPUT);
  digitalWrite(LORA_NSS, HIGH);
  Serial.println("NSS gesetzt");

  Serial.println("Setze RST...");
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, HIGH);
  Serial.println("RST gesetzt");

  Serial.println("Setze DIO0...");
  pinMode(LORA_DIO0, INPUT);
  Serial.println("DIO0 gesetzt");

  Serial.println("LoRa konfigurieren...");
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
  Serial.println("LoRa-Pins gesetzt");

  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa-Initialisierung fehlgeschlagen!");
    status = "ERR";
    scrollText = "LoRa-Fehler - Prüfe Hardware";
  } else {
    Serial.println("LoRa initialisiert");
  }
  */

  pinMode(TASTER_PIN, INPUT_PULLUP);
}

void loop() {
  // Dummy-Status
  status = "TEST";
  scrollText = "I2C-Test - Läuft stabil";

  // Display aktualisieren mit Fehlerprüfung
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  if (!display.drawString(0, 0, status)) {
    Serial.println("Display-Fehler bei Status");
  }

  display.setFont(ArialMT_Plain_10);
  int textWidth = display.getStringWidth(scrollText);
  if (textWidth > 128) {
    if (!display.drawString(-scrollOffset, 50, scrollText)) {
      Serial.println("Display-Fehler bei Scrolltext");
    }
    scrollOffset = (scrollOffset + 1) % (textWidth + 128);
  } else {
    if (!display.drawString(0, 50, scrollText)) {
      Serial.println("Display-Fehler bei statischem Text");
    }
  }

  display.display();

  delay(200);
}
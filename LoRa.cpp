#include "LoRa.h"

LoRa::LoRa() {}

bool LoRa::init() {
  SPI.begin(SCK, MISO, MOSI, SS_PIN);
  if (!LT.begin(SS_PIN, RST_PIN, BUSY_PIN, DIO1_PIN, SW_PIN, DEVICE_SX1262)) {
    debugPrint(DEBUG_LORA, "LoRa-Initialisierung fehlgeschlagen!");
    return false;
  }
  LT.setupLoRa(FREQUENCY, 0, SPREADING_FACTOR, BANDWIDTH, CODE_RATE, LDRO_AUTO);
  debugPrint(DEBUG_LORA, "LoRa initialisiert erfolgreich!");
  return true;
}

bool LoRa::send(const String& message, uint16_t timeout, int8_t txPower) {
  uint8_t buffer[256];
  int len = message.length();
  if (len > 255) len = 255;
  message.getBytes(buffer, len + 1);
  if (LT.transmit(buffer, len, timeout, txPower, WAIT_TX)) {
    debugPrint(DEBUG_LORA, "Nachricht gesendet: " + message);
    return true;
  } else {
    Serial.println("Fehler beim Senden!");
    return false;
  }
}

bool LoRa::send(uint8_t& sensorId, uint8_t action) {
  uint8_t buffer[2] = {sensorId, action};
  if (LT.transmit(buffer, 2, 1000, TXpower, WAIT_TX)) {
    debugPrint(DEBUG_LORA, "Sent: sensor" + String(sensorId) + ": " + String(actionToString(action)));
    return true;
  } else {
    debugPrint(DEBUG_LORA, "Send failed: sensor" + String(sensorId) + ": " + String(actionToString(action)));
    return false;
  }
}

bool LoRa::receive(uint8_t& sensorId, uint8_t& action) {
  uint8_t buffer[2];
  if (LT.receive(buffer, 2, 100, WAIT_RX)) {
    sensorId = buffer[0];
    action = buffer[1];
    debugPrint(DEBUG_LORA, "Received: sensor" + String(sensorId) + ": " + String(actionToString(action)));
    return true;
  }
  return false;
}

const char* LoRa::actionToString(uint8_t action) {
  switch (action) {
    case STARTE: return "starte";
    case STOPPE: return "stoppe";
    case STARTED: return "gestartet";
    case STOPPED: return "gestoppt";
    case ACK: return "ack";
    case QUERY: return "query";
    default: return "unknown";
  }
}

LoRa lora;
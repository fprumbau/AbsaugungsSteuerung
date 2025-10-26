#include "global.h"

// Status-Variablen
String status = "ON";
String scrollText = "LoRa-Test";
int scrollOffset = 0;
unsigned long lastPacketTime = 0;  // Zeit des letzten empfangenen Pakets
uint8_t confirmAction = 0;

void setup() {
  debugLevel = LORA_MSGS;

  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("Absaugung startet...");

  if (!oled.init()) {
    Serial.println("Display initialization failed!");
    while (1);
  }
  oled.clear();
  oled.drawString(0, 0, "Absaugungs startet...");
  oled.display();

  if (!lora.init()) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }

  if (!config.load()) {
    debugPrint(DEBUG_CONFIG, "Config load failed, setting and saving new values");
    config.setValue("ssid", "P...y", true);
    config.setValue("pass", "5...7", true);
  }

  debugPrint(DEBUG_WIFI, "Starting WiFi with SSID=" + String(config.getSSID()) + ", Pass=" + String(config.getPass()));
  wifi.begin(config.getSSID(), config.getPass());
  //oled.clear();

  updater.setup();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    wifi.resetTimeout();
    String html = "<html><body><h1>WiFi Config</h1>";
    html += "<form action='/config' method='post'>";
    html += "SSID: <input type='text' name='ssid' value='" + String(config.getSSID()) + "'><br>";
    html += "Pass: <input type='text' name='pass' value='" + String(config.getPass()) + "'><br>";
    html += "<input type='submit' value='Save'></form>";
    html += "<a href='/ota'>OTA Update</a></body></html>";
    request->send(200, "text/html", html);
    debugPrint(DEBUG_WIFI, "Main page accessed");
  });
}

void loop() {
    debugPrint(DEBUG_INIT, "Loop Start...");

    if(updater.restartRequired) {
      delay(2000);
      ESP.restart();
    }
    if (updater.getUpdating()) {
      updater.loop();
      oled.clear();
      oled.drawString(0, 0, "OTA Update läuft...");
      oled.display();
      return;
    }

    static unsigned long lastConfirmTime = 0;
    static uint8_t confirmCount = 0;
    static uint8_t lastSensorId = 0;
    static bool confirming = false;

    uint8_t sensorId, action;
    if (lora.receive(sensorId, action)) {
        if (action == STARTE) { 
            debugPrint(LORA_MSGS, "\nReceived from sensor" + String(sensorId) + ": starte, sending started");
            confirming = true;
            lastSensorId = sensorId;
            confirmCount = 0;
            confirmAction = STARTED;
            lastConfirmTime = 0; // Sofort erste Bestätigung senden
            absaugung.currentSensor = sensorId;
        }
        if (action == STOPPE) { 
            debugPrint(LORA_MSGS, "\nReceived from sensor" + String(sensorId) + ": stoppe, sending stopped");
            confirming = true;
            lastSensorId = sensorId;
            confirmCount = 0;
            confirmAction = STOPPED;
            lastConfirmTime = 0; // Sofort erste Bestätigung senden
            absaugung.currentSensor = 0;
        }        
        if (action == ACK) { 
            debugPrint(LORA_MSGS, "     Received from sensor" + String(sensorId) + ": ack, stopping confirmations");
            confirming = false;
        }
        if (action == QUERY) { 
            debugPrint(LORA_MSGS, "     Query from sensor" + String(sensorId) + ": sending status");
            if(absaugung.currentSensor == sensorId) {
              lora.send(sensorId, STARTED);
            } else {
              lora.send(sensorId, STOPPED);
            }
        }        
    }

    if (confirming && (millis() - lastConfirmTime >= 2000)) {
        
        if (confirmCount < 3) {
            if (lora.send(lastSensorId, confirmAction)) { 
                debugPrint(LORA_MSGS, "     Confirmation sent to sensor" + String(lastSensorId) + ": " + lora.actionToString(confirmAction) + " (" + String(confirmCount + 1) + "/3)");
                confirmCount++;
                lastConfirmTime = millis();
            } else {
                debugPrint(LORA_MSGS, "     Confirmation send failed to sensor" + String(lastSensorId));
            }
        }
        if (confirmCount >= 3) {
            debugPrint(LORA_MSGS, "     Confirmation count surpassed for sensor" + String(lastSensorId));
            confirming = false;
            confirmCount = 0;
        }
    }

    oled.updateScreen();

    debugPrint(DEBUG_INIT,"Loop Ende...");
    //delay(5); 
}
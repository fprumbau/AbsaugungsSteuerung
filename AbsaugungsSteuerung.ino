#include "global.h"

// Status-Variablen
String status = "ON";
String scrollText = "LoRa-Test";
int scrollOffset = 0;
unsigned long lastPacketTime = 0;  // Zeit des letzten empfangenen Pakets

void setup() {
  debugLevel = LORA_MSGS | DEBUG_CONFIG | DEBUG_WIFI;

  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("AbsaugungsSteuerung startet...");

  if (!oled.init()) {
    Serial.println("Display initialization failed!");
    while (1);
  }
  oled.clear();
  oled.drawString(0, 0, "AbsaugungsSteuerung startet...");
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
  oled.drawString(0, 13, "WiFi-Modus aktiviert");
  oled.display();

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

  if (updater.getUpdating()) {
    updater.loop();
    oled.clear();
    oled.drawString(0, 0, "OTA Update läuft...");
    oled.display();
    return;
  }


  debugPrint(DEBUG_INIT,"Loop Ende...");
  delay(500);
}
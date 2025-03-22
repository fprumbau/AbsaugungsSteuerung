# AbsaugungsSteuerung
Zentraler Steuerhub für die Absaugung mit Heltec WiFi LoRa 32 V3.2. Visualisiert LoRa-Anweisungen auf einem OLED-Display.

## Hardware
- Heltec WiFi LoRa 32 V3.2
- Druckschalter (25x25 mm, handschuhfreundlich)
- CR2450-Batterie

## Funktion
- Empfängt LoRa-Nachrichten von Absaugungssensoren (z. B. „Anf“).
- OLED zeigt Status (z. B. „ABS“) und sofort scrollende Textzeile (z. B. „Empfangen: Gerät 1 – Anforderung“).
- Debugmodus für erweiterte Infos (zukünftig konfigurierbar).

## Installation
1. Installiere die SSD1306Wire-Bibliothek von ThingPulse.
2. Baue das Gehäuse (siehe Absaugungssensor-Repo).
3. Lade den Sketch hoch (zukünftig verfügbar).


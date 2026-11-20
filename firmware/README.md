# Firmware RC-Multiswitch-Sound

Arduino-Sketch `RC_Multiswitch_Sound/` fuer die Platine v0.9 mit ESP32-S3-DevKitC-1 N16R8. Nicht auf Hardware getestet.

Reine Zusammenfuehrung von RC-Multiswitch-ESP32-S3 v4.06 (Schaltteil, = Multiswitch-Basis V3/TXRX) und ESP32-Sound-Basis_Ziege-One v7.21 (Soundteil). Was neu ist, steht in [CHANGELOG.md](../CHANGELOG.md).

## Arduino IDE
1. Board-Verwalter: **esp32 von Espressif, Version 3.0.7** (getestet in GitHub Actions; 3.3.12 baut ebenfalls).
2. Bibliotheken: **Bolder Flight Systems SBUS 8.1.4**, **TinyGPSPlus 1.0.3**.
3. `RC_Multiswitch_Sound/RC_Multiswitch_Sound.ino` oeffnen.
4. Werkzeuge:
   - Board: **ESP32S3 Dev Module**
   - USB CDC On Boot: **Enabled**
   - USB Mode: **USB-OTG (TinyUSB)** (neu ab v1.04, fuer das USB-Laufwerk; mit "Hardware CDC and JTAG" laeuft alles ausser dem Laufwerk)
   - Upload Mode: **USB-OTG CDC (TinyUSB)** (wenn ueber die Buchse "USB" hochgeladen wird)
   - Partition Scheme: **Minimal SPIFFS (1.9MB APP with OTA/128KB SPIFFS)**
   - PSRAM: Disabled
5. Hochladen. Danach gehen Updates auch per WLAN (Reiter WiFi, Firmware-Update) mit der Datei `RC_Multiswitch_Sound.ino.bin`.

Fertige Dateien baut GitHub Actions bei jedem Push (Artefakt `RC_Multiswitch_Sound-S3`): `*.ino.bin` = Update per WLAN, `*.merged.bin` = Komplett-Image ab Adresse 0x0 fuer USB.

## Dateien
| Datei | Inhalt |
|---|---|
| `RC_Multiswitch_Sound.ino` | Schaltteil (Ausgaenge, CRSF/SBUS, Lua-Menue, NVS "msw"), Ablauf setup/loop |
| `board_config.h` | Pins der Platine v0.9 |
| `webui.h` | Webserver und API des Schaltteils (`/api/status`, `/api/config`, `/api/output`, ...) |
| `webui_page.h` | Die Web-Seite (Schalt- und Soundteil zusammen) |
| `sound.h`, `sound.cpp` | Soundteil: Einstellungen (NVS "snd"), Wiedergabe, Motor, Einkanal, Lua-Felder |
| `sound_web.cpp` | API des Soundteils (`/api/sconfig`, `/api/sound`, `/api/debug`, SD-Dateien) |
| `usb_drive.h`, `usb_drive.cpp` | v1.04: SD-Karte als USB-Laufwerk am PC |
| `XT_I2S_Audio.*` | I2S-Audio und WAV von der SD-Karte, unveraendert aus der Sound-Basis |
| `src/RC_Common/` | Gemeinsame Basis 1.2.0, unveraenderte Kopie (CI prueft `MANIFEST.sha256`) |

## SD-Karte
FAT32, Dateien im Hauptordner: `start.wav`, `loop.wav`, `shut.wav` (Motor) und `sound1.wav` bis `sound24.wav`. Hochladen geht auch per WLAN (Reiter Motor und Sounds). Sicherungen auf der Karte liegen in `/cfgbkup`.

### SD-Karte als USB-Laufwerk (ab v1.04)
Das DevKitC mit der Buchse **USB** (nicht "UART") an den PC stecken. Nach etwa einer Sekunde erscheint die SD-Karte im Explorer als Laufwerk ("RC Sound-SD"). Sounds einfach hinkopieren, ueberschreiben oder loeschen, Dateinamen wie oben.

- Solange das Laufwerk am PC haengt, spielt das Modul **keinen Sound**, und die Web-Seite kann die Karte nicht lesen (Meldung "sd_busy"). Ausgaenge, CRSF und Lua laufen normal weiter.
- Fertig? Im Explorer **Auswerfen** (oder Kabel abziehen). Das Modul bindet die Karte neu ein und spielt wieder. Nach dem Auswerfen kommt das Laufwerk erst nach erneutem Einstecken wieder.
- Wer nur die serielle Konsole braucht und dabei Sound hoeren will: Laufwerk einfach auswerfen, der COM-Port bleibt.
- Ein USB-Netzteil ohne PC loest nichts aus.
- Der COM-Port bekommt mit "USB-OTG (TinyUSB)" eine andere Nummer als vorher. Klappt das Hochladen nicht: **BOOT** halten, **RST** kurz druecken, BOOT loslassen, dann hochladen.
- Voraussetzung: Firmware gebaut mit **USB Mode: USB-OTG (TinyUSB)** (die Dateien aus GitHub Actions sind so gebaut). Nicht auf Hardware getestet.

## Lua-Menue (CRSF)
Ab v1.11 neu durchnummeriert, 71 statt 132 Felder.

| Feld | Inhalt |
|---|---|
| 0-4 | Version, Global (Switch Addr, SBUS EK Kanal) |
| 5-15 | Ordner "Ausgaenge" (v1.11): Ausgang Nr (1-8, nur Auswahl), Name, Quelle, Nr, Blink, Blink AN/AUS, PWM Modus, PWM Festwert, Test |
| 16-19 | WLAN |
| 20-24 | UART B/C |
| 25-37 | Motor |
| 38-49 | Einstellungen: Sound-Adresse, RC-System, Sound-Einkanal (SBUS), Gruppenadressen, SBUS-Gruppen, WLAN jetzt |
| 50 | Geraetename |
| 51-58 | Ordner "Sounds": Sound Nr (1-24, nur Auswahl), Name, Quelle, Kanal Nr, Volumen, Wiedergabe Mode, Test Sound |
| 59-62 | Sound-Zuordnung (Platz, Datei, Groesse) |
| 63 | LED Farbfolge (im Ordner Global) |
| 64-71 | Funk: Betrieb, Kennung, Kanal, Leistung, Kanaele alle, Rueckkanal, Verbindung |

Seit v1.10 sind im Lua alle 24 Sounds erreichbar (vorher hoechstens 23, einstellbar in der Web-UI). Der Ordner "Sounds" ist in allen Soundmodulen gleich.

## Bekannte Grenzen
- SBUS: Ohne Frames gilt das Signal nach 10 s als weg (Wert des Schaltteils, die Sound-Basis hatte 2 s). Bis dahin laufen Sounds weiter.
- RC-System (auch Funk), UART B/C und Status-LED wirken erst nach einem Neustart.

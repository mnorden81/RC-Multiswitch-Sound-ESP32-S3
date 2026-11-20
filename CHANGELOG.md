# Versionsgeschichte RC-Multiswitch-Sound-ESP32-S3

Die Version wird bei jeder Änderung erhöht, weil sie als CRSF „Parameter Version Number“ an den Sender geht. Neueste Version oben. Noch keine Version ist auf der Platine getestet.

## v1.11 (RC_Common 1.5.2)
- Lua: statt acht Ordnern „Ausgang 1“ bis „Ausgang 8“ ein Ordner „Ausgaenge“ mit Auswahl „Ausgang Nr“ (1–8), darunter Name, Quelle, Nr, Blink, AN, AUS, PWM Modus, Festwert, Test. Gleiches Prinzip wie Ordner „Sounds“ und RC-Multiswitch v5.05. 71 statt 132 Felder, das Menü lädt schneller.
- Neue Feld-IDs: Ausgaenge 5–15, WLAN 16–19, UART B/C 20–24, Motor 25–37, Einstellungen 38–49, Gerätename 50, Sounds 51–58, Sound-Zuordnung 59–62, LED Farbfolge 63, Funk 64–71. NVS und Web-UI unverändert; der Sender lädt das Menü nach dem Update neu.

## v1.10 (RC_Common 1.5.2)
- Neu: Funk (ESP-NOW) wie RC-Multiswitch v5.04. RC-System 5 = „CRSF + Funk senden (Zentrale)“, 6 = „CRSF aus Funk (Teilnehmer)“, UART A bleibt dann frei. Web-UI: Karte „Funk“ im Reiter RC-System, Reiter „Funk“ mit Verbindungsanzeige, Hinweis auf der Übersicht. Lua: Ordner „Funk“ (Felder 125–132). Status-LED: zweite Zentrale = Konfigurationsfehler.
- Lua: statt einzelner Ordner „Sound 1“ … „Sound n“ ein Ordner „Sounds“ mit Auswahl „Sound Nr“ für alle 24 Sounds (gleich in allen Soundmodulen). Feld-IDs ab 112 neu: Sounds 112–119, Sound-Zuordnung 120–123, LED Farbfolge 124.
- Entfällt: Einstellung „Sounds im Lua-Menü“ (Karte Kombimodul, NVS `luasnd`).
- Modul-Adresse aus der Web-UI wirkt sofort (CRSF und Funk).

## v1.09
- Fix: Lua „LED Farbfolge“ hatte dieselbe Feld-ID 86 wie der Ordner „Motor“. Das Feld hat jetzt eine eigene ID hinter der Sound-Zuordnung und steht weiter im Ordner „Global“.

## v1.08 (RC_Common 1.4.0)
- Status-LED mit demselben Farbschema wie alle Module (grün langsam = OK, rot schnell = Failsafe, rot Aufblitzen = kein Signal, blau Doppelblitz = doppelte Adresse, violett = WLAN an).
- Lua Global > „LED Farbfolge“ (NVS `ledrgb`, Vorgabe GRB für den DevKitC-1).
- BOOT-Taster 2 s halten (vorher 3 s) schaltet das WLAN.

## v1.07
- Sound-Dateien behalten ihren Namen (Umlaute/Leerzeichen ersetzt). Die Zuordnung Datei → Platz steht in der Konfiguration (`sound_map.h`).
- Neuer Web-Tab „Speicher“ mit Auswahlliste je Platz, Lua-Ordner „Sound-Zuordnung“.

## v1.06
- Lua zeigt das RC-System nur noch an. Umstellen auf SBUS geht nur in der Web-UI (per Lua hätte man sich das Lua selbst abgeschaltet).

## v1.05 (RC_Common 1.3.0)
- Fix: lange Lua-Auswahlfelder kommen Chunk für Chunk, das ELRS-Lua zeigt sie vollständig.
- Ordnernamen zeigen den aktuellen Wert, unpassende Felder werden ausgeblendet (Feld-IDs bleiben).
- Versionszeile im Lua zeigt die richtige Version (vorher „v1.00“).

## v1.04
- SD-Karte als USB-Laufwerk am PC (`usb_drive.cpp`). Braucht in der Arduino IDE „USB Mode: USB-OTG (TinyUSB)“.

## v1.03
- Lua: „PWM Festwert“ bleibt bei MWprop ein gesperrtes Zahlenfeld statt Info-Feld.

## v1.02
- Lua: Sound-Kanal-Felder bleiben bei Aus/Dauer gesperrte Zahlenfelder statt Info „Deaktiviert“.

## v1.01
- Baut auch mit esp32-Core 3.3.x (`BOOT_PIN` heißt jetzt `BOOT_BTN_PIN`).

## v1.00
- Zusammenführung von RC-Multiswitch-ESP32-S3 v4.06 (Schaltteil) und ESP32-Sound-Basis_Ziege-One v7.21 (Soundteil) in einer Firmware.
- Zwei Adressen: „Switch Addr“ (Ausgänge, zugleich CRSF-Geräteadresse) und „Sound-Adresse“ (Einkanal-Sounds).
- Status-LED in der Web-UI wählbar: IO48 (DevKitC-1 v1.0, Vorgabe) oder IO38 (v1.1).
- Lua zeigt 8 Sounds, in der Web-UI auf 12/16/20/23 erhöhbar (Grenze 255 Lua-Felder). In der Web-UI immer alle 24.
- Eigener Name je Sound und je Ausgang (16 Zeichen).
- Export, Flash-Backup und SD-Sicherung enthalten beide Teile. Alte Sicherungen von Multiswitch oder Sound-Basis lassen sich einspielen.

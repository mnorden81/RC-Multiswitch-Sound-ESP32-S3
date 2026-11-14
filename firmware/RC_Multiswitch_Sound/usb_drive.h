/*
 * usb_drive.h  -  v1.04: SD-Karte als USB-Laufwerk am PC
 *
 * Steckt das Modul mit der Buchse "USB" (nativer USB des ESP32-S3) an einem
 * PC, erscheint die SD-Karte dort als Laufwerk (USB Mass Storage). Sounds
 * lassen sich dann direkt per Explorer kopieren, loeschen oder umbenennen.
 *
 *   - Solange das Laufwerk am PC haengt, spielt das Modul keinen Sound und
 *     die Web-Seite kann nicht auf die Karte zugreifen (Fehler "sd_busy").
 *     Schalten (Ausgaenge, CRSF, Lua) laeuft normal weiter.
 *   - Am PC "Auswerfen" (oder Kabel abziehen) gibt die Karte zurueck: das
 *     Modul bindet sie neu ein und spielt wieder. Nach dem Auswerfen
 *     erscheint das Laufwerk erst nach erneutem Einstecken wieder.
 *   - Ein USB-Netzteil ohne PC loest nichts aus (es meldet sich kein Host).
 *
 * Voraussetzung (Arduino IDE, Werkzeuge): USB Mode = "USB-OTG (TinyUSB)".
 * Mit "Hardware CDC and JTAG" baut die Firmware weiter, nur ohne Laufwerk.
 * Die serielle Konsole (Serial) bleibt in beiden Faellen am selben USB.
 */
#pragma once
#include <Arduino.h>
#include "soc/soc_caps.h"

#if defined(SOC_USB_OTG_SUPPORTED) && SOC_USB_OTG_SUPPORTED && \
    defined(ARDUINO_USB_MODE) && (ARDUINO_USB_MODE == 0) && \
    defined(CONFIG_TINYUSB_MSC_ENABLED) && CONFIG_TINYUSB_MSC_ENABLED
  #define USB_DRIVE_AVAILABLE 1
#else
  #define USB_DRIVE_AVAILABLE 0
#endif

void usbDriveBegin();      // in setup() nach soundSetupLate()
void usbDriveLoop();       // in loop(), vor soundLoop()
bool usbDriveActive();     // true = Karte gehoert gerade dem PC

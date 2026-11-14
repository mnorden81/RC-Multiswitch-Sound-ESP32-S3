/*
 * usb_drive.cpp  -  v1.04: SD-Karte als USB-Laufwerk, siehe usb_drive.h
 *
 * Ablauf (alles aus loop(), die Lese-/Schreib-Callbacks laufen im
 * TinyUSB-Task):
 *   AUS       PC hat das Geraet 1 s lang eingebunden (tud_mounted) und eine
 *             Karte steckt -> soundStorageRelease(), Laufwerk melden: AKTIV
 *   AKTIV     PC wirft aus oder USB weg -> Laufwerk zurueckziehen, kurz auf
 *             einen laufenden Sektorzugriff warten, soundStorageReload():
 *             AUSGEWORFEN (USB noch da) bzw. AUS (USB weg)
 *   AUSGEWORFEN  wartet, bis USB weg ist, dann AUS
 */
#include "usb_drive.h"
#include "sound.h"

#if USB_DRIVE_AVAILABLE

#include "USB.h"
#include "USBMSC.h"
#include "SD.h"

extern "C" bool tud_mounted(void);   // TinyUSB: Host hat das Geraet konfiguriert

// Global, damit sich die MSC-Schnittstelle schon vor dem USB-Start (vor
// setup(), "USB CDC On Boot") im Geraete-Deskriptor anmeldet.
static USBMSC msc;

static constexpr uint32_t HOST_STABLE_MS = 1000;

enum : uint8_t { DRV_OFF, DRV_ACTIVE, DRV_EJECTED };
static uint8_t           s_state      = DRV_OFF;
static volatile bool     s_present    = false;   // Medium fuer den PC sichtbar
static volatile bool     s_inCallback = false;   // Sektorzugriff laeuft
static volatile bool     s_ejectReq   = false;   // PC hat "Auswerfen" geschickt
static uint16_t          s_secSize    = 512;
static unsigned long     s_hostSince  = 0;
static bool              s_noCardLog  = false;

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
    if (!s_present) return -1;
    s_inCallback = true;
    uint8_t* p = (uint8_t*)buffer;
    uint32_t sec = lba + offset / s_secSize;
    uint32_t n = bufsize / s_secSize;
    for (uint32_t i = 0; i < n; i++) {
        if (!SD.readRAW(p + i * s_secSize, sec + i)) { s_inCallback = false; return -1; }
    }
    s_inCallback = false;
    return (int32_t)(n * s_secSize);
}

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
    if (!s_present) return -1;
    s_inCallback = true;
    uint32_t sec = lba + offset / s_secSize;
    uint32_t n = bufsize / s_secSize;
    for (uint32_t i = 0; i < n; i++) {
        if (!SD.writeRAW(buffer + i * s_secSize, sec + i)) { s_inCallback = false; return -1; }
    }
    s_inCallback = false;
    return (int32_t)(n * s_secSize);
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
    (void)power_condition;
    if (load_eject && !start) s_ejectReq = true;   // "Auswerfen" am PC
    return true;
}

static void driveEnter() {
    uint32_t sectors = (uint32_t)SD.numSectors();
    uint16_t secSize = (uint16_t)SD.sectorSize();
    if (sectors == 0 || secSize == 0) {
        if (!s_noCardLog) Serial.println("USB-Laufwerk: keine SD-Karte - Laufwerk bleibt leer");
        s_noCardLog = true;
        return;
    }
    soundStorageRelease();
    s_secSize  = secSize;
    s_ejectReq = false;
    s_present  = true;
    msc.begin(sectors, secSize);
    msc.mediaPresent(true);
    s_state   = DRV_ACTIVE;
    Serial.printf("USB-Laufwerk: SD-Karte am PC (%lu MB) - Sound pausiert bis zum Auswerfen\n",
                  (unsigned long)((uint64_t)sectors * secSize / (1024UL * 1024UL)));
}

static void driveLeave(bool hostStillThere) {
    s_present = false;
    msc.mediaPresent(false);
    unsigned long t0 = millis();
    while (s_inCallback && millis() - t0 < 500) delay(1);
    soundStorageReload();
    s_state = hostStillThere ? DRV_EJECTED : DRV_OFF;
    Serial.println(hostStillThere ? "USB-Laufwerk: ausgeworfen - SD-Karte wieder beim Modul"
                                  : "USB-Laufwerk: USB getrennt - SD-Karte wieder beim Modul");
}

void usbDriveBegin() {
    msc.vendorID("RC");             // max. 8 Zeichen
    msc.productID("Sound-SD");      // max. 16 Zeichen
    msc.productRevision("1.04");    // max. 4 Zeichen
    msc.onRead(onRead);
    msc.onWrite(onWrite);
    msc.onStartStop(onStartStop);
    msc.mediaPresent(false);        // erst wenn der PC wirklich da ist
    USB.begin();                    // laeuft meist schon (USB CDC On Boot)
    Serial.println("USB-Laufwerk: bereit (Buchse USB an einen PC stecken)");
}

void usbDriveLoop() {
    bool host = tud_mounted();
    if (!host) { s_hostSince = 0; s_noCardLog = false; }
    else if (s_hostSince == 0) s_hostSince = millis() | 1;

    switch (s_state) {
    case DRV_OFF:
        if (host && millis() - s_hostSince >= HOST_STABLE_MS && !s_noCardLog) driveEnter();
        break;
    case DRV_ACTIVE:
        if (!host || s_ejectReq) driveLeave(host);
        break;
    case DRV_EJECTED:
        if (!host) s_state = DRV_OFF;
        break;
    }
}

bool usbDriveActive() { return s_state == DRV_ACTIVE; }

#else   // ohne USB-OTG (TinyUSB): kein Laufwerk, alles andere unveraendert

void usbDriveBegin() {
    Serial.println("USB-Laufwerk: aus (Werkzeuge > USB Mode: \"USB-OTG (TinyUSB)\" waehlen)");
}
void usbDriveLoop() {}
bool usbDriveActive() { return false; }

#endif

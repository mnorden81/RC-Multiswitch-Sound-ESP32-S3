/*
 * diag.cpp  -  DIAGNOSE-Firmware (nur Zweig zur Fehlersuche "kein Ton")
 *
 * - Beim Start: Inhalt der SD-Karte, Lautstaerken und Dateizuordnung ins Log,
 *   danach 2 s Piepton (1 kHz) direkt in den I2S-Puffer, ohne SD-Karte.
 * - Alle 2 s eine Statuszeile: I2S-Bloecke/s, Fehler, hoechster Pegel.
 * - Befehle im Seriellen Monitor (Zeichen senden):
 *     t = Piepton 2 s     1 = Sound 1 testen     m = Motor an/aus
 */
#include <Arduino.h>
#include "SD.h"
#include "board_config.h"
#include "XT_I2S_Audio.h"
#include "sound.h"

extern volatile uint32_t g_diagToneLeft, g_diagI2sBlocks, g_diagI2sErrors;
extern volatile int32_t  g_diagPeak;
void diagBeep(uint32_t ms);
extern bool Sound_on_web[SND_NUM_SLOTS];
extern bool Sound_play[SND_NUM_SLOTS];

static void diagListSd() {
    if (!sdMutexTake(2000)) { Serial.println("[DIAG] SD belegt"); return; }
    File root = SD.open("/");
    if (!root) { Serial.println("[DIAG] SD: Wurzel nicht lesbar!"); sdMutexGive(); return; }
    Serial.println("[DIAG] Dateien auf der SD-Karte:");
    int n = 0;
    for (File f = root.openNextFile(); f && n < 40; f = root.openNextFile(), n++) {
        Serial.printf("[DIAG]   %s  %lu Bytes\n", f.name(), (unsigned long)f.size());
        f.close();
    }
    root.close();
    sdMutexGive();
    if (n == 0) Serial.println("[DIAG]   (keine Dateien)");
}

void diagSetup() {
    Serial.println();
    Serial.println("[DIAG] ===== DIAGNOSE-FIRMWARE (kein Ton) =====");
    Serial.printf("[DIAG] I2S: BCLK=%d LRCK=%d DOUT=%d, %d Hz\n", I2S_BCLK, I2S_LRC, I2S_DOUT, SAMPLES_PER_SEC);
    diagListSd();
    Serial.printf("[DIAG] Motor: Quelle=%d Lautstaerke=%d  Dateien: %s / %s / %s\n",
                  scfg.Source_Start_Sound[0], scfg.Volumen_Sound[0],
                  scfg.Sound_File[0], scfg.Sound_File[1], scfg.Sound_File[2]);
    for (int i = 1; i <= 8; i++)
        Serial.printf("[DIAG] Sound %d: Quelle=%d Lautstaerke=%d Modus=%d Datei=%s\n", i,
                      scfg.Source_Start_Sound[i], scfg.Volumen_Sound[i], scfg.Mode_Sound[i],
                      scfg.Sound_File[2 + i]);
    Serial.println("[DIAG] Piepton 2 s ...  (Befehle: t=Piepton, 1=Sound 1, m=Motor an/aus)");
    diagBeep(2000);
}

void diagLoop() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == 't') { Serial.println("[DIAG] Piepton 2 s"); diagBeep(2000); }
        else if (c == '1') { Serial.println("[DIAG] Test Sound 1"); Sound_on_web[1] = true; }
        else if (c == 'm') { Sound_on_web[0] = !Sound_on_web[0];
                             Serial.printf("[DIAG] Motor-Test %s\n", Sound_on_web[0] ? "AN" : "AUS"); }
    }
    static uint32_t last = 0, lastBlocks = 0;
    uint32_t now = millis();
    if (now - last >= 2000) {
        uint32_t b = g_diagI2sBlocks;
        Serial.printf("[DIAG] t=%lus I2S-Bloecke/s=%lu Fehler=%lu Pegel(max)=%ld Ton=%s Motor=%d S1=%d\n",
                      (unsigned long)(now / 1000), (unsigned long)((b - lastBlocks) / 2),
                      (unsigned long)g_diagI2sErrors, (long)g_diagPeak,
                      g_diagToneLeft ? "an" : "aus", Sound_play[0], Sound_play[1]);
        g_diagPeak = 0; lastBlocks = b; last = now;
    }
}

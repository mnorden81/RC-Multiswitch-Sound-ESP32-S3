# Gehaeuse fuer RC-Multiswitch-Sound-ESP32-S3 (Platine v0.9)

3D-druckbares Gehaeuse aus Unterteil und Deckel. Die Masse stammen aus `hardware/RC-Multiswitch-Sound-ESP32-S3.kicad_pcb` (Platine 106 x 68,15 mm), die Bauteilhoehen sind Annahmen (siehe unten).

![Gehaeuse offen](gehaeuse_offen.png)

## Dateien
| Datei | Inhalt |
|---|---|
| `Gehaeuse_Unterteil.stl` | Unterteil, druckfertig (Boden aufs Bett) |
| `Gehaeuse_Deckel.stl` | Deckel, druckfertig (schon gedreht, Oberseite aufs Bett) |
| `Gehaeuse_Unterteil.step`, `Gehaeuse_Deckel.step` | zum Weiterbearbeiten in Fusion 360, FreeCAD oder KiCad (Deckel in Einbaulage) |
| `gehaeuse.py` | Quelle (CadQuery), alle Masse als Parameter oben in der Datei |
| `Gehaeuse3D_Vorschau.html` | drehbare 3D-Vorschau mit Platine (im Browser oeffnen) |

## Masse
- Aussen 117,2 x 78,2 x 34,6 mm (mit Deckel), Wand 2,2 mm, Boden und Deckel 2,0 mm.
- 5 mm Platz unter der Platine (Loetstellen), 24 mm ueber der Platine.

## Annahmen (am echten Aufbau pruefen)
- Elko C1 (470 uF/35 V) ist 22 mm hoch und steht, er ist das hoechste Teil.
- Unterkante des ESP32-S3-DevKitC-1 liegt 13 mm ueber der Platine (Buchsenleisten). Danach richtet sich der USB-Ausschnitt.
- Lage von BOOT, RESET und RGB-LED auf dem DevKitC-1 ist nicht in der KiCad-Datei, deshalb ein grosses Fenster im Deckel am USB-Ende.

## Oeffnungen
- Links: USB-C des ESP32 (beide Buchsen), microSD-Karte mit Griffmulde, CN1 UART A, CN2 Akku 3S.
- Vorne: J5 UART B, J6 UART C, CN4 Lautsprecher, Kabelschlitz fuer J1.
- Deckel: Ausschnitt ueber J1 (Ausgaenge 2x8), Fenster BOOT/RESET/RGB-LED, 8 Lichtloecher fuer die Kontroll-LEDs, Lueftungsschlitze ueber dem TPA3118, vertiefte Beschriftung.
- Rechts: 6 Lueftungsschlitze.

## Drucken und Montieren
- PETG oder ASA (der TPA3118 wird warm), 0,2 mm Schicht, 3 Wandlinien, 20 % Fuellung, keine Stuetzen noetig.
- Platine: Sie hat keine eigenen Montageloecher. 4 x M3-Schraube von oben durch die Befestigungsloecher des TPA3118-Moduls und die Platine in die Saeulen (Kernloch 2,5 mm, selbstschneidend, ca. M3 x 16-20 je nach Modulabstand). 6 Stuetzen am Rand tragen die Platine zusaetzlich.
- Deckel: 4 x Blechschraube 2,5 x 8 in die Eckdome.
- Einbau ins Modell: 4 Laschen am Boden mit 3,4-mm-Loch fuer Schrauben oder Kabelbinder.
- Die Kontroll-LEDs sitzen ca. 19 mm unter dem Deckel; wer sie direkt im Deckel haben will, loetet sie mit langen Beinen ein.

## Version mit gewinkelter Stiftleiste (J1 nach vorne)
Fuer eine gewinkelte Stiftleiste 2x8, 2,54 mm an J1 statt der stehenden. Die Stecker der 8 Ausgaenge kommen vorne durch die Wand, der Deckel ist ueber J1 geschlossen. Der hohe Kabelschlitz fuer J1 entfaellt.

![Gehaeuse gewinkelt, Vorderseite](gehaeuse_gewinkelt_vorne.png)

![J1 mit Dupont-Steckern](gehaeuse_gewinkelt_j1.png)

| Datei | Inhalt |
|---|---|
| `Gehaeuse_Unterteil_gewinkelt.stl` / `.step` | Unterteil mit Schlitz 21,5 x 8,6 mm in der vorderen Wand |
| `Gehaeuse_Deckel_gewinkelt.stl` / `.step` | Deckel ohne J1-Ausschnitt |
| `Gehaeuse3D_Vorschau_gewinkelt.html` | drehbare 3D-Vorschau mit bestueckter Platine, Stiftleiste und Dupont-Steckern, dazu ein massstaeblicher Schnitt durch einen Ausgang (im Browser oeffnen) |

### Stiftleiste
- Teil: Stiftleiste 2x8, Raster 2,54 mm, gewinkelt (90 Grad), Kunststoff 2,5 mm, Stifte ca. 6 mm lang.
- Die Leiste wird in die vorhandenen Loecher von J1 geloetet, der Kunststoffkoerper zeigt zur vorderen Platinenkante. Die Platine bleibt unveraendert.
- Der Koerper steht ca. 0,6 mm ueber die Platinenkante. Leiste flach auf die Platine druecken, erst einen Stift loeten, dann ausrichten.
- Belegung: oben liegen die Ausgaenge 1-8 (ungerade Pins), unten VLOAD (gerade Pins).
- Masse nach KiCad-Footprint `PinHeader_2x08_P2.54mm_Horizontal`: Koerper 5,1 mm hoch, untere Stiftreihe 1,27 mm und obere 3,81 mm ueber der Platine.

### Schlitz in der Vorderwand
- 21,5 x 8,6 mm: von 1 mm unter der Platinen-Unterseite bis 6 mm ueber der Platinen-Oberseite (Parameter `J1_SCHLITZ_U` und `J1_SCHLITZ_H` in `gehaeuse.py`).
- Die Stecker sitzen vor der Platinenkante und reichen nach Footprint genau bis zur Platinen-Oberseite. Unten bleiben 2,6 mm Luft, falls die Leiste etwas tiefer sitzt (Stand bis 06.11.2026 waren es nur 0,2 mm, das war zu knapp).
- Die Vorderwand ist dort 4,4 mm dick (Aussenkontur um die Eckdome). Die Stifte ragen ca. 1,6 mm aus der Wand, die Stecker werden 4,4 mm tief in den Schlitz geschoben.
- Passt fuer 8 Dupont-Stecker 1x2 nebeneinander (je 2,54 x 5,08 mm) oder einen Stecker 2x8.

![Schnitt durch einen Ausgang, alt und aktuell](gehaeuse_gewinkelt_schnitt.png)

- Aussenmasse wie bei der stehenden Version.
- Erzeugen: `python gehaeuse.py export --gewinkelt`

## Aendern
Masse in `gehaeuse.py` anpassen und neu erzeugen (Python 3 mit `pip install cadquery`):
```
python gehaeuse.py export
```
Die Dateien landen im Ordner `export`. Mit `--vorschau` wird zusaetzlich die Platine als Modell erzeugt und auf Ueberschneidungen geprueft.

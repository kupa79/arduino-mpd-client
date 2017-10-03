# Arduino MPD Hardware Client

## Beschreibung:
* Stand-alone Hardware-Client zur Steuerung des [Music Player Daemon](https://www.musicpd.org/).
* Bedienung mit (beliebiger) IR-Fernbedienung.
* Abspiel-Funktionen: Play, Pause, Stop, Next, Prevoius.
* Info-Funktionen: Anzeige Interpret, Titel, Zeit.
* Kommunikation mit MPD übers Netzwerk.

## Erste Schritte
* Client fragt nach MPD-Server-IP
* Benutzer wählt über Fernbedienung (hoch/runter) Ziffern 0-9 oder “Punkt” aus, Ziffern-Bestätigung mit Enter. Vollständigkit der IP-Eingabe mit weiterem Enter bestätigen.
* Client fragt nach MPD-Server-Port
* Benutzer wählt über Fernbedienung (hoch/runter) Ziffern 0-9 aus, Ziffern-Bestätigung mit Enter. Vollständigkit der Port-Eingabe mit weiterem Enter bestätigen.
* Client fragt nach MPD-Server-Passwort
* Benutzer wählt über Fernbedienung (hoch/runter) Ziffern 0-9 und/oder Buchstaben (A-Z, a-z) aus, Ziffern-Bestätigung mit Enter. Vollständigkit der Passwort-Eingabe mit weiterem Enter bestätigen.
* Client speichert IP, Port und Passwort in einem nicht-flüchtigen Speicher.
* Client verbindet sich mit dem MPD.
* Client zeigt den aktuellen MPD-Status an.

## Stückliste Hardware
* Arduino UNO Rev3
* Arduino Ethernet Shield
* LCD EA DOGM 163
* Hintergrundbeleuchtung für LCD + Vorwiderstand + Transistor
* IR Receiver TSOP 4838
* 2 LEDs + Vorwiderstand

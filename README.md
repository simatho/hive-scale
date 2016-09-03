# Bienenstockwaage
Eine *Internet of Things* - Bienenstockwaage.

### Features:
- Zyklisches Messen unterschiedlicher Größen
  - Spannung an der angeschlossenen Messbrücke => Gewicht auf der Waage
  - Temperatur des A/D-Wandlers
  - Temperatur des angeschlossenen 1-Wire-Temperatursensors
- Versenden der Daten per HTTP-GET-Request an eine Datenbank
- Grafische Aufbereitung der Daten im Backend ([Beispiel](http://waage.retsifp.de), auf Grund der laufenden Entwicklung nicht mit echten Daten)
- Open Source Soft- und Hardware!

### Hardware:
- Hauptprozessor, WLAN: Espressif **ESP8266**
- A/D-Wandler: Intersil **ISL26102**
- Tiefsetzer (3V3, 5V0): Linear Technology **LTC3622-23/5**
- Wägezelle: Bosche **H40A**
- Temperatursensor: Maxim **DS18B20**

### Software:
- ESP8266-Firmware: C++ (Greift mit einigen Funktionen auf die [ESP8266-Arduino](https://github.com/esp8266/Arduino/)-Portierung zurück und wird mit dieser Kompiliert)
- Backend: PHP7 (sollte auch mit niedrigeren Versionen funktionieren)
- Darstellung: [nvd3](http://nvd3.org/)
- Datenbank: MySQL

### Hintergrund
Dieses Projekt ist im Rahmen der Vorlesung _Systems Engineering I_ an der [Hochschule für angewandte Wissenschaften Augsburg](https://hs-augsburg.de) entstanden.
In dieser Vorlesung sucht sich eine Gruppe von Studenten ein Projekt, das sie ein Semester lang zu entwickeln versuchen.
Unser Ansatz zielt explizit darauf ab, nachgebaut und weiterentwickelt zu werden.
Da einige Software-Features nicht rechtzeitig fertiggestellt werden konnten, werden diese sukzessive nachgereicht.

### Versionsgeschichte:
#### Für die nächsten Versionen angedachte Features:
- Verwendung der [SPIFFS](https://github.com/pellepl/spiffs)-Bibliothek zum Abspeichern der Konfiguration sowie der Messdaten
- Daten nur alle paar Messzyklen übertragen (und im SPIFFS zwischenspeichern)
- Server kann dem Client eine geänderte Zykluszeit vorgeben

#### Version 0.9.7.2 (03.09.2016)
- Kleinere Verbesserungen am Webinterface
- firmware war nicht kompilierbar, korrigiert

#### Version 0.9.7 (01.09.2016)
- Webinterface: Geräteverwaltung integriert, per Login gesichert
- Anzeigen von Daten wie: Letztes Sendedatum, Firmwareversion
- Erstellen von Geräteklassen
- Einstellen von: Firmwareupdate, Geräteklasse
- Upload von neuen Firmware-Versionen

#### Version 0.9.6 (26.06.2016)
- Erste öffentliche Version
- _firmware/main.cpp_: Instruktionen hinzugefügt, wie die Einstellungs-Datei _settings.h_ beschaffen sein muss. Außerdem Option hinzugefügt, das folgende Feature (z.B. aus Speicherplatzgründen) wieder deaktivieren zu können.
- Alle nötigen Funktionen hinzugefügt, die Firmware über das Internet zu aktualisieren:
- _backend/ota_update.php_: Liefert die Firmware-Datei auf dem Server aus (momentan noch keine Kontrolle, ob Firmware überhaupt aktualisiert werden muss).
- Neue Firmware-Klasse _Updater_: Prüft auf neues Update (ist an dieser Stelle bereits darauf vorbereitet, vom Server die Antwort "nicht aktualisieren" zu bekommen).
- _firmware/main.cpp_: Neue Befehle _autoUpdate_ (Prüft auf neues Update und aktualisiert falls der Server dazu anweist) und _update_ (erzwingt ein Update) hinzugefügt.
- Mehrere kleinere Änderungen und Verbesserungen, Refactoring

#### Version 0.9 (21.06.2016, Präsentationsstand HSA)
- An der HSA präsentierte und so auch zur Bewertung abgegebene Version

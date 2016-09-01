// Einstellungs-Datei einbinden
#include "settings.h"
#ifndef __CONFIGURED__
#error ERROR: NOT CONFIGURED! Read main.cpp for details.
// Bitte eine Datei "settings.h" anlegen, folgendes einfügen und entsprechend anpassen:
#define __CONFIGURED__
// WLAN-Daten
#define cfg_wifiSSID "ESP_Wifi"
#define cfg_wifiPass "samplePass"

// Um Debug-Ausgaben zu aktivieren, folgende Zeile einkommentieren!
//#define cfg_debugEnabled

// Empfänger Host-Adresse (An diesen Host werden die Messdaten gesendet)
#define cfg_receiverAddress "http://data.example.org/data_receive.php"

// Host für "Over the Air" (OTA)-Updates.
#define cfg_otaHost "ota.example.org"
// SHA1-Fingerprint des SSL-Zertifikats
#define cfg_sha1Fingerprint "3A D2 DA C2 23 CB 31 D1 8C B7 F1 6B 56 F3 C4 12 7B 0D 2E E2"

// Falls kein OTA-Server bereit steht kann OTA auch komplett abgeschaltet werden, dafür die folgende Zeile einkommentieren
// Dies kann auch notwendig sein, wenn auf dem Flash des ESP8266 zu wenig Speicher verfügbar ist
//#define disableOTA

// Ende "settings.h"
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include "FS.h"
#include "SOC.h"
#include "Webserver.h"
#include "ISL2610x.h"
#include "Mass_Sensor.h"
#include <OneWire.h>
#include "DS18B20.h"
#include "Updater.h"

ESP8266WiFiMulti WiFiMulti;
SOC stateOfCharge;
ISL2610x * v_r_bridgeADC;
OneWire * v_p_oneWire;
DS18B20 v_ds_extTemp;
Webserver server;
MassSensor * massSensor = new MassSensor(16043.95, 178e-6);
#ifndef disableOTA
Updater updater;
#endif
unsigned long sendNext = 0;

void setup() {
	Serial.begin(115200);
	delay(10);
	WiFiMulti.addAP(cfg_wifiSSID, cfg_wifiPass);

	Serial.print("\nBienenstockwaage v" + Updater::getVersion() + "\nReset: " + ESP.getResetReason() + "\nReset Info:" + ESP.getResetInfo() + "\nVerbindungsaufbau...");

	while(WiFiMulti.run() != WL_CONNECTED) {
		Serial.print(".");
		delay(100);
	}

	Serial.println("\nWiFi verbunden\nIP-Adresse:");
	Serial.println(WiFi.localIP());
	delay(500);
	server.init();
	v_r_bridgeADC = new ISL2610x(ISL26102, 5, 15, SPI)
	v_r_bridgeADC->init();
	struct s_ChConfig ch1Config = {0x00, 0x7};
	v_r_bridgeADC->setChConfig(CH1, ch1Config);
	v_p_oneWire = new OneWire(4);
	v_ds_extTemp.setOneWire(v_p_oneWire);
	v_ds_extTemp.searchAddress();
	v_ds_extTemp.setRes(9);
	#ifdef cfg_noMeasurements
	sendNext = -1; // Nicht automatisch senden
	#endif
}

void loop() {
	// ESP-Interner Webserver: Anfragen bearbeiten
	server.handleClient();
	if(Serial.available() > 0) {
		String input = Serial.readString();
		if(input == "setOffset") {
			Serial.println(F("Waage entlasten, weiter mit Eingabe eines beliebigen Zeichens"));
			while(!Serial.available() );
			Serial.read();
			Serial.println("Offset wird ermittelt...");
			float result_V;
			v_r_bridgeADC->readVoltage(CH1, result_V);
			massSensor.setOffset(result_V);
			Serial.println("Offset gesetzt");
		} else if(input == "calibGain") {
			Serial.println("Bekanntes Gewicht auf Waage stellen");
			float result_V;
			Serial.print("Eingabe Bekanntes Gewicht [kg]: ");
			while(!Serial.available() );
			float knownMass_kg = Serial.parseFloat();
			Serial.println(knownMass_kg, 3);
			Serial.println("Gain wird ermittelt...");
			v_r_bridgeADC->readVoltage(CH1, result_V);
			massSensor.calibGain(result_V, knownMass_kg);
			Serial.println("Gain gesetzt");
		} else if(input == "reset") {
			ESP.restart();
		#ifndef disableOTA
		} else if(input == "forceUpdate") {
			Serial.println("\nHartes Update gestartet...");
			updater.update("ota_update.php?force=1");
		} else if(input == "update") {
			Serial.println("\nAutomatisches Update gestartet...");
			updater.update();
		#endif
		} else if(input == "stopM") {
			// Keine Messungen mehr durchführen
			sendNext = -1; // Größte mögliche Zahl ~ etwa 50 Tage
			Serial.println("Keine automatischen Messungen mehr");
		} else if(input == "startM") {
			// Messungen wieder einschalten
			sendNext = 0;
			Serial.println("Automatische Messungen reaktiviert");
		} else if(input == "hallo"){
			Serial.println("Messung gestartet...");
			Serial.println(server.getHelloString());
		} else if(input == "sleep"){
			Serial.println("Gehe 2s schlafen...");
			ESP.deepSleep(2000000, WAKE_RF_DEFAULT);
		} else {
			Serial.println(F("Unbekannter Befehl.\nOptionen:\n\thallo\tEinzelmessung\n\tstopM\tMessung beenden\n\tstartM\tMessung starten\n\tupdate\tUpdate starten"));
		}
	}
	if(sendNext < millis()) {
		// Daten zum Server schicken

		// Daten sammeln
		float result_V; // Brückenspannung
		v_r_bridgeADC->calibCh(CH1);
		v_r_bridgeADC->readVoltage(CH1, result_V);
		float result_dgC; // A/D-Temperatur
		v_r_bridgeADC->readTemp(result_dgC);
		float temp_ext;
		v_ds_extTemp.readTemp(temp_ext);
		uint8_t * ext_sensorid = nullptr;
		v_ds_extTemp.getAddr(ext_sensorid);

		// Request-URL zusammenbauen
		String url = cfg_receiverAddress; //Request URL
		url += "?weight=";
		url += String (massSensor.calcMass(result_V), 4); //Berechnete Masse
		url += "&chipmillis=";
		url += millis(); // Millisekunden seit CPU-Start. Momentan nur zu Debug-Zwecken
		url += "&chipid=";
		url += ESP.getChipId(); // Chip-Seriennummer. Momentan nur zu Debug-Zwecken (Und um "echte" Daten von Testdaten unterscheiden zu können)
		url += "&temp_int=";
		url += String (result_dgC, 3);
		url += "&vcc=";
		url += (String) stateOfCharge.getVoltage(); // Versorgungsspannung
		url += "&temp_ext=";
		url += String (temp_ext, 3); // Temperatur am externen Sensor
		url += "&sid_ext=";
		url += String (ext_sensorid[7], HEX); //Seriennummer des Temperatursensors
		url += "&ver=";
		url += Updater::getVersion();
		url += "&mac=";
		url += WiFi.macAddress();
		url += "&reset=";
		url += ESP.getResetReason();
		time_t now = time(nullptr);
		if(now > 0) { // Wenn die Zeit via NTP bereits ermittelt werden konnte
			url += "&sensetime=";
			url += (String) now;
		}

		// Daten abschicken
		HTTPClient http;
		#ifdef cfg_debugEnabled
		Serial.print("URL: ");
		Serial.println(url);
		#endif
		http.begin(url);
		int httpCode = http.GET();

		// Negativer Code: Fehler
		if(httpCode > 0) {
			// Verbindung zum Host steht soweit.
			Serial.printf("[HTTP] GET... code: %d\n", httpCode);

			// Request erfolgreich
			if(httpCode == HTTP_CODE_OK) {
				// Antwort vom Server Seriell ausgeben
				// TODO: Herausfinden, ob nötig oder ob die Antwort einfach ignoriert werden könnnte.
				String payload = http.getString();
				if(payload == "Erfolg"){
					Serial.println("Erfolg.");
				} else if (payload == "Update") {
					#ifndef disableOTA
					Serial.println("Update.");
					updater.update();
					#else
					Serial.println(F("Vom Server zum Updaten angewiesen worden, OTA-Updatesupport nicht integriert!"));
					#endif

				}
			}
		} else {
			// Misserfolg - Fehler ausgeben
			Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();
		sendNext = millis() + 10000; // In 10 Sekunden wieder senden
	}
}

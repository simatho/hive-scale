//! \file Webserver.cpp
//! \brief Diese Klasse aktiviert auf Port 80 einen Webserver, über den der
//! ESP8266 einige Informationen ausgibt und Konfigurationen
//! vorgenommen werden können.
//!
//! \author Thomas Pfister
//! \version 1.0
//!
// -------------------------------------------------------------------------------------------------
// Einbinden von Headern
// -------------------------------------------------------------------------------------------------
#include "Webserver.h"
// -------------------------------------------------------------------------------------------------
// Definition entsprechend der Reihenfolge im Header
// -------------------------------------------------------------------------------------------------
Webserver::Webserver() : ESP8266WebServer() {
	// Konfiguriere SNTP. Zeitzone: UTC, da NTP keine Sommerzeit (DST) vorsieht
	configTime(0, 0, "de.pool.ntp.org");
}

void Webserver::init() {
	this->on("/", std::bind(&Webserver::handleRoot, this));
	this->onNotFound(std::bind(&Webserver::handleNotFound, this));
	this->begin();
}

void Webserver::handleRoot() {
	this->send(200, "text/plain", this->getHelloString());
}

void Webserver::handleNotFound(){
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += this->uri();
	message += "\nMethod: ";
	message += (this->method() == HTTP_GET)?"GET":"POST";
	message += "\nArguments: ";
	message += this->args();
	message += "\n";
	for (uint8_t i=0; i<this->args(); i++){
		message += " " + this->argName(i) + ": " + this->arg(i) + "\n";
	}
	this->send(404, "text/plain", message);
}

String Webserver::getHelloString(){
	String hello = F("Hallo Welt, hier ist die Bienenstockwaage!\nMeine Versorgungsspannung: ");
	hello += (String) stateOfCharge.getVoltage();
	hello += " V";
	time_t now = time(nullptr);
	if(now > 0) {
		// Falls Zeit bereits ermittelt.
		hello += "\nDie Uhrzeit (UTC): ";
		hello += ctime(&now);
	}
	hello += "\nBrueckenspannung: ";
	float result_V; // Brückenspannung
	v_r_bridgeADC->calibCh(CH1);
	v_r_bridgeADC->readVoltage(CH1, result_V);
	hello += String (result_V * 1e3, 4);
	hello += " mV\nDaraus berechnetes Gewicht: ";
	hello += String (massSensor->calcMass(result_V), 4);
	float result_dgC; // A/D-Temperatur
	v_r_bridgeADC->readTemp(result_dgC);
	hello += " kg\nTemperatur des A/D-Wandlers: ";
	hello += String (result_dgC, 3);
	hello += " Grad Celsius\nTemperatur des externen Sensors: ";
	float temp_ext;
	v_ds_extTemp->readTemp(temp_ext);
	hello += String (temp_ext, 3);
	hello += " Grad Celsius\nVersion: ";
	//hello += Updater::getVersion();
	hello += F(" (Build Date: "  __DATE__  ", " __TIME__ ")\nMAC: ");
	hello += WiFi.macAddress();
	return hello;
}

//! \file Webserver.h
//! \brief Diese Klasse aktiviert auf Port 80 einen Webserver, über den der
//! ESP8266 einige Informationen ausgibt und Konfigurationen
//! vorgenommen werden können.
//!
//! \author Thomas Pfister
//! \version 1.0
//!
// -------------------------------------------------------------------------------------------------
#ifndef __WEBSERVER__
#define __WEBSERVER__
// -------------------------------------------------------------------------------------------------
// Einbinden von Headern
// -------------------------------------------------------------------------------------------------
#include <stdint.h>
#include <Esp.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>
#include "SOC.h"
#include "ISL2610x.h"
#include "Mass_Sensor.h"
#include "DS18B20.h"
#include "Updater.h"

// -------------------------------------------------------------------------------------------------
// Externe Objekte
// -------------------------------------------------------------------------------------------------
extern SOC stateOfCharge;
extern ISL2610x * v_r_bridgeADC;
extern MassSensor * massSensor;
extern DS18B20 * v_ds_extTemp;
// -------------------------------------------------------------------------------------------------
// Definition von Konstanten
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// Klassendeklaration
// -------------------------------------------------------------------------------------------------
class Webserver: public ESP8266WebServer{
private:
	// -----------------------------------------------------------------------------------------------
	// Private Attribute
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Verbotene Konstruktoren
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Verbotene Operatoren
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Private Methoden
	// -----------------------------------------------------------------------------------------------
	void handleRoot();
	void handleNotFound();
protected:
	// -----------------------------------------------------------------------------------------------
	// Private zu vererbende Attribute
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Private zu vererbende Methoden
	// -----------------------------------------------------------------------------------------------

public:
	// -----------------------------------------------------------------------------------------------
	// Konstruktoren
	// -----------------------------------------------------------------------------------------------
	Webserver();
	// -----------------------------------------------------------------------------------------------
	// Operatoren
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Öffentliche Attribute
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Setter
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Getter
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Öffentliche Methoden
	// -----------------------------------------------------------------------------------------------
	void init();
	String getHelloString();
};

#endif __WEBSERVER__

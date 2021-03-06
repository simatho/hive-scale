//! \file SOC.h
//! \brief Mithilfe dieser Klasse lässt sich die Spannung an den Batterie-
//!	anschlüssen messen
//!
//! \author Thomas Pfister
//! \version 1.0
//!
//! Der integrierte A/D-Wandler des ESP8266 verfügt über einen
//!	Spannungsbereich von 0.0 - 1.0 V
//!
//! Um die Spannung des Akkus messen zu können, wird die Akkuspannung
//! durch einen Spannungsteiler (1M & 68k) entsprechend heruntergeteilt.
//!
// -------------------------------------------------------------------------------------------------
#ifndef __SOC__
#define __SOC__
// -------------------------------------------------------------------------------------------------
// Einbinden von Headern
// -------------------------------------------------------------------------------------------------
#include <stdint.h>
#include <Esp.h>
#include "settings.h"
// -------------------------------------------------------------------------------------------------
// Definition von Konstanten
// -------------------------------------------------------------------------------------------------
// Wie oft geoversampled werden soll. Maximal 255!
#define oversample 50
// Kalibrationswert. Hierfür müssen erst noch Erfahrungswerte gesammelt werden
#define calibValue 0.0154607
// -------------------------------------------------------------------------------------------------
// Klassendeklaration
// -------------------------------------------------------------------------------------------------
class SOC {
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
	SOC();
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
	float getVoltage();
	// -----------------------------------------------------------------------------------------------
	// Öffentliche Methoden
	// -----------------------------------------------------------------------------------------------

};

#endif __SOC__

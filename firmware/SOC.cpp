//! \file SOC.cpp
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
// Einbinden von Headern
// -------------------------------------------------------------------------------------------------
#include "SOC.h"

// -------------------------------------------------------------------------------------------------
// Definition entsprechend der Reihenfolge im Header
// -------------------------------------------------------------------------------------------------
SOC::SOC() {
	pinMode(A0, INPUT);
}
/*
 *	Gibt die Spannung in Volt aus
 * 	TODO: Kalibration durchführen und Werte auf internem Flash ablegen
 */
float SOC::getVoltage() {
	float sum = 0.0;
	#ifdef debugEnabled
	long sum2 = 0;
	#endif
	for(uint8_t foo = 0;foo<oversample;foo++) {
		#ifdef debugEnabled
		uint16_t readValue = analogRead(A0);
		sum2 += readValue;
		sum += readValue;
		#else
		sum += analogRead(A0);
		#endif
	}
	#ifdef debugEnabled
	Serial.print("SOC: Raw voltage value: ");
	Serial.println((1.0*sum2/oversample));
	#endif
	return (float)(calibValue*sum/oversample);
}
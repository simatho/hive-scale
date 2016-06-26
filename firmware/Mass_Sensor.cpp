//! \file
//! \brief Dieses Modul nimmt die Brückenquerspannung einer auf Dehnmessstreifen basierten
//! Wägezelle entgegen und rechnet diese in eine Masse um.
//!
//! \author Simon Diehl
//! \version 1.0


#include "Mass_Sensor.h"

void MassSensor::setOffset(float p_f_BridgeVoltage_V) {
	// Die aktuelle Brückenspannung, also Offset des Sensors plus gemessene Masse der Konstruktion
	// auf dem Sensor, wird als Offset angenommen.
	a_f_Offset_V = p_f_BridgeVoltage_V;
	#ifdef debugEnabled
	Serial.print("Offset gesetzt auf: ");
	Serial.print(a_f_Offset_V * 1e3, 6);
	Serial.println(" mV");
	#endif
}

MassSensor::MassSensor(float p_f_Gain_kgpV, float p_f_Offset_V) :
	a_f_Gain_kgpV(p_f_Gain_kgpV), a_f_Offset_V(p_f_Offset_V) {};

void MassSensor::calibGain(float p_f_BridgeVoltage_V, float p_f_Mass_kg) {
	// Brückenquerspannung um Offset bereinigen
	p_f_BridgeVoltage_V -= a_f_Offset_V;
	// Proportionalitätskonstante des Sensors aus korrigierter Brückenspannung und bekannter Masse
	// berechnen.
	a_f_Gain_kgpV = p_f_Mass_kg / p_f_BridgeVoltage_V;
	#ifdef debugEnabled
	Serial.print("Gain gesetzt auf: ");
	Serial.print(a_f_Gain_kgpV, 3);
	Serial.println(" kg/V");
	#endif
}

float MassSensor::calcMass(float p_f_BridgeVoltage_V) {
	// Brückenquerspannung um Offset bereinigen
	p_f_BridgeVoltage_V -= a_f_Offset_V;
	// Masse mit korrigierter Brückenquerspannung und Proportionalitätskonstante des Sensors
	// berechnen.
	float v_f_Mass_kg = p_f_BridgeVoltage_V * a_f_Gain_kgpV;
	//
	v_f_Mass_kg -= a_f_Tare_kg;
	#ifdef debugEnabled
	Serial.print("Offset: ");
	Serial.print(a_f_Offset_V * 1e3, 6);
	Serial.println(" mV");
	Serial.print("Gain: ");
	Serial.print(a_f_Gain_kgpV, 3);
	Serial.println(" kg/V");
	Serial.print("Korrigierte Brueckenquerspannung: ");
	Serial.print(p_f_BridgeVoltage_V * 1e3, 6);
	Serial.println(" mV");
	Serial.print("Tara: ");
	Serial.print(a_f_Tare_kg, 3);
	Serial.println(" kg");
	Serial.print("Errechnetes Gewicht: ");
	Serial.print(v_f_Mass_kg, 3);
	Serial.println(" kg");
	#endif
	return v_f_Mass_kg;
}

void MassSensor::tare(float p_f_Mass_kg) {
	// Tara um die aktuell gemessene Masse erhöhen und diese damit auf Null reduzieren-
	a_f_Tare_kg += p_f_Mass_kg;
	#ifdef debugEnabled
	Serial.print("Tara gesetzt auf: ");
	Serial.print(a_f_Tare_kg);
	Serial.print(" kg");
	#endif
}

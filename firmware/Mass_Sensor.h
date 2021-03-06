//! \file
//! \brief Dieses Modul nimmt die Brückenquerspannung einer auf Dehnmessstreifen basierten
//! Wägezelle entgegen und rechnet diese in eine Masse um.
//!
//! \author Simon Diehl
//! \version 1.0

#ifndef __MASS_SENSOR__
#define __MASS_SENSOR__
// ------------------------------------------------------------------------------------------------
// Einbinden von Headern
// ------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "settings.h"
// ------------------------------------------------------------------------------------------------
// Definition von Konstanten
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Deklaration von Enumerationen
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Klassendeklaration
// ------------------------------------------------------------------------------------------------
class MassSensor {
protected:
	// ---------------------------------------------------------------------------------------------
	// Private zu vererbende Attribute
	// ---------------------------------------------------------------------------------------------
	float a_f_Offset_V;
	float a_f_Gain_kgpV;
	float a_f_Tare_kg;
	// ---------------------------------------------------------------------------------------------
	// Private zu vererbende Methoden
	// ---------------------------------------------------------------------------------------------

public:
	// ---------------------------------------------------------------------------------------------
	// Konstruktoren
	// ---------------------------------------------------------------------------------------------
	MassSensor(float p_f_Gain_kgpV, float p_f_Offset_V = 0.0);
	// ---------------------------------------------------------------------------------------------
	// Operatoren
	// ---------------------------------------------------------------------------------------------

	// ---------------------------------------------------------------------------------------------
	// Öffentliche Attribute
	// ---------------------------------------------------------------------------------------------

	// ---------------------------------------------------------------------------------------------
	// Setter
	// ---------------------------------------------------------------------------------------------
	void setOffset(float p_f_BridgeVoltage_V);

	// ---------------------------------------------------------------------------------------------
	// Getter
	// ---------------------------------------------------------------------------------------------

	// ---------------------------------------------------------------------------------------------
	// Öffentliche Methoden
	// ---------------------------------------------------------------------------------------------
	void calibGain(float p_f_BridgeVoltage_V, float p_f_Mass_kg);

	float calcMass(float p_f_BridgeVoltage_V);

	void tare(float p_f_Mass_kg);
};

#endif // __MASS_SENSOR__

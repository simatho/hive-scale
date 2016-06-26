
// -------------------------------------------------------------------------------------------------
//! \file
//! \brief Dieses Modul ermöglicht die Kommunikation mit externen Temperatursensoren des Typs DS18B20
//!
//! \author Markus Hölzle
//!
//! Der DS18B20 ist ein digitaler Temperatursensor der Firma Maxim Integrated mit Anbindung über
//! das OneWire-Interface
//!
//! Die Sensore verfügen über:
//! * 64-bit Adresse
//! * 9 - 12-bit Auflösung
//!
//! Verwendung:
//! * initOneWire(..) oder setOneWire(..) zur Festlegung der OneWire-Instanz
//! * searchAddress(..) oder setAddress(..) zur Festlegung der Adresse
//! * setRes(..) zur Festlegung der Auflösung falls gewünscht
//! * getTemperature(..) zur Ermittlung der aktuellen Temperatur
//!
// -------------------------------------------------------------------------------------------------
#ifndef __DS18B20__
#define __DS18B20__
// -------------------------------------------------------------------------------------------------
// Einbinden von Headern
// -------------------------------------------------------------------------------------------------
#include <OneWire.h>
#include "settings.h"
// -------------------------------------------------------------------------------------------------
// Definition von Konstanten
// -------------------------------------------------------------------------------------------------
// Definition von Fehlercodes
// Verwendung mit Definition von "debugEnabled"
#define DS_NO_ERR 1
#define DS_ERR_ILLEGAL_ARG -1
#define DS_ERR_INIT -2
#define DS_ERR_NO_CON -3
#define DS_ERR_CRC -4
#define DS_ERR_SEARCH -5
// -------------------------------------------------------------------------------------------------
// Klassendeklaration
// -------------------------------------------------------------------------------------------------
class DS18B20 {
private:
	// -----------------------------------------------------------------------------------------------
	// Private Attribute
	// -----------------------------------------------------------------------------------------------
	OneWire * m_p_OneWire = nullptr;
	//! Auflösung (9 - 12 Bit)
	//! Powerup mit 12
	uint8_t m_u8_Res = 12;
	//! Aktuell Rohdaten
	uint8_t m_a12u8_Data[12];
   //! ROM-Adresse
   uint8_t m_a8u8_Addr[8];
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

	//! Erzeugt ein generisches DS18B20-Objekt
	//
	DS18B20();

	// -----------------------------------------------------------------------------------------------
	// Operatoren
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Öffentliche Attribute
	// -----------------------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------------------
	// Setter
	// -----------------------------------------------------------------------------------------------

   //! Setzt die zu verwendende Auflösung für den Sensor
	//!
	//! \param p_u8_Res Zu verwendende Auflösung (9 - 12 Bit Standard = 9)
	//! \return Fehlercode oder 1
   uint8_t setRes(uint8_t p_u8_Res = 9);

   //! Setzt die zu verwendende Adresse für den Sensor
	//!
	//! \param p_pu8_Addr Pointer auf die zu verwendende Adresse
	//! \return Fehlercode oder 1
   uint8_t setAddress(uint8_t * p_pu8_Addr);

   //! Setzt die zu verwendende OneWire-Instanz für den Sensor
	//!
	//! \param p_p_OneWire Pointer auf die zu verwendende OneWire-Instanz
	//! \return Fehlercode oder 1
   uint8_t setOneWire(OneWire * p_p_OneWire);

	// -----------------------------------------------------------------------------------------------
	// Getter
	// -----------------------------------------------------------------------------------------------

	//! Startet die Wandlung und liest die Temperatur aus
	//!
	//! \param p_r_ri16_Temp_mC Schreiben des Temperaturwertes auf die Referenz
	//! \return Fehlercode oder 1
	uint8_t readTemp(float & p_rf_Temp_C);

	//! Ermittlung der verwendeten OneWire-Instanz
	//! \param p_pr_OneWire Rückgabe des Zeigers auf die verwendete OneWire-Instanz
	//! \return Fehlercode oder 1
	uint8_t getOneWire(OneWire* & p_pr_OneWire);

	//! Ermittlung Adresse des Sensors
	//! \param p_rp_addr Rückgabe des Zeigers auf die verwendete Adresse
	//! \return Fehlercode oder 1
	uint8_t getAddr(uint8_t* & p_pr_addr);

	// -----------------------------------------------------------------------------------------------
	// Öffentliche Methoden
	// -----------------------------------------------------------------------------------------------

	//! Sucht die Adresse des Sensors und setzt diese direkt
	//!
	//! \param p_u8_nOnBus Stelle des Sensors auf dem Bus (Standard nächster Sensor)
	//! \return Fehlercode oder 1
	uint8_t searchAddress(uint8_t p_u8_nOnBus = 0);
};

#endif __DS18B20__

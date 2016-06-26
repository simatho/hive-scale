// -------------------------------------------------------------------------------------------------
//! \file
//! \brief Dieses Modul ermöglicht die Kommunikation mit externen Temperatursensoren des Typs DS18B20
//!
//! \author Markus Hölzle
//!
// -------------------------------------------------------------------------------------------------
// Einbinden von Headern
// -------------------------------------------------------------------------------------------------
#include "DS18B20.h"
// -------------------------------------------------------------------------------------------------
// Definition entsprechend der Reihenfolge im Header
// -------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------
// Konstruktoren
// -----------------------------------------------------------------------------------------------

//! Erzeugt ein generisches DS18B20-Objekt
//
DS18B20::DS18B20(){}

// -----------------------------------------------------------------------------------------------
// Operatoren
// -----------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------
// Öffentliche Attribute
// -----------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------
// Setter
// -----------------------------------------------------------------------------------------------


uint8_t DS18B20::setRes(uint8_t p_u8_Res /*= 9*/){
	bool present = m_p_OneWire->reset();
	m_u8_Res = p_u8_Res;
	#ifdef debugEnabled
	if (m_p_OneWire == nullptr)
		return DS_ERR_INIT;
	if(!present)
		return DS_ERR_NO_CON;
	if (9 < p_u8_Res)																											// Anpassung bei ungültiger Angabe
		m_u8_Res = 9;
	if (p_u8_Res > 12)
		m_u8_Res = 12;
	#endif
	m_p_OneWire->select(m_a8u8_Addr); 																					// Sensor wählen
	m_p_OneWire->write(0x4E);																								// Konfiguration des Sensors starten
	m_p_OneWire->write(0x00);																								// Temperatur für Alarm (T_H)
	m_p_OneWire->write(0x00);																								// Temperatur für Alarm (T_L)
	m_p_OneWire->write(0x1F + ( (m_u8_Res - 0x8) << 5));															// Auflösung in Bit Setzen
	m_p_OneWire->reset();
	return DS_NO_ERR;
}


uint8_t DS18B20::setAddress(uint8_t * p_pu8_Addr){
	#ifdef debugEnabled
	if (p_pu8_Addr == nullptr)
		return DS_ERR_ILLEGAL_ARG;
	#endif
	for(int i = 0; i < 8; i++)																								// Adresse kopieren
		m_a8u8_Addr[i] = p_pu8_Addr[i];
	return DS_NO_ERR;
}


uint8_t DS18B20::setOneWire(OneWire * p_p_OneWire){
	#ifdef debugEnabled
	if (p_p_OneWire == nullptr)
		return DS_ERR_ILLEGAL_ARG;
	#endif
	m_p_OneWire = p_p_OneWire;																								// Zeiger kopieren
	return DS_NO_ERR;
}

// -----------------------------------------------------------------------------------------------
// Getter
// -----------------------------------------------------------------------------------------------

uint8_t DS18B20::readTemp(float & p_rf_Temp_C){
	m_p_OneWire->reset();
	m_p_OneWire->select(m_a8u8_Addr); 																					// Sensor wählen
	m_p_OneWire->write(0x44);																								// Wandeln starten
	delay(200 + 250 * m_u8_Res);																							// Wartezeit berechnen
	bool present = m_p_OneWire->reset();
	m_p_OneWire->select(m_a8u8_Addr);
	m_p_OneWire->write(0xBE);																								// Starte Auslesen
	for( int i = 0; i < 9; i++){																							// Lese 9 Bytes
		m_a12u8_Data[i] = m_p_OneWire->read();
	}
	#ifdef debugEnabled
	if (m_p_OneWire == nullptr)
		return DS_ERR_INIT;
	if(!present)
		return DS_ERR_NO_CON;
	if(m_p_OneWire->crc8(m_a12u8_Data,8) != m_a12u8_Data[8])														// CRC prüfen
		return DS_ERR_CRC;
	#endif
	int16_t v_i16_tmpRaw = ( ( m_a12u8_Data[1] << 8 ) | m_a12u8_Data[0] );									// In °C konvertieren
	p_rf_Temp_C = (float) ( v_i16_tmpRaw * 0.0625 ) ;
	return DS_NO_ERR;
}

uint8_t DS18B20::getOneWire(OneWire* & p_pr_OneWire){
		p_pr_OneWire =  m_p_OneWire;																						// Zeiger auf OneWire-Instanz zurückgeben
		return DS_NO_ERR;
}

uint8_t DS18B20::getAddr(uint8_t* & p_pr_addr){
		p_pr_addr =  m_a8u8_Addr;																							// Zeiger auf Adresse zurückgeben
		return DS_NO_ERR;
}

// -----------------------------------------------------------------------------------------------
// Öffentliche Methoden
// -----------------------------------------------------------------------------------------------

uint8_t DS18B20::searchAddress(uint8_t p_u8_nOnBus /*= 0*/){
	bool present = m_p_OneWire->reset();
	m_p_OneWire->target_search(0x28);																					//Familiencode des Sensors angeben
	if (p_u8_nOnBus)																											//Suche auf Null setzen wenn Nummer auf Bus angegeben
		m_p_OneWire->reset_search();
	else																															//Verwendung des nächsten Sensors wenn nicht angegeben
		p_u8_nOnBus = 1;
		bool found = false;
	for(int i = 0; i < p_u8_nOnBus; i++){																				//Durchführung der Suche
		found = m_p_OneWire->search(m_a8u8_Addr);
	}
	#ifdef debugEnabled
	if (m_p_OneWire == nullptr)
		return DS_ERR_INIT;
	if(!present)
		return DS_ERR_NO_CON;
	if(!found)
		return DS_ERR_SEARCH;
	if (OneWire::crc8(m_a8u8_Addr,7) != m_a8u8_Addr[7])															// Prüfen der Gültigkeit der Adresse
		return DS_ERR_CRC;
	#endif
	return DS_NO_ERR;
}

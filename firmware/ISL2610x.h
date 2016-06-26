//! \file
//! \brief Dieses Modul ermöglicht die Kommunikation mit der ADU-Familie ILS2610x von Intersil.
//!
//! \author Simon Diehl
//! \version 0.0
//!
//! ILS2610x ist eine Familie von 24 b Delta-Sigma Analog-Digial-Umsetzern der Intersil
//! Corporation.
//!
//! Diese bieten unter anderem:
//! * Bis zu vier Kanäle
//! * Kommunikation über SPI (CPOL = 0; CPHA = 0)
//! * Interner analoger Verstärker mit programmierbarem Verstärkungsfaktor 1..128 und maximal
//! 49 nV²/Hz Rauschleistung
//! * Abstastrate bis zu 4 kSPS und internes Oversampling wenn eine geringere Abtastrate gewählt
//! wird
//! * Open-Drain-Schalter der die Messbrücke abschaltet um Energie zu sparen wenn nicht abgetastet
//! wird

#ifndef __ISL2610x__
#define __ISL2610x__
// ------------------------------------------------------------------------------------------------
// Einbinden von Headern
// ------------------------------------------------------------------------------------------------
#include <cmath>
#include <stdint.h>
#include <Arduino.h>
#include <Esp.h>
#include <SPI.h>
#include "settings.h"
// ------------------------------------------------------------------------------------------------
// Definition von Konstanten
// ------------------------------------------------------------------------------------------------
//! Maximale Anzahl von differenziellen Kanälen, die in der ISL2610x-Familie vorkommt.
#define MAX_nCHANNELS 4
#define ISL_READ 0
#define ISL_WRITE 1
// ------------------------------------------------------------------------------------------------
// Deklaration von Enumerationen
// ------------------------------------------------------------------------------------------------
typedef enum {
	//! Die Operation wurde ohne Fehler durchgeführt.
	NO_ERROR,
	//! Es wird versucht auf den ADU zuzugreifen obwohl sich dieser im Standby befindet.
	ADC_INACTIVE,
	//! Es wird versucht mit einem Kanal zu arbeiten, der noch nicht konfiguriert wurde.
	CH_NOT_INITIALISED
} t_Errorcode;
//! Enumeration für Anzahl der differenziellen Kanäle des ADU in Abhängigkeit vom Typ
enum e_nCh {
	//! Typ mit zwei differenziellen Kanäle
	ISL26102 = 2,
	//! Typ mit vier differenziellen Kanäle
	ISL26104 = 4
};
//! Enumeration für Standbymodus
enum e_Standby {
	//! Standbymodus ist deaktiviert --> ADU ist aktiv
	STANDBY_DISABLE = 0,
	//! Standbymodus ist aktiviert --> ADU ist inaktiv
	STANDBY_ENABLE = 1
};
//! Enumeration für Zustand des Schalters für die Versorgung der Messbrücke
enum e_LSPS {
	//! Schalter ist deaktiviert --> Es fließt kein Strom über die Messbrücke, daher liegt am ADU
	//! kein gültiges Signal an
	LSPS_DISABLE = 0,
	//! Schalter ist aktiviert --> Es fließt Strom über die Messbrücke, daher liegt am ADU ein
	//! gültiges Signal an
	LSPS_ENABLE = 1
};
//! Enumeration für Einstellung des ausgewählten Eingangs
enum e_InputSelection {
	//! Kanal 1
	CH1 = 0,
	//! Kanal 2
	CH2 = 1,
	//! Kanal 3 (Relevant für ISL26104)
	CH3 = 2,
	//! Kanal 4 (Relevant für ISL26104)
	CH4 = 3,
	//! Versorgungsspannung
	SUPPLY = 4,
	//! Interner Temperatursensor
	TEMPERATURE = 5
};
// ------------------------------------------------------------------------------------------------
// Deklaration von Strukturen
// ------------------------------------------------------------------------------------------------
//! Struktur für Kanal abhängige Einstellungen
struct s_ChConfig {
	//! Mit dieser Rate stellt der ADU Datenwörter zur Verfügung. Liegt diese Rate unter der
	//! höchst möglichen (4 kSPS) dann werden die Zwischenwerte für internes Oversampling
	//! genutzt.
	//!
	//! | Wortrate [SPS] | Code |
	//! | :------------:	| :--: |
	//! | 2.5				| 0x00 |
	//! | 5					| 0x01 |
	//! | 10					| 0x02 |
	//! | 20					| 0x03 |
	//! | 40					| 0x04 |
	//! | 80					| 0x05 |
	//! | 100				| 0x0B |
	//! | 160				| 0x06 |
	//! | 200				| 0x0C |
	//! | 320				| 0x07 |
	//! | 400				| 0x0D |
	//! | 640				| 0x08 |
	//! | 800				| 0x0E |
	//! | 1000				| 0x11 |
	//! | 1280				| 0x09 |
	//! | 1600				| 0x0F |
	//! | 2000				| 0x12 |
	//! | 2560				| 0x0A |
	//! | 3200				| 0x10 |
	//! | 4000				| 0x13 |
	//
	uint8_t u8_OutputWordRate;
	//! Verstärkungsfaktor des internen Verstärkers.
	//!
	//! | Faktor | Code |
	//! | :----: | :--: |
	//! | 1		 | 0x0  |
	//! | 2		 | 0x1  |
	//! | 4		 | 0x2  |
	//! | 8		 | 0x3  |
	//! | 16		 | 0x4  |
	//! | 32		 | 0x5  |
	//! | 64		 | 0x6  |
	//! | 128	 | 0x7  |
	//
	unsigned int u3_PGA_Gain : 3;
	//! Dieses Flag kennzeichnet, dass der entsprechende Kanal konfiguriert wurde und die
	//! gespeicherte Konfiguration gültig ist.
	//!
	//! Muss nicht vom Nutzer gesetzt werden.
	bool b_Initialised;
};
//! Struktur für Aufbau einer SPI Nachricht zur Übertragung von Einstellungen
struct s_SPI_ConfigMessage {
	//! Dies ist die Adresse des Registers das gelesen oder geschrieben werden soll.
	//!
	//! Die Funktion der Register ist dem Datenblatt zu entnehmen.
	//!
	//! | Register            | Adresse |
	//! | :------------------ | :-----: |
	//! | Chip-ID             | 0x00    |
	//! | SDO/LSPS            | 0x02    |
	//! | Standby             | 0x03    |
	//! | Output Word Rate    | 0x05    |
	//! | Input Mux Selection | 0x07    |
	//! | Channel Pointer     | 0x08    |
	//! | PGA Gain            | 0x17    |
	//! | Conversion Control  | 0x04    |
	//! | Delay Timer         | 0x42    |
	//! | PGA Offset MSB      | 0x3D    |
	//! | PGA Offset          | 0x3E    |
	//! | PGA Offset LSB      | 0x3F    |
	//! | PGA Monitor         | 0x3C    |
	//
	uint32_t u7_RegAddr : 7;
	//! Dieses bit setzt den Zugriffsmodus.
	uint32_t u1_Acc : 1;
	//! Dies ist das Daten-byte.
	//!
	//! Der zum Register passende Inhalt ist dem Datenblatt zu entnehmen.
	uint32_t u8_Data : 8;
};
// ------------------------------------------------------------------------------------------------
// Klassendeklaration
// ------------------------------------------------------------------------------------------------
//! \brief Klasse für ADU aus der ISL2610x-Familie.
class ISL2610x {
protected:
	// ---------------------------------------------------------------------------------------------
	// Private zu vererbende Attribute
	// ---------------------------------------------------------------------------------------------
	//! Anzahl der differenziellen Kanäle des ADU.
   const enum e_nCh a_e_nCh;
   //! Nummer des Pins an dem der Eingang /PDWN des ADU angeschlossen ist.
   const uint8_t a_u8_PinPDWNn;
   //! Nummer des Pins an dem der Eingang /CS des ADU angeschlossen ist.
   const uint8_t a_u8_PinCSn;
   //! Referenz auf das SPI an welchem der ADU angeschlossen ist. Die richtige Konfiguration wird
   //! bei Verwendung vorgenommen.
   SPIClass & a_rc_SPI;
   //! Einstellungen für SPI beim Lesen und Schreiben von Registern
   const SPISettings a_c_SPISettings_Reg;
   //! Einstellungen für SPI beim Lesen des ADU-Wertes
   const SPISettings a_c_SPISettings_Code;
   //! Aktivitäts-Zustand des ISL2610x.
   //!
   //! Es gibt grundsätzlich zwei Möglichkeiten wie der ADU in den Ruhezustand versetzt werden
   //! kann:
   //! * Über einen Eingang des ADU (/PDWN)
   //! * Über eine Nachricht
   //! In der aktuellen Implementierung wird die Steuerung mittels Eingang genutzt.
   enum e_Standby a_e_Standby;
   //! Zustand des Schalters für die Versorgung der Messbrücke
   bool a_b_LSPS_IsOn;
   //! Konfiguration der Kanäle des ISL2610x.
   //!
   //! Kanal 1 ist beim Index 0 hinterlegt.
   struct s_ChConfig a_sa_ChConfig[MAX_nCHANNELS];
	// ---------------------------------------------------------------------------------------------
	// Private zu vererbende Methoden
	// ---------------------------------------------------------------------------------------------
   //! Berechnet die Kanalnummer in Abhängigkeit vom ADU Typ
   //!
   //! \param p_e_Ch Kanal, dessen Kanalnummer berechnet werden soll.
   //! \return Kanalnummer angepasst für den richtigen ADU Typ.
	//!
   unsigned int calcChNumber(enum e_InputSelection p_e_Ch);
   //! Überträgt die gesetzte Konfiguration für den Kanal an den ADU.
   //!
   //! \param p_e_Ch Kanal, dessen Konfiguration an den ADU übertragen werden soll.
   //! \return Fehlercode
   //!
   t_Errorcode writeChConfig(enum e_InputSelection p_e_Ch);
   //! Überträgt eine Konfigurationsnachricht an den ADU.
   //!
   //! Hier ist die in der Arduino library enthaltene Methode SPIClass::writeBytes um die
   //! Verwaltung der SS-Leitung und der benötigten Verzögerungen ergänzt
   //!
   //! @param      p_ps_ConfigMessage  Adresse der Konfigurationsnachricht
   //!
   //! @return     Fehlercode
   //!
   t_Errorcode spiWriteConfig(s_SPI_ConfigMessage * p_ps_ConfigMessage);
   //! Reaktiviert den ADU aus dem Ruhezustand
   //!
   void wakeup();
   //! Versetzt den ISL2610x in den Ruhezustand.
   //!
	void standby();
	/**
	 * @brief      Schaltet die Messbrücke
	 *
	 * @param[in]  p_b_LSPS_IsOn  Gewünschter Schaltzustand: true := EIN
	 */
	void switchLSPS(bool p_b_LSPS_IsOn);

public:
	// ---------------------------------------------------------------------------------------------
	// Konstruktoren
	// ---------------------------------------------------------------------------------------------
	//! Elementare Attribute (hier als unveränderlich deklariert) initialisieren und
	//! Standardkonfiguration des ADU vornehmen.
	//!
	//! Standardkonfiguration:
	//! * Signal /RDY deaktiviert
	//! * LSPS deaktiviert
	//! * 100 µs delay
	//!
	//!
	//! \param p_e_nCH			Anzahl der differenziellen Kanäle des ADU.
	//! \param p_u8_PinPDWNn	Nummer des Pins an dem der Eingang /PDWN des ADU angeschlossen ist.
	//! \param p_u8_PinCSn		Nummer des Pins an dem der Eingang /CS des ADU angeschlossen ist.
	//! \param p_pc_SPI			Adresse des SPI an dem der ADU betrieben wird.
	//
	ISL2610x(enum e_nCh p_e_nCH, uint8_t p_u8_PinPDWNn, uint8_t p_u8_PinCSn, SPIClass & p_pc_SPI);
	// ---------------------------------------------------------------------------------------------
	// Operatoren
	// ---------------------------------------------------------------------------------------------

	// ---------------------------------------------------------------------------------------------
	// Öffentliche Attribute
	// ---------------------------------------------------------------------------------------------

	// ---------------------------------------------------------------------------------------------
	// Setter
	// ---------------------------------------------------------------------------------------------
	//! Setzt die Konfiguration für einen Kanal.
	//!
	//! Die Konfiguration wird mit readVoltage() übertragen.
	//!
	//! \param p_e_Ch 			Kanal, der konfiguriert werden soll.
	//! \param p_rs_ChConfig 	Adresse der Konfiguration für den Kanal.
	//
	void setChConfig(enum e_InputSelection p_e_Ch, struct s_ChConfig & p_rs_ChConfig);
	// ---------------------------------------------------------------------------------------------
	// Getter
	// ---------------------------------------------------------------------------------------------

	// ---------------------------------------------------------------------------------------------
	// Öffentliche Methoden
	// ---------------------------------------------------------------------------------------------
	t_Errorcode init();
	//! Kalibriert den internen Verstärker für alle verwendeten Verstärkungsfaktoren und Wortraten.
	//!
	//! \param p_e_Ch Wahl des Kanals, der kalibriert werden soll
	//! \return Fehlercode
	//
	t_Errorcode calibCh(enum e_InputSelection p_e_Ch);
	//! Weckt den ADU auf und misst mit dem internen Temperatursensor die Gehäusetemperatur.
	//!
	//! \param p_rf_Result_dgC Adresse der Variable in die die Temperatur [°C] geschrieben werden
	//! soll.
	//! \return Fehlercode
	//
	t_Errorcode readTemp(float & p_rf_Result_dgC);
	//! Liest den gewählten Kanal aus und berechnet die Spannung.
	//!
	//! \param p_e_Ch Wahl des Kanals, dessen Spannung gelesen werden soll
	//! \param p_rf_Result_V Adresse der Variable in die die Spannung [V] geschrieben werden
	//! soll.
	//! \return Fehlercode
	//
	t_Errorcode readVoltage(enum e_InputSelection p_e_Ch, float & p_rf_Result_V);
};

#endif // __ISL2610x__

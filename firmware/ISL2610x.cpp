//! \file
//! \brief Dieses Modul ermöglicht die Kommunikation mit der ADU-Familie ILS2610x von Intersil.
//!
//! \author Simon Diehl
//! \version 0.0
// ------------------------------------------------------------------------------------------------
// Einbinden von Headern
// ------------------------------------------------------------------------------------------------
#include "ISL2610x.h"
// ------------------------------------------------------------------------------------------------
// Definition von Methoden entsprechend der Reihenfolge im Header
// ------------------------------------------------------------------------------------------------
unsigned int ISL2610x::calcChNumber(enum e_InputSelection p_e_Ch) {
	// Zwischenspeicher für Berechnung der Kanalnummer. Dies ist notwendig, da die Kanäle SUPPLY und
	// TEMPERATURE immer die höchsten Kanalnummern des jeweiligen ADU einnehmen und nicht bei jedem
	// Modell auf dem gleichen Wert zu finden sind. Da bei der verwendeten enumeration den beiden
	// Kanälen jedoch immer der höchste überhaupt auftretende Wert zugewiesen wird muss für die
	// Modelle mit weniger Kanälen eine Anpassung vorgenommen werden.
	unsigned int v_u_Ch = p_e_Ch;
	// Anpassung berechnen
	if(p_e_Ch == SUPPLY || p_e_Ch == TEMPERATURE) {
		// Von der aktuellen Nummer des Kanals v_u_Ch wird die menge Kanäle abgezogen, die das Modell
		// gegenüber dem Modell mit den meisten Kanälen nicht hat und somit werden die Kanalnummern
		// für SUPPLY und TEMPERATURE direkt hinter die differenziellen Kanäle gelegt.
		v_u_Ch -= MAX_nCHANNELS - a_e_nCh;
	}
	return v_u_Ch;
}

t_Errorcode ISL2610x::writeChConfig(enum e_InputSelection p_e_Ch) {
	// Konfigurationsnachrichten erstellen
	s_SPI_ConfigMessage v_sa3_ConfigMessages[3] = {
		// Register Output Word Rate
		{0x05, ISL_WRITE, a_sa_ChConfig[p_e_Ch].u8_OutputWordRate},
		// Register Channel Pointer
		{0x08, ISL_WRITE, calcChNumber(p_e_Ch) },
		// Register PGA Gain
		{0x17 ,ISL_WRITE, a_sa_ChConfig[p_e_Ch].u3_PGA_Gain}
	};
	t_Errorcode v_t_Errorcode;
	for(int i = 0; i < 3; i ++) {
		v_t_Errorcode = spiWriteConfig(v_sa3_ConfigMessages + i);
		if(v_t_Errorcode != NO_ERROR) return v_t_Errorcode;
	}
	return NO_ERROR;
}

t_Errorcode ISL2610x::spiWriteConfig(s_SPI_ConfigMessage * p_ps_ConfigMessage) {
	// Methode abbrechen, wenn ADU im Standby ist.
	if(a_e_Standby == STANDBY_ENABLE) return ADC_INACTIVE;
	else;
	// SPI konfigurieren und semaphore nehmen
	a_rc_SPI.beginTransaction(a_c_SPISettings_Reg);
	// Interface des ADU aktivieren
	digitalWrite(a_u8_PinCSn, LOW);
	// Konfigurationsnachricht byteweise übertragen
	a_rc_SPI.writeBytes( (uint8_t *)p_ps_ConfigMessage, 2);
	// Interface des ADU deaktivieren
	digitalWrite(a_u8_PinCSn, HIGH);
	// Semaphore freigeben
	a_rc_SPI.endTransaction();
	// Zeit zum Schreiben in Register abwarten.
	delay(1);
	return NO_ERROR;
}

void ISL2610x::wakeup() {
	digitalWrite(a_u8_PinPDWNn, HIGH);
	// Warten bis ADU bereit ist
	delay(13);
	a_e_Standby = STANDBY_DISABLE;
}

void ISL2610x::standby() {
	digitalWrite(a_u8_PinPDWNn, LOW);
	a_e_Standby = STANDBY_ENABLE;
}

void ISL2610x::switchLSPS(bool p_b_LSPS_IsOn) {
	// Methode abbrechen, wenn LSPS bereits im gewünschten Schaltzustand ist.
	if(a_b_LSPS_IsOn == p_b_LSPS_IsOn) return;
	else;
	// Konfigurationsnachricht erstellen
	// Register SDO/LSPS
	// Disable SDO & LSPS On
	s_SPI_ConfigMessage v_s_ConfigMessage =
		{0x02, ISL_WRITE, ( (!p_b_LSPS_IsOn << 1) | p_b_LSPS_IsOn) };
	// Konfiguration übertragen
	spiWriteConfig(&v_s_ConfigMessage);
	a_b_LSPS_IsOn = p_b_LSPS_IsOn;
}


ISL2610x::ISL2610x(enum e_nCh p_e_nCH,
                   uint8_t p_u8_PinPDWNn,
                   uint8_t p_u8_PinCSn,
                   SPIClass & p_pc_SPI) :
	a_e_nCh(p_e_nCH),
	a_u8_PinPDWNn(p_u8_PinPDWNn),
	a_u8_PinCSn(p_u8_PinCSn),
	a_rc_SPI(p_pc_SPI),
	a_c_SPISettings_Reg(4000000, MSBFIRST, SPI_MODE0),
	a_c_SPISettings_Code(4000000, MSBFIRST, SPI_MODE1),
	a_sa_ChConfig( {} )
{ }

void ISL2610x::setChConfig(enum e_InputSelection p_e_Ch, struct s_ChConfig & p_rs_ChConfig) {
	// Übergebene Konfigurationsdaten in Speicher übernehmen
	a_sa_ChConfig[p_e_Ch] = p_rs_ChConfig;
	// Signalisieren, dass dieser Kanal vom Nutzer initialisiert wurde
	a_sa_ChConfig[p_e_Ch].b_Initialised = true;
}

t_Errorcode ISL2610x::init() {
	SPI.begin();
	// Ausgänge für die Steuerung des ADU konfigurieren
	pinMode(a_u8_PinPDWNn, OUTPUT);
	pinMode(a_u8_PinCSn, OUTPUT);
	// Definierten Zustand des ADU herstellen
	digitalWrite(a_u8_PinCSn, HIGH);
	digitalWrite(a_u8_PinPDWNn, LOW);
	a_e_Standby = STANDBY_ENABLE;
	// Konfigurationsnachrichten erstellen
	s_SPI_ConfigMessage v_sa2_ConfigMessages[2] = {
		// Register SDO/LSPS
		// Disable SDO & LSPS OFF
		{0x02, ISL_WRITE, 0x02},
		// Register Delay Timer
		// 100e-6 s delay
		{0x42, ISL_WRITE, 0x00}
	};
	wakeup();
	// Konfiguration übertragen
	t_Errorcode v_t_Errorcode;
	for(int i = 0; i < 2; i ++) {
		v_t_Errorcode = spiWriteConfig(v_sa2_ConfigMessages + i);
	}
	switchLSPS(true);
	// Die grundsätzlichen Einstellungen des ADU sind nun vorgenommen. Nach der Konfiguration der
	// differenziellen Kanäle kann der ADU verwendet werden.
}

t_Errorcode ISL2610x::calibCh(enum e_InputSelection p_e_Ch) {
	// Methode abbrechen, wenn ADU im Standby ist.
	if(a_e_Standby == STANDBY_ENABLE) return ADC_INACTIVE;
	else;
	// Methode abbrechen, wenn Kanal nicht konfiguriert ist.
	if(a_sa_ChConfig[p_e_Ch].b_Initialised == false) return CH_NOT_INITIALISED;
	else;
	switchLSPS(true);
	// Verstärkungsfaktor für Kanal konfigurieren
	writeChConfig(p_e_Ch);
	// Konfigurationsnachrichten erstellen
	s_SPI_ConfigMessage v_sa5_ConfigMessages[5] = {
		// Register Output Word Rate
		// 2.5 SPS
		{0x05, ISL_WRITE, 0x00},
		// Register Input Mux Selection
		{0x07, ISL_WRITE, calcChNumber(p_e_Ch) },
		// Register Conversion Control
		// Start continuous conversions
		{0x04, ISL_WRITE, 0x02},
		// Register Conversion Control
		// Start channel calibration
		{0x04, ISL_WRITE, 0x06},
		// Register Conversion Control
		// Stop continuous conversions
		{0x04, ISL_WRITE, 0x00}
	};
	// Konfiguration übertragen
	t_Errorcode v_t_Errorcode;
	for(int i = 0; i < 3; i ++) {
		v_t_Errorcode = spiWriteConfig(v_sa5_ConfigMessages + i);
		if(v_t_Errorcode != NO_ERROR) return v_t_Errorcode;
	}
	delay(1000);
	v_t_Errorcode = spiWriteConfig(v_sa5_ConfigMessages + 3);
	delay(3000);
	v_t_Errorcode = spiWriteConfig(v_sa5_ConfigMessages + 4);
	return NO_ERROR;
}

t_Errorcode ISL2610x::readTemp(float & p_rf_Result_dgC) {
	// Spannung über dem Temperatursensor messen
	float v_f_TempVoltage_V;
	readVoltage(TEMPERATURE, v_f_TempVoltage_V);
	// Entsprechend der Formel aus dem Datenblatt die Temperatur berechnen
	// 3.0 subtrahieren wegen Wärmeübergangswiderstand des Gehäuses
	p_rf_Result_dgC = ( (v_f_TempVoltage_V - 102.2e-3) / 379e-6) - 3.0;
	return NO_ERROR;
}

t_Errorcode ISL2610x::readVoltage(enum e_InputSelection p_e_Ch, float & p_rf_Result_V) {
	// ADU reaktivieren, wenn dieser im Standby ist
	if(a_e_Standby == STANDBY_ENABLE) wakeup();
	else;
	// Methode abbrechen, wenn Kanal nicht konfiguriert ist.
	// p_e_Ch < (MAX_nCHANNELS - 1) wertet aus, ob es sich um einen differenziellen Kanal handelt
	switchLSPS(true);
	if(p_e_Ch < (MAX_nCHANNELS - 1) && a_sa_ChConfig[calcChNumber(p_e_Ch)].b_Initialised == false)
		return CH_NOT_INITIALISED;
	// Kanal konfigurieren
	else writeChConfig(p_e_Ch);
	// Register Conversion Control
	// Start continuous conversions
	s_SPI_ConfigMessage v_sa2_ConfigMessage[2] = {
		// Register Input Mux
		{0x07, ISL_WRITE, calcChNumber(p_e_Ch) },
		// Register Conversion Control
		// Start continuous conversion
		{0x04, ISL_WRITE, 0x02}
	};
	// Konfiguration übertragen
	for(int i = 0; i < 2; i ++) {
		spiWriteConfig(v_sa2_ConfigMessage + i);
	}
	// Warten bis ADU auf Betriebstemperatur
	delay(1000);
	// Messbrücke einschalten
	// Warten bis Ergebnis bereit
	delay(2000);
	uint32_t v_u32_ADC_Code = 0;
	a_rc_SPI.beginTransaction(a_c_SPISettings_Code);
	a_rc_SPI.transferBytes(NULL, (uint8_t *)&v_u32_ADC_Code, 3);
	a_rc_SPI.endTransaction();
	// Messbrücke ausschalten
	switchLSPS(false);
	int32_t bla = ( ( (v_u32_ADC_Code & 0xFF) << 24) | ( (v_u32_ADC_Code & 0xFF00) << 8) | ( (v_u32_ADC_Code & 0xFF0000) >> 8) ) >> 8;
	p_rf_Result_V = 2.5 * bla / pow(2, a_sa_ChConfig[calcChNumber(p_e_Ch)].u3_PGA_Gain) / 0x7FFFFF;
}

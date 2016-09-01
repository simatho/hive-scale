//! \file Updater.cpp
//! \brief Diese Klasse kümmert sich um das automatische Update via HTTP
//!
//! \author Thomas Pfister
//! \version 1.0
//!
//! Die Klasse ist für den Over-the-air (OTA)-Updatevorgang verantwortlich.
//!
// -------------------------------------------------------------------------------------------------
#include "Updater.h"

Updater::Updater() {}

String Updater::getVersion() {
	return VERSION;
}

void Updater::update(const String& filename) {
	//SPIFFS.end(); // Beende alle Operationen auf dem SPIF-FS
	delete v_r_bridgeADC;
	delete v_p_oneWire;
	t_httpUpdate_return ret = ESPhttpUpdate.update(String ("https://") + cfg_otaHost + "/" +  filename, this->getVersion(), cfg_sha1Fingerprint);
	switch(ret) {
	case HTTP_UPDATE_FAILED:
		Serial.printf("Update fehlgeschlagen (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
		break;
	case HTTP_UPDATE_NO_UPDATES:
		Serial.println("Kein Update verfügbar.");
		break;
	case HTTP_UPDATE_OK:
		Serial.println("HTTP_UPDATE_OK");
		break;
	}
	ESP.restart();
	//SPIFFS.begin(); // Falls das Update fehlgeschlagen ist oder kein Update verfügbar war: SPIFFS wieder starten
}
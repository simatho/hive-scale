//! \file Updater.h
//! \brief Diese Klasse kümmert sich um das automatische Update via HTTP
//!
//! \author Thomas Pfister
//! \version 1.0
//!
//! Die Klasse ist für den Over-the-air (OTA)-Updatevorgang verantwortlich.
//!
// -------------------------------------------------------------------------------------------------
#ifndef __UPDATER__
#define __UPDATER__
// -------------------------------------------------------------------------------------------------
// Einbinden von Headern
// -------------------------------------------------------------------------------------------------
#include "settings.h"
#include "version.h"
#ifndef disableOTA
#include <ESP8266httpUpdate.h>
#endif

// -------------------------------------------------------------------------------------------------
// Definition von Konstanten
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// Klassendeklaration
// -------------------------------------------------------------------------------------------------
class Updater {
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

	/**
	 * @brief      Constructor (does nothing)
	 */
	Updater();
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

	/**
	 * @brief      Gets the current version number.
	 *
	 * @return     The version number string.
	 */
	static String getVersion();
	// -----------------------------------------------------------------------------------------------
	// Öffentliche Methoden
	// -----------------------------------------------------------------------------------------------

	/**
	 * @brief      Sucht auf dem in "settings.h" hinterlegten Update-Server nach
	 *             Updates. Die Updates werden per https übertragen, deshalb
	 *             muss der SHA1-Fingerprint des verwendeten Zertifikats
	 *             ebenfalls hinterlegt werden. Hier kann ein beliebiges - auch
	 *             selbst-signiertes Zertifikat verwendet werden, allerdings
	 *             sollte beim Zertifikatswechsel aufgepasst werden, sonst
	 *             könnte das Update fehlschlagen. (Alle ESPs gleichzeitig
	 *             updaten, eine Weile lang beide Fingerprints hinterlegen, ...)
	 *             Von der Verwendung von http anstatt von https raten wir
	 *             explizit ab!
	 *
	 *             Standardmäßig macht diese Funktion ein "normales" Update,
	 *             d.h. sie zwingt den Server nicht, die Updatedatei
	 *             auszuliefern, sondern lässt den Server entscheiden, ob ein
	 *             Update notwendig ist
	 *
	 * @param[in]  filename  Name der aufzurufenden Datei. Dies kann ein Skript
	 *                       sein ("ota_update.php") oder auch eine einfache
	 *                       Datei (z.B. "firmware.ino.bin")
	 */
	void update(const String& filename = "ota_update.php");
};

#endif __UPDATER__

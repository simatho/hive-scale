<?php
include("mysql_class.php");

class MySQL_Waage extends MySQL {
	/**
	 * Fügt einen Datensatz der `weightdata`-Tabelle hinzu
	 *
	 * @param      float   $weight        Gewicht, in kg
	 * @param      float   $temp_int      Temperatur des AD-Wandlers, in °C
	 * @param      float   $temp_ext      Temperatur des externen Sensors, in °C
	 * @param      String  $sensorid_ext  Sensor-ID des externen ~
	 * @param      int     $sensetime     Messzeit, Unix-Timestamp
	 * @param      long    $chipmillis    Millisekunden seit Chip-Start
	 * @param      String  $chipid        Chip-Seriennummer
	 * @param      float   $vcc           Betriebsspannung, in V
	 * @param      String  $ver           Firmwareversion des ESP
	 * @param      String  $mac           MAC-Adresse des ESP
	 *
	 * @return     bool    Erfolg
	 */
	public function addEntry($weight, $temp_int, $temp_ext, $sensorid_ext, $sensetime, $chipmillis, $chipid, $vcc, $ver, $mac){
		$query = "INSERT INTO `weightdata` (`weight`,`temp_int`,`temp_ext`, `sensorid_ext`, `sensetime`, `chipmillis`, `chipid`, `servertime`, `vcc`, `ver`) VALUES ('".
					(float)($weight)."','".(float)($temp_int)."','".(float)$temp_ext."','".$this->sqli->real_escape_string($sensorid_ext)."','".(int)$sensetime."','".
					(int)$chipmillis."','".$this->sqli->real_escape_string($chipid)."','".time()."','".(float) $vcc."', '".$this->sqli->real_escape_string($ver)."');";
		$this->addOrUpdateDevice($mac, $chipid, $ver);
		return $this->sqli->query($query);
	}

	/**
	 * Gewichtsdaten auslesen, mit Zeiteinschränkung
	 *
	 * @param      int    $time_from  Ab diesem Timestamp
	 * @param      int    $time_to    Bis zu diesem Timestamp
	 *
	 * @return     array  Die Gewichtsdaten
	 */
	public function getEntries($time_from, $time_to){
		$query = "SELECT * FROM `weightdata` WHERE `servertime` >= '".(int)$time_from."' AND `servertime` <= '".(int)$time_to."';";
		return $this->fetch_all($query);
	}
}
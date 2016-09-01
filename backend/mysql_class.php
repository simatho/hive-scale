<?php
include_once("functions.php"); // Allgemeine Funktionen einbinden
include_once("login_class.php"); // Login einbinden
// "Datenbank-Stuff" (Markus H.)
class MySQL {
	function __construct() {
		if(file_exists("mysql.php")) {
			include("mysql.php"); // Include der MySQL-Konfiguration
		} else {
			// Generische Konfiguration
			$mysqli_host = "localhost";
			$mysqli_pass = "Gehaim";
			$mysqli_user = "root";
			$mysqli_db   = "espTemp";
			$mysqli_error = false;
		}

		// Verbinde zur Datenbank
		$this->sqli = new mysqli($mysqli_host, $mysqli_user, $mysqli_pass, $mysqli_db);

		// Fehler ausgeben
		if(mysqli_connect_errno()) {
			die("Mysql-Error: ".$this->sqli->error."\n");
		}
	}

	/**
	 * User zur Datenbank hinzufügen, Passwort als gesalzenen Hash abspeichern
	 *
	 * @param      String	$username  The username
	 * @param      String	$password  The password
	 *
	 * @return     boolean	Erfolg
	 */
	public function addUser($username, $password) {
		$options = ['cost' => 11];
		$hash = password_hash($password, PASSWORD_BCRYPT, $options);

		$query = "INSERT INTO `users`(`user`, `hash`, `time`) VALUES ".
		"('".$this->sqli->real_escape_string($username)."','".$this->sqli->real_escape_string($hash)."','".time()."')";
		return $this->sqli->query($query);
	}

	/**
	 * User anhand des Benutzernamens zurückgeben, wenn kein Benutzer angegeben wird einfach alle zurückgeben
	 *
	 * @param      String	$user   Username
	 *
	 * @return     mixed	Benutzer-Informationen
	 */
	public function getUser($user = "") {
		$query = "SELECT * FROM `users`";
		if($user != "")
			$query .= " WHERE `user` = '".$this->sqli->real_escape_string($user)."';";
		else
			$query .= ";";
		$result = $this->fetch_all($query);
		if($user != "")
			return $result[0];
		return $result;
	}

	/**
	 * Fügt eine Session in die Login-Verwaltungsdatenbank ein
	 *
	 * @param      String	$user        Username
	 * @param      String	$secret      Für die Session verwendete Zufallszahl
	 * @param      String	$hash        Generierter Session-Hash
	 * @param      String	ip           IP-Adresse des Benutzers
	 * @param      int 		time_until   Zeit, bis zu der der Login gültig ist
	 *
	 * @return     boolean	Erfolg
	 */
	public function setLogin($user, $secret, $hash, $ip, $time_until) {
		if($time_until > (time() + 3600 * 24) || strlen(trim($hash)) < 59 || strlen(trim($secret)) < 2) return false;
		$query = "INSERT INTO `logins`(`user`, `secret`, `hash`, `ip`, `time_until`) VALUES ".
		"('".$this->sqli->real_escape_string($user)."','".$this->sqli->real_escape_string($secret)."','".$this->sqli->real_escape_string($hash).
		"','".$this->sqli->real_escape_string($ip)."','".(int)$time_until."')";
		return $this->sqli->query($query);
	}

	/**
	 * Holt eine Login-Session aus der Datenbank
	 *
	 * @param      String	$user   Username
	 * @param      String	$hash   Generierter Session-Hash
	 *
	 * @return     mixed	Login-Session
	 */
	public function getLogin($user, $hash) {
		$query = "SELECT * FROM `logins` WHERE `user` = '".$this->sqli->real_escape_string($user)."' AND `hash` = '".$this->sqli->real_escape_string($hash)."' AND `time_until` > '".time()."';";
		$result = $this->fetch_all($query);
		return $result[0];
	}

	/**
	 * Entfernt den angegebenen Login anhand des Session-Hashes aus der Datenbank
	 *
	 * @param      String	$hash   Generierter Session-Hash
	 *
	 * @return     boolean	Erfolg
	 */
	public function removeLogin($hash) {
		$query = "DELETE FROM `logins` WHERE `hash` = '".$this->sqli->real_escape_string($hash)."';";
		return $this->sqli->query($query);
	}

	/**
	 * Räumt abgelaufene Login-Sessions in der Datenbank auf
	 *
	 * @return     booleam	Erfolg
	 */
	public function cleanLogin() {
		$query = "DELETE FROM `logins` WHERE `time_until` < '".time()."';";
		return $this->sqli->query($query);
	}

	/**
	 * Gibt alle Geräteklassen zurück, wenn $class angegeben wird nur die angegebene.
	 *
	 * @param      int  $class  (optional) Klasse, die zurückgegeben werden soll
	 *
	 * @return     mixed   Ergebnis
	 */
	public function getDeviceClasses($class = -1) {
		$query = "SELECT * FROM `device_class` ORDER BY `id` ASC;";
		if($class != -1) {
			$query = "SELECT * FROM `device_class` WHERE `id` = '".(int)$class."';";
		}
		$result = $this->fetch_all($query);
		if($class != -1)
			return $result[0];
		return $result;
	}

	/**
	 * Aktualisiert den Namen einer Geräteklasse, fügt eine neue hinzu falls $id "new" ist.
	 *
	 * @param      integer 	$id     Identifier
	 * @param      String  	$name   Neuer Name
	 *
	 * @return     boolean	Erfolg
	 */
	public function updateOrAddDeviceClass($id, $name) {
		if(trim($name) == "") return false; // Name kann nicht leer sein
		$query = $id === "new" ? "INSERT INTO `device_class`(`name`) VALUES ('".$this->sqli->real_escape_string($name)."');":
			"UPDATE `device_class` SET `name` = '".$this->sqli->real_escape_string($name)."' WHERE `id` = '".(int)$id."';";
		return $this->sqli->query($query);
	}

	/**
	 * Gibt alle Geräte zurück, die zu einer bestimmten Klasse gehören
	 *
	 * @param      integer	$class  Klassen-ID
	 *
	 * @return     mixed	Ergebnis
	 */
	public function getDevicesByClass($class) {
		$query = "SELECT * FROM `devices` WHERE `class` = '".(int)$class."';";
		return $this->fetch_all($query);
	}

	/**
	 * Gibt die neueste Firmwareversion zurück
	 *
	 * @return     String  The latest version.
	 */
	public function getLatestVersion() {
		$versions = $this->getFirmwareInfo(); // Alle Versionen auslesen
		$latest = "0.0.0-b0000"; // Hier wird die neueste abgespeichert
		foreach($versions as $version) {
			if(updateNeeded($version["ver"], $latest))
				$latest = $version["ver"];
		}
		return $latest;
	}

	/**
	 * Fügt eine neue Firmwareversion hinzu
	 *
	 * @param      String  $filename  Filename
	 * @param      String  $filepath  Filepath
	 * @param      String  $version   Version
	 * @param      integer $class     Geräteklasse
	 *
	 * @return     boolean Erfolg
	 */
	public function addFirmwareVersion($filename, $filepath, $version, $class){
		$query = "INSERT INTO `versions`(`filename`, `ver`, `date`, `md5sum`, `sha256sum`, `class`) VALUES ".
		"('".$this->sqli->real_escape_string($filename)."','".$this->sqli->real_escape_string($version)."','".time().
		"','".hash_file("md5",$filepath.$filename)."','".hash_file("sha256",$filepath.$filename)."', '".(int)$class."')";
		return $this->sqli->query($query);
	}

	/**
	 * Gibt den in der Datenbank vorhandenen Datensatz zu einer angegebenen
	 * Firmwareversion zurück
	 *
	 * Falls kein Versionsstring angegeben wird, werden alle in der Datenbank vorhandenen Versionen zurückgegeben
	 *
	 * @param      String/int  $version  (optional) Versionsstring oder Versions-ID
	 *
	 * @return     array   The firmware information.
	 */
	public function getFirmwareInfo($version = "") {
		$query = "SELECT * FROM `versions`";
		if($version != "") {
			if(isVersionString($version)) // Versionsstring
				$query .= " WHERE `ver` = '".$this->sqli->real_escape_string($version)."';";
			else // ID
				$query .= " WHERE `id` = '".(int)$version."';";
		}
		else
			$query .= " ORDER BY `date` DESC;";
		$result = $this->fetch_all($query);
		if($version != "")
			return $result[0];
		return $result;
	}

	/**
	 * Gibt ein oder mehrere Geräte zurück
	 *
	 * @param      string  $mac    (optional) MAC-Adresse des Geräts
	 *
	 * @return     array   Eigenschaften der Gerät(e) als array
	 */
	public function getDevice($mac = "") {
		$query = "SELECT * FROM `devices`";
		if($mac != "")
			$query .= "WHERE `mac` = '".$this->sqli->real_escape_string($mac)."';";
		$result = $this->fetch_all($query);
		if($mac != "")
			return $result[0];
		return $result;
	}

	/**
	 * Aktualisiert Firmwareversion
	 *
	 * $follow_upstream: Gerät aktualisiert immer auf die neueste Version, wenn
	 * eine in die Datenbank eingetragen wird
	 *
	 * @param      int      $id               ID des Geräts in der Datenbank
	 * @param      int   	$ver_upd          Die Soll-Version
	 * @param      int      $class            Geräteklasse
	 * @param      boolean  $follow_upstream  (optional) Gerät folgt dem
	 *                                        Upstream
	 *
	 * @return     boolean  Erfolg
	 */
	public function updateDevice($id, $ver_upd, $class, $follow_upstream = 0){
		$query = "UPDATE `devices` SET `ver_upd` = '".(int)$ver_upd."', `follow_upstream` = '".(int)$follow_upstream."', `class` = '".(int)$class."' WHERE `id`='".(int)$id."';";
		return $this->sqli->query($query);
	}

	private $sqli;

	/**
	 * Fügt ein neues Gerät hinzu oder aktualisiert ein bestehendes
	 *
	 * @param      String  $mac     MAC-Adresse
	 * @param      String  $chipid  Chip-Seriennummer
	 * @param      String  $ver     Aktuelle Firmware-Version
	 *
	 * @return     boolean Erfolg
	 */
	private function addOrUpdateDevice($mac, $chipid, $ver){
		$query = "INSERT INTO `devices`(`mac`, `chipid`, `ver`, `last_seen`) VALUES ('".$this->sqli->real_escape_string(strtolower($mac))."','".$this->sqli->real_escape_string($chipid)."','".$this->sqli->real_escape_string($ver)."','".time()."') ON DUPLICATE KEY UPDATE `ver`= '".$this->sqli->real_escape_string($ver)."', `last_seen`='".time()."';";
		return $this->sqli->query($query);
	}

	/*
	** Holt alle Ergebnisse einer Query $query und gibt diese in einem Array aus
	**
	** @param      String  $query  SQL-Query
	**
	** @return     array   Datenbank-Rückgabe
	*/
	protected function fetch_all($query){
		$iResult = $this->sqli->query($query);
		$return = array();
		while ($row = $iResult->fetch_assoc()) {
			$return[] = $row;
		}
		$iResult->close();
		return $return;
	}
}
<?php
// "Datenbank-Stuff" (Markus H.)
class MySQL_Waage {
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

	public function addEntry($weight, $temp_int, $temp_ext, $sensorid_ext, $sensetime, $chipmillis, $chipid, $vcc, $ver){
		$query = "INSERT INTO `weightdata` (`weight`,`temp_int`,`temp_ext`, `sensorid_ext`, `sensetime`, `chipmillis`, `chipid`, `servertime`, `vcc`, `ver`) VALUES ('".
					(float)($weight)."','".(float)($temp_int)."','".(float)$temp_ext."','".$this->sqli->real_escape_string($sensorid_ext)."','".(int)$sensetime."','".
					(int)$chipmillis."','".$this->sqli->real_escape_string($chipid)."','".time()."','".(float) $vcc."', '".$this->sqli->real_escape_string($ver)."');";
		return $this->sqli->query($query);
	}
	public function getEntries($time_from, $time_to){
		$query = "SELECT * FROM `weightdata` WHERE `servertime` >= '".(int)$time_from."' AND `servertime` <= '".(int)$time_to."';";
		return $this->fetch_all($query);
	}
	private $sqli;

	/*
	**	Funktion: 	array fetch_all($query)
	**	Aufgabe:	Holt alle Ergebnisse einer Query $query und gibt diese in einem Array aus
	**	Version:	2.0 (04.05.2016)
	**				Thomas Pfister
	*/
	private function fetch_all($query){
		$iResult = $this->sqli->query($query);
		$return = array();
		while ($row = $iResult->fetch_assoc()) {
			$return[] = $row;
		}
		$iResult->close();
		return $return;
	}
}

function debug_out($var){
	print "<pre>".print_r($var, 1)."</pre><br />";
}
?>

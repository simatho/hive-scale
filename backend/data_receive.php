<?php
// Daten empfangen und in Datenbank schreiben
include("mysql_waage.php");
$mysql = new MySQL_Waage();

// Beispiel-URL
// http://waage.retsifp.de/data_receive.php?weight=31.000&temp_int=21.0&temp_ext=25.7&chipmillis=5100&chipid=1&vcc=12.5&sensetime=0
// Gewicht: 31.000kg, A/D-Temp: 21 °C, Externe Temperatur (optional): 25.7 °C, Chipmillis: 5100 ms, Chipid: 1, Versorgungsspannung: 12,5V, Messzeit: 01.01.1970 (= Unix-Timestamp)

if(trim($_GET["weight"]) == "" || trim($_GET["temp_int"]) == "" || trim($_GET["chipmillis"]) == "" || trim($_GET["chipid"]) == "") {
	die("FEHLER, zu wenige Daten!");
}

if ($mysql->addEntry($_GET["weight"], $_GET["temp_int"], $_GET["temp_ext"], $_GET["sid_ext"], $_GET["sensetime"], $_GET["chipmillis"], dectohex($_GET["chipid"], 32), $_GET["vcc"], $_GET["ver"])) {
	if($_GET["dbg"] != 1)
		die("Erfolg");
	print "Daten erfolgreich gespeichert!";
} else {
	print "FEHLER, Daten konnten nicht gespeichert werden";
}

if($_GET["dbg"] == 1)
	debug_out($_GET);

function dectohex($input, $bits){
	$output = dechex($input);
	while (strlen($output) < $bits/4){
		$output = "0".$output;
	}
	return $output;
}
?>

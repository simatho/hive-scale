<?php

/**
 * Erzeugt gut lesbare Debug-Ausgaben von Arrays
 *
 * @param      any  $var    Die zu debuggende Variable
 */
function debug_out($var){
	print "<pre>".print_r($var, 1)."</pre><br />";
}

/**
 * HTML-Weiterleitung an angegebene Adresse
 *
 * @param      String	$url    Ziel-URL (Gleiche Seite, falls "")
 * @param      int 		$time   Zeitverzögerung (Standard: 0 Sekunden)
 */
function forw($url, $time = 0){
	if($url == "")
		$url = "?".$_SERVER["QUERY_STRING"];
	print '<meta http-equiv="refresh" content="'.(int)$time.'; url='.$url.'">';
}

/**
 * Prüft, ob der Client ein Update benötigt
 *
 * @param      String   $serverVersionString  The server version string
 * @param      String   $clientVersionString  The client version string
 *
 * @return     boolean  true, falls der Client updaten muss
 */
function updateNeeded($serverVersionString, $clientVersionString) {
    // Der Versionsstring ist in der Form "1.2.3-b4567"
    // "1.2.3" ist Hauptversion, "b4567" ist Build-Nummer, diese kann von Zeit zu Zeit
    // rückgesetzt werden, sofern die Hauptversion hochgezählt wird.
	if(!isVersionString($clientVersionString) || !isVersionString($serverVersionString))
		return false;
    $serverV = explode(".", $serverVersionString); // Jetzt als array(0 => "1", 1 => "2", 2 => "3-b4567")
    $clientV = explode(".", $clientVersionString);
    if($serverV[1] > $clientV[1] || $serverV[0] > $clientV[0])
        return true; // Server hat in den ersten zwei Ziffern eine größere Version -> updaten
    $serverV2 = explode("-b", $serverV[2]); // Jetzt: array(0 => "3", 1 => "4567")
    $clientV2 = explode("-b", $clientV[2]);
    if($serverV2[0] > $clientV2[0] || ($serverV2[0] == $clientV2[0] && $serverV2[1] > $clientV2[1]))
        return true; // Der Server hat eine neuere Version
    return false; // Sonst nicht updaten
}

/**
 * Stellt fest, ob der mitgegebene String ein Versions-String ist
 *
 * @param      String   $versionString  The version string
 *
 * @return     boolean  True if version string, False otherwise.
 */
function isVersionString($versionString){
	return preg_match("(([0-9]\.){2}[0-9]\-b[0-9]{4})", $versionString) === 1;
}

/**
 * Gibt einen String so zurück, dass er im Browser mit der entsprechenden Farbe
 * erscheint
 *
 * @param      String  $string  The string
 * @param      String  $color   The color
 */
function colorString($string, $color) {
	return '<p style="color:'.htmlentities($color).';">'.$string.'</p>';
}
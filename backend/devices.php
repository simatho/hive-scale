<?php
include("mysql_waage.php");
include_once("login_class.php");
ob_start(); // Ausgabenpufferung (Notwendig für Setzen der Login-Cookies)
$mysql = new MySQL_Waage();
$login = new Login($mysql);

// Erreichbare Funktionen
$links = array("<a href='?s=list_devices'>Geräte auflisten</a>",
	"<a href='?s=new_firmware'>Neue Firmwareversion hochladen</a>",
	"<a href='?s=device_class'>Geräteklassen verwalten</a>",
	"<a href='?s=logout'>Ausloggen</a>");

// Maximale Dateigröße. Da der verwendete ESP-07 nur 1 MiB Flash besitzt, ist
// mehr nicht sinnvoll.
$maxSize = 1 * 1024 * 1024;
// SHA1-Summe des verwendeten Zertifikats (Leer lassen, wenn die Prüfung entfallen soll)
$crt_sha1sum = "3A D2 DA C2 23 CB 31 D1 8C B7 F1 6B 56 F3 C4 12 7B 0D 2E e2";
print '<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>Geräteverwaltung</title>
    <link href="style.css" rel="stylesheet" type="text/css" />
    <!-- Benötigt, da sonst die "selected"-Werte vom Browser nicht aktualisiert werden -->
	<meta http-equiv="cache-control" content="no-cache">
  </head><body>';

print '<div id="wrapper">';
$login->checkOrPrintLogin();
print'<div class="post2">';

switch($_GET["s"]){
	case "new_firmware":
		// Eine neue Firmwareversion übermitteln
		$classes = $mysql->getDeviceClasses();
		$class_dropdown = '<select name="class">';
		foreach($classes as $class){
			$class_dropdown .= '<option value="'.$class["id"].'">'.$class["name"].'</option>';
		}
		$class_dropdown .= "</select>";
		print "<div class='title'>Neue Firmwareversion</div>";
		print '<form enctype="multipart/form-data" action="?s=upload_firmware" method="POST">'.
		'<input type="hidden" name="MAX_FILE_SIZE" value="'.$maxSize.'" />'.
		'Neue Firmwareversion (<i>*.bin</i>): <input name="firmwarefile" type="file" /><br />'.
		'Versionsnummer (leer für automatische Erkennung): <input name="version" type="text" length="15" /><br />'.
		'Zugehörig zur Klasse: '.$class_dropdown."<br />".
		'<input type="submit" value="Senden" /></form>';
	break;

	case "upload_firmware":
		$warning = 0; // Warnungen werden im Folgenden aufsummiert
		$uploaddir = dirname($_SERVER["SCRIPT_FILENAME"]).'/files/';
		if($_FILES['firmwarefile']['error'] !== 0) {
			die(colorString("Es ist ein Fehler beim Upload aufgetreten.", "red"));
		}
		if($_FILES['firmwarefile']['size'] > $maxSize) {
			die(colorString("Die hochgeladene Datei ist zu groß! (".($_FILES['firmwarefile']['size']/1024)." kByte, maximale Größe ist ".($maxSize/1024)." kByte)", "red"));
		}

		// Dateinamen auftrennen "firmware.ino.bin" => array(0 => "firmware", 1 =>
		// "ino", 2 => bin)
		$ending = explode(".", $_FILES['firmwarefile']['name']);
		if($ending[count($ending) - 1] != "bin") {
			die(colorString("FEHLER: Die hochgeladene Datei ist keine *.bin-Datei (stattdessen *.".$ending[count($ending) - 1]."). Dies bedeutet normalerweise, dass eine falsche Datei ausgewählt wurde!<br />", "red"));
			$warning++;
		}
		// Firmware-File einlesen (zur Sicherheit maxSize festlegen, nicht dass der Arbeitsspeicher vollläuft...)
		$contents = file_get_contents($_FILES['firmwarefile']['tmp_name'], false, NULL, -1, $maxSize);
		// Leeres Rückgabe-Array anlegen
		$matches = array();
		// Nach Firmware-Versions-String in der Datei suchen
		$pregResult = preg_match("(([0-9]\.){2}[0-9]\-b[0-9]{4})", $contents, $matches);
		if($pregResult == 1 && count($matches) >= 1) {
			// TODO: Für den Fall count($matches) > 0 könnte erkannt werden, warum das so ist
			// Es wurde ein Versions-String in der Binärdatei gefunden
			$recognizedVersion = $matches[0];
			if(trim($_POST["version"]) == "" || trim($_POST["version"]) == $recognizedVersion) {
				// Alles paletti.
				print "Erkannte Version: ".$recognizedVersion."<br />";
			} else {
				// User hat eine Version angegeben, diese stimmt nicht mit der erkannten überein.
				print colorString("WARNUNG: Erkannte Version: ".$recognizedVersion.", angegeben wurde aber ".htmlentities($_POST["version"])."<br />", "orange");
				$warning++;
			}
		} else {
			die(colorString("FEHLER: Im Firmware-File wurde keine gültige Versionsnummer gefunden!<br />", "red"));
		}

		$matches = array();
		// TODO: Bessere regexp, diese liefert zwei Ergebnisse...
		$pregResult = preg_match("/[a-f0-9]{2}((\s|:)[a-f0-9]{2}){19}/i", $contents, $matches);

		foreach ($matches as $key => $value) {
			print "SHA1-Hash eines Zertifikats gefunden: <i>".$value."</i><br />";
		}
		// Falls der User Zertifikats-Prüfung eingeschaltet hat
		if($crt_sha1sum != "") {
			if(count($matches) == 0) {
				// Zertifikat konfiguriert, aber keins gefunden
				print colorString("WARNUNG: Es wurde keine Zertifikat-Checksumme gefunden! Fahren Sie nur fort, wenn Sie wirklich wissen, was Sie tun!<br />", "red");
				$warning++;
			} else if(count($matches) >= 1 && strtolower($matches[0]) === strtolower($crt_sha1sum)){
				print colorString("Diese stimmt mit dem konfigurierten Hash überein.<br />", "green");
			} else {
				print colorString("WARNUNG: Konfiguriert wurde <i>".$crt_sha1sum."</i>! Fahren Sie nur fort, wenn Sie wirklich wissen, was Sie tun!<br />", "red");
				$warning++;
			}
		}

		$filename = "firmware-".$recognizedVersion.".bin";
		if(file_exists($uploaddir.$filename)){
			die(colorString("FEHLER: Diese Datei (".$filename.") existiert bereits auf dem Server!", "red"));
		}
		if (move_uploaded_file($_FILES['firmwarefile']['tmp_name'], $uploaddir.$filename)) {
			// TODO: Bei Fehlern dem Benutzer ermöglichen, diesen Fehler auszubügeln
			print colorString("Datei ist valide und wurde erfolgreich hochgeladen.<br />", "green");
			chmod($uploaddir.$filename, 0444); // Hochgeladene Datei kann nur noch gelesen werden
			if($mysql->addFirmwareVersion($filename, $uploaddir, $recognizedVersion, $_POST["class"])){
			print colorString("Version erfolgreich in die Datenbank übertragen.<br />", "green");
			}
		} else {
			die(colorString("FEHLER: Upload der Datei fehlgeschlagen", "red"));
		}
	break;

	case "list_devices":
		print "<div class='title'>Geräteverwaltung - alle Geräte auflisten</div>";
		if(count($_POST) > 0){
			foreach($_POST as $key => $el){
				if(preg_match("/^\d+$/", $key) === 1)
					$device = $key;
			}
			if($device > 0 &&
				preg_match("/^\d+$/", $_POST["firmware"]) === 1 &&
				preg_match("/^\d+$/", $_POST["class"]) === 1){
				if($mysql->updateDevice($device, $_POST["firmware"], $_POST["class"]))
					print "<div class='date'>Erfolgreich gespeichert</div>";
			}
		}

		print "<br /><style>.datagrid table { border-collapse: collapse; text-align: left; width: 100%; } .datagrid {font: normal 12px/150% Century Gothic, Quattrocento Sans, Verdana, Arial, Helvetica, sans-serif; background: #fff; overflow: hidden; border: 1px solid #8C8C8C; -webkit-border-radius: 3px; -moz-border-radius: 3px; border-radius: 3px; width: 100%;}.datagrid table td, .datagrid table th { padding: 3px 10px; }.datagrid table thead th {background:-webkit-gradient( linear, left top, left bottom, color-stop(0.05, #8C8C8C), color-stop(1, #7D7D7D) );background:-moz-linear-gradient( center top, #8C8C8C 5%, #7D7D7D 100% );filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#8C8C8C', endColorstr='#7D7D7D');background-color:#8C8C8C; color:#FFFFFF; font-size: 15px; font-weight: bold; border-left: 1px solid #A3A3A3; } .datagrid table thead th:first-child { border: none; }.datagrid table tbody td { color: #7D7D7D; border-left: 1px solid #DBDBDB;font-size: 12px;font-weight: normal; }.datagrid table tbody .alt td { background: #EBEBEB; color: #7D7D7D; }.datagrid table tbody td:first-child { border-left: none; }.datagrid table tbody tr:last-child td { border-bottom: none; }.datagrid table tfoot td div { border-top: 1px solid #8C8C8C;background: #EBEBEB;} .datagrid table tfoot td { padding: 0; font-size: 12px } .datagrid table tfoot td div{ padding: 2px; }.datagrid table tfoot td ul { margin: 0; padding:0; list-style: none; text-align: right; }.datagrid table tfoot  li { display: inline; }.datagrid table tfoot li a { text-decoration: none; display: inline-block;  padding: 2px 8px; margin: 1px;color: #F5F5F5;border: 1px solid #8C8C8C;-webkit-border-radius: 3px; -moz-border-radius: 3px; border-radius: 3px; background:-webkit-gradient( linear, left top, left bottom, color-stop(0.05, #8C8C8C), color-stop(1, #7D7D7D) );background:-moz-linear-gradient( center top, #8C8C8C 5%, #7D7D7D 100% );filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#8C8C8C', endColorstr='#7D7D7D');background-color:#8C8C8C; }.datagrid table tfoot ul.active, .datagrid table tfoot ul a:hover { text-decoration: none;border-color: #7D7D7D; color: #F5F5F5; background: none; background-color:#8C8C8C;}div.dhtmlx_window_active, div.dhx_modal_cover_dv { position: fixed !important; }</style>";
		$devices = $mysql->getDevice(); // Alle Geräte aus der Datenbank holen

		$classes = $mysql->getDeviceClasses();
		$class_dropdown = '<select name="class"><option value="0"></option>';
		foreach($classes as $class){
			$class_dropdown .= '<option value="'.$class["id"].'">'.$class["name"].'</option>';
		}
		$class_dropdown .= "</select>";

		$firmwares = $mysql->getFirmwareInfo(); // Alle Firmware-Versionen holen
		$firmware_dropdown = '<select name="firmware"><option value="0"></option>';
		foreach($firmwares as $firm){
			$firmware_dropdown .= '<option style="/*'.$firm["class"].'*/" value="'.$firm["id"].'">'.$firm["ver"];
			$class = $mysql->getDeviceClasses($firm["class"]);
			if(array_key_exists("name", $class))
				$firmware_dropdown .= "(".$class["name"].")";
			$firmware_dropdown .= '</option>';
		}
		$firmware_dropdown .= "</select>";

		print "<div class='datagrid'><table><thead><tr><td>ID</td><td>MAC</td><td>Seriennummer</td><td>Firmware IST</td><td>Firmware SOLL</td><td>Zuletzt online (UTC)</td><td>Geräteklasse</td><td></td></tr></thead>";
		print "<tfoot><tr><td></td><td></td><td></td><td></td><td></td><td></td></tr></tfoot>";
		foreach($devices as $key => $line){
			$needles = array('"'.$line["ver_upd"].'">', '/*'.$line["class"].'*/');
			$replace = array('"'.$line["ver_upd"].'" selected>', 'background-color: green;');
			$dropdown = str_replace($needles, $replace, $firmware_dropdown);

			$needles = array('"'.$line["class"].'">');
			$replace = array('"'.$line["class"].'" selected>');
			$dropdown2 = str_replace($needles, $replace, $class_dropdown);

			$tr = $key % 2 == 1 ? '<tr class="alt">': '<tr>';
			print "<form method='POST'>";
			print $tr."<td>".$line["id"]."</td><td>".$line["mac"]."</td><td>".$line["chipid"]."</td><td>".$line["ver"]."</td><td>".
				$dropdown."</td><td>".date("d.m.Y, H:i:s", $line["last_seen"])."</td><td>".$dropdown2."</td><td><input name='".$line["id"]."' type='submit' value='Abspeichern' /></td></form></tr>";
		}
		print "</table></div>";
		if($_GET["dbg"] == 1){
			debug_out($mysql->getDevice());
			debug_out($mysql->getFirmwareInfo());
			debug_out($_POST);
		}
	break;

	case "logout":
		$login->checkLogin(true);
		print "Erfolgreich ausgeloggt!";
		forw("?", 1);
	break;

	case "device_class":
		// Geräteklassen anlegen, verändern und abspeichern
		$classes = $mysql->getDeviceClasses();
		print "<div class='title'>Geräteklassen verwalten</div>";
		print "<div class='date'>Geräteklassen teilen sich eine gleiche Firmware, so kann gut bestimmt werden, welche Geräte eine neu hochgeladene Firmware erhalten sollen</div>";
		print '<form action="?s=update_device_class" method="POST">';
		print '<table><thead><tr><td>ID</td><td>Bezeichner</td><td>Anzahl Geräte</td></tr></thead>';
		foreach ($classes as $value) {
			print '<tr><td>'.$value["id"].'</td><td><input name="'.$value["id"].'" type="text" length="15" value="'.$value["name"].'"/></td><td></td></tr>';
		}
		print '<tr><td>Neue hinzufügen:</td><td><input name="new" type="text" length="15" /></td><td></td></tr>';
		print '<tr><td>&nbsp;</td><td><input type="submit" value="Senden" /></td><td></td></tr>';
		print "</table>";
	break;

	case "update_device_class":
		foreach ($_POST as $key => $value) {
			if(trim($value) == "") continue;
			print "UPDATE '".$value."': ".(boolean)$mysql->updateOrAddDeviceClass($key, $value);
			print "<br />";
		}
		forw("?s=device_class", 1);
	break;

	default:
		print "<div class='title'>Geräteverwaltung</div>
				<ul>";
		foreach ($links as $value) {
			print "<li>".$value."</li>";
		}
		print "</ul>";
}
print "</div>"; // post2
print "<div id='copy'><a href='?'>Übersicht</a>";
foreach ($links as $value) {
	print " | ".$value;
}
print "</div>";
print "</div>"; // wrapper
print '</body></html>';
ob_end_flush();
<?php
include("mysql_waage.php");
$mysql = new MySQL_Waage();
if($_GET["dbg"] == 1) $debug = true;
else $debug = false;
$date["from"] = time() - 7*24*60*60*3;
$date["to"] = time();
if(isset($_POST["reset"])) {
	setcookie("fDate", "", 0, "/");
	setcookie("tDate", "", 0, "/");
} elseif($_POST["submit"] == "Einstellen" && $_POST["fDate"] != "" && $_POST["tDate"] != ""){
	// Daten wurden abgeschickt
	// Setze 12h lang gültige Cookies über die Einstellungen
	$date["from"] = strtotime($_POST["fDate"]);
	$date["to"] = strtotime($_POST["tDate"]) + 24*3600 - 1;
	if($debug)
		debug_out($date);
	setcookie("fDate", $date["from"], time() + 12*3600, "/");
	setcookie("tDate", $date["to"], time() + 12*3600, "/");
} elseif(isset($_COOKIE["fDate"])) {
	$date["from"] = $_COOKIE["fDate"];
	$date["to"] = $_COOKIE["tDate"];
}
print '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="de">
  <head>
    <title>Datenaufbereitung</title>
	<link href="js/nv.d3.css" rel="stylesheet" type="text/css">
    <script src="js/d3.min.js" charset="utf-8"></script>
    <script src="js/nv.d3.js"></script>
	<style>
        text {
            font: 12px sans-serif;
        }
        svg {
            display: block;
            overflow: visible;
        }
        html, body, #chart, svg {
            margin: 0px;
            padding: 0px;
            height: 100%;
            width: 100%;
        }
    </style>
  </head>
  <body style="font-family: Quattrocento Sans, Century Gothic, Verdana, sans-serif; background: #E6E6E6;">';
print '<form method="POST">Daten Anzeigen von:&nbsp;&nbsp;<input type="date" name="fDate" value="'.date("Y-m-d", $date["from"]).'" />&nbsp;&nbsp;bis:&nbsp;&nbsp;<input type="date" name="tDate" value="'.date("Y-m-d", $date["to"]).'" />&nbsp;<input type="submit" name="submit" value="Einstellen" />&nbsp;<input type="submit" name="reset" value="Zur&uuml;cksetzen" /></form>';
$data = $mysql->getEntries($date["from"], $date["to"]);

if($debug)
	debug_out($data);
$maxY = 0.0; // Maximale Y-Koordinate
$index = 0; // x-Koordinate für Gewicht
//$values = array_map("jsonify", $data);
$values = null; // Ausgabe-Array anlegen
foreach($data as $array) {
	// Alle Elemente des Arrays aus der Datenbank durchgehen

	// Für Skalierung: Maximales Gewicht feststellen
	$maxY = $array["weight"] > $maxY ? $array["weight"] : $maxY;
	$maxY = $array["temp_int"] > $maxY ? $array["temp_int"] : $maxY;
	$maxY = $array["temp_ext"] > $maxY ? $array["temp_ext"] : $maxY;
	// Gewicht und Temperatur jew. als y-Koordinate speichern
	$values[0][$index] = array("x" => (int)$array["servertime"]*1000, "y" => $array["weight"], "unit" => "kg");
	$values[1][$index] = array("x" => (int)$array["servertime"]*1000, "y" => $array["temp_int"], "unit" => "°C");
	$values[2][$index] = array("x" => (int)$array["servertime"]*1000, "y" => $array["temp_ext"], "unit" => "°C");
	// Index hochzählen
	$index++;
}
// Datensatz-Arrays anlegen (Neue Datensätze hier einfügen, alles Weitere wird automatisch erledigt)
$dataArray[0] = array("key"=> "Gewicht [kg]", "area" => false, "values" => $values[0]);
$dataArray[1] = array("key"=> "Temp. intern [°C]", "area" => false, "values" => $values[1]);
$dataArray[2] = array("key"=> "Temp. extern [°C]", "area" => false, "values" => $values[2]);
// Alle Elemente prüfen, ob Daten vorhanden sind, sonst Datensatz nicht anzeigen (Array-Element löschen)
for($foo = 0;$foo < count($dataArray[0]);$foo++){
	if(count($values[$foo]) == 0) {
		unset($dataArray[$foo]);
	}
}
if($debug)
	debug_out($dataArray);
// PHP-Array in JSON umwandeln
$json = json_encode($dataArray, JSON_PRETTY_PRINT);
if($debug)
	debug_out($json);


?>
<div id="chart" style="height:80%" class='with-3d-shadow with-transitions'>
    <svg></svg>
</div>

<script>
nv.addGraph(function () {
	<?php
	// JSON-Daten an JavaScript verfüttern
	print "var data = ".$json.";\n";
	//print "console.log(JSON.stringify(data2));";
	?>
    var chart = nv.models.lineChart()
		.useInteractiveGuideline(true)
		.showLegend(true)
		.forceY([0,<?php print $maxY; ?>]);


    chart.xAxis.axisLabel("Zeit").rotateLabels(-45).tickFormat(function (d) {
        return d3.time.format("%d.%m.%y %H:%M:%S Uhr")(new Date(d))
    });
    chart.yAxis.tickFormat(d3.format(',.2f'));
    d3.select("#chart svg")
        .datum(data)
        .call(chart);

    nv.utils.windowResize(function () {
        d3.select("#chart svg").call(chart)
    });

    return chart;
});

</script>
</body>
</html>

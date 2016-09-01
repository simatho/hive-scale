<?php
/**
 * Angepasstes Beispiel von hier:
 * https://github.com/esp8266/Arduino/blob/master/doc/ota_updates/readme.md
 */
include("mysql_waage.php");
$mysql = new MySQL_Waage();

header('Content-type: text/plain; charset=utf8', true);

function checkHeader($name, $value = false) {
    if(!isset($_SERVER[$name])) {
        return false;
    }
    if($value && $_SERVER[$name] != $value) {
        return false;
    }
    return true;
}

function sendFile($path, $md5sum) {
    header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
    header('Content-Type: application/octet-stream', true);
    header('Content-Disposition: attachment; filename='.basename($path));
    header('Content-Length: '.filesize($path), true);
    header('x-MD5: '.$md5sum, true);
    readfile($path);
}
// TODO: remove this!!!
if($_GET["dbg"] == 1) goto debug;

if(!checkHeader('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    die("only for ESP8266 updater!");
}

if(
    !checkHeader('HTTP_X_ESP8266_STA_MAC') ||
    !checkHeader('HTTP_X_ESP8266_AP_MAC') ||
    !checkHeader('HTTP_X_ESP8266_FREE_SPACE') ||
    !checkHeader('HTTP_X_ESP8266_SKETCH_SIZE') ||
    !checkHeader('HTTP_X_ESP8266_CHIP_SIZE') ||
    !checkHeader('HTTP_X_ESP8266_SDK_VERSION') ||
    !checkHeader('HTTP_X_ESP8266_VERSION')
) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    die("only for ESP8266 updater! (header)\n");
}

debug:
$device = $mysql->getDevice($_SERVER['HTTP_X_ESP8266_STA_MAC']);
$firmware_upd = $mysql->getFirmwareInfo($device["ver_upd"]);
if($_GET["force"] == "1" || (updateNeeded($mysql->getLatestVersion(), $_SERVER["HTTP_X_ESP8266_VERSION"]) && $device["follow_upstream"] == 1)) {
    $versionInfo = $mysql->getFirmwareInfo($mysql->getLatestVersion());
    sendFile("./files/".$versionInfo["filename"], $versionInfo["md5sum"]);
} else if(trim($device["ver_upd"]) != "0" && $firmware_upd["ver"] != $_SERVER["HTTP_X_ESP8266_VERSION"] && isVersionString($_SERVER["HTTP_X_ESP8266_VERSION"])) {
    $versionInfo = $mysql->getFirmwareInfo($device["ver_upd"]);
    sendFile("./files/".$versionInfo["filename"], $versionInfo["md5sum"]);
} else {
    header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
}
?>
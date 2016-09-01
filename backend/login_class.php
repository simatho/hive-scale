<?php
include_once("mysql_class.php");
class Login {

	/**
	 * Konstruktor, setzt private Variablen und räumt die Datenbank auf.
	 *
	 * @param      MySQL  $mysql_conn  The mysql connection
	 */
	function __construct($mysql_conn) {
		$this->mysql = $mysql_conn;
		$this->mysql->cleanLogin(); // Veraltete Login-Einträge aus Datenbank löschen
		$loggedIn = false;
	}

	/**
	 * Handelt den Login-Vorgang auf Nutzerseite und zeigt bei Bedarf das Login-Formular an.
	 */
	public function printLogin() {
		// Login-Formular ausgeben
		$this->checkHTTPS();
		print '<div class="post2">';
		if($_POST["submit"] != "Login" && !$this->checkLogin()) {
			print "<div class=\"title\">Login</div>";
			if(count($this->mysql->getUser()) === 0) // Wenn kein User in der Datenbank ist
				print '<div class="date">Kein Benutzer angelegt. Login-Daten zum Anlegen eingeben.</div>';
			print "<form method=\"POST\">";
			print "<table cellspacing =\"5px\">";
			print "<tr height=\"40px\"><td valign=\"center\">Username: </td><td><input type=\"text\" name=\"username\" size=\"30\" /></td></tr>";
			print "<tr height=\"40px\"><td valign=\"center\">Password: </td><td><input type=\"password\" name=\"password\" size=\"30\" /></td></tr>";
			print "<tr><td>&nbsp;</td><td><input type=\"submit\" value=\"Login\" name=\"submit\" /></td></tr>";
			print "</table>";
		} elseif (!$this->checkLogin() && count($this->mysql->getUser()) === 0) {
			$this->mysql->addUser(trim($_POST['username']), $_POST['password']);
			print "<div class=\"title\">Benutzer angelegt, jetzt bitte einloggen.</div><br />";
			forw("", 1);
		} elseif (!$this->checkLogin() && $this->checkPass(trim($_POST['username']), $_POST['password'])) {
			print "<div class=\"title\">Erfolgreich eingeloggt</div><br />";
			forw("", 1);
		} elseif($this->checkLogin()) print "<div class=\"title\">Bereits eingeloggt.</div><br />".forw("", 1);
		else print "<div class=\"title\">Beim Login ist leider ein Fehler aufgetreten.</div><br />".forw("", 2);
		print "</div>";
	}

	/**
	 * Prüft, ob Benutzer per HTTPS (oder lokal) verbunden ist und bricht evtl. mit Fehlermeldung ab.
	 */
	public function checkHTTPS() {
		if((!isset($_SERVER['HTTPS']) || $_SERVER['HTTPS'] == 'off') &&
			!($this->isLocalhost())) {
			// Nicht per HTTPS verbunden und auch nicht lokal verbunden
			die("FEHLER: Sie sind nicht über eine sichere Verbindung verbunden!");
		}
	}

	/**
	 * Prüft eingegebene Benutzerdaten und loggt bei Korrektheit ein, räumt alles auf wenn Daten falsch waren
	 *
	 * @param      String  $user   The user
	 * @param      String  $pass   The pass
	 *
	 * @return     boolean Erfolg
	 */
	public function checkPass($user, $pass){
		$this->checkHTTPS();
		// Prüft, ob der User korrekte Daten verwendet hat
		$user = $this->mysql->getUser($user);
		if(password_verify($pass, $user["hash"])){
			// Erfolg!
			$this->loggedIn = true;
			if(version_compare(phpversion(), '7.0.0', '>=')) {
				$secret = random_int(0, PHP_INT_MAX); // Gute Zufallszahlen für PHP >= 7.0
			} else {
				$secret = rand(); // Pseudo-Zufallszahlen für PHP < 7.0
			}
			$until = time() + 3600; // Gültig für eine Stunde
			$hash = hash("sha256", $secret.$user["user"].$until.$_SERVER["REMOTE_ADDR"]);
			$secureCookie = !$this->isLocalhost(); // Bei lokalem Zugriff geht HTTPS meist nicht.
			setcookie("login_user", $user["user"], $until, "/", $_SERVER["SERVER_NAME"], $secureCookie, true);
			setcookie("login_hash", $hash, $until, "/", $_SERVER["SERVER_NAME"], $secureCookie, true);
			$this->mysql->setLogin($user["user"], $secret, $hash, $_SERVER["REMOTE_ADDR"], $until);
		} else {
			$this->loggedIn = false;
		}
		return $this->loggedIn;
	}

	/**
	 * Prüft. ob Nutzer (anhand Cookies) korrekt eingeloggt ist.
	 *
	 * @param      boolean  $logout  True, falls Nutzer ausgeloggt werden will
	 *
	 * @return     boolean  Erfolg
	 */
	public function checkLogin($logout = false) {
		if($this->loggedIn && !$logout) return true;
		$dbEntry = $this->mysql->getLogin($_COOKIE["login_user"], $_COOKIE["login_hash"]);
		if(	$_COOKIE["login_hash"] === $dbEntry["hash"] &&
			$dbEntry["hash"] === hash("sha256", $dbEntry["secret"].$dbEntry["user"].$dbEntry["time_until"].$_SERVER["REMOTE_ADDR"]) &&
			$dbEntry["time_until"] > time() &&
			!$logout) {
			$this->loggedIn = true;
		} else {
			$this->loggedIn = false;
			$this->mysql->removeLogin($_COOKIE["login_hash"]);
			setcookie("login_user", "", time(), "/", $_SERVER["SERVER_NAME"]);
			setcookie("login_hash", "", time(), "/", $_SERVER["SERVER_NAME"]);
		}
		return $this->loggedIn;
	}

	/**
	 * Funktion, die am Anfang eines geschützten Skriptes stehen kann.
	 * Falls der Nutzer nicht eingeloggt ist, wird ein Formular ausgegeben und die weitere Bearbeitung des Skriptes verhindert,
	 * falls er korrekt eingeloggt ist kann das Skript weiterlaufen.
	 *
	 * @return     boolean	Erfolg (Nur bei Erfolg, bei Misserfolg wird das PHP-Skript beendet.)
	 */
	public function checkOrPrintLogin() {
		if(!$this->checkLogin()) {
			$this->printLogin();
			die();
		}
		return true;
	}

	/**
	 * Prüft, ob Benutzer lokal verbunden ist
	 *
	 * @return     boolean  True if localhost, False otherwise.
	 */
	private function isLocalhost() {
		return $_SERVER["SERVER_NAME"] === "localhost" || $_SERVER["REMOTE_ADDR"] === "127.0.0.1" || $_SERVER["REMOTE_ADDR"] === "::1";

	}

	private $loggedIn;
	private $mysql;
}
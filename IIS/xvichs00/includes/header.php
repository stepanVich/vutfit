<?php header("Content-Type: text/html; charset=UTF-8");?>
<?php require('db-init.php'); ?>
<?php // Databaze dostupna pres promennou db ?>
<?php
	function destroySession() {
		unset($_SESSION["row"]);
		unset($_SESSION["isLogin"]);	
		// Unset sessionu
		session_unset();
		session_destroy();
	}
	session_save_path("tmp");
	// Nastaveni session
	if (!isset($_SESSION)) 
	session_start();

	// Nastaveni automatickeho odhlaseni
	if (isset($_SESSION['last_activity']) and isset($_SESSION["isLogin"])) {
		if ((time() - $_SESSION['last_activity']) > 5*60) { 
			destroySession();
			header("Location: index.php"); 
			exit();
		}
	}
	$_SESSION['last_activity'] = time();

	// Nastaveni loginu pres formular
	if( isset($_POST["login"]) and isset($_POST["password"]) ) { 

		// Osetreni vstupnich hodnot
		// Ochrana proti SQL injekci
		$login =    filter_var($_POST["login"],    FILTER_SANITIZE_STRING);
		$password = filter_var($_POST["password"], FILTER_SANITIZE_STRING);

		// Nacteni dat z databaze
		$query1="SELECT * FROM pracovnik_banky WHERE login='".$login."' AND heslo='".$password."'";
		$result1 = @mysql_query($query1, $db); 
		$prihlasen = false;
		while ($data = mysql_fetch_array($result1, MYSQL_ASSOC)) {
			// prihlaseni probehlo uspesne
			$prihlasen = true;
			// nastaveni session
			$_SESSION["row"] = $data;
			$_SESSION["role"]=( $data["pozicia"] ? "admin" : "pracovnik");
			$_SESSION["isLogin"] = true;
			header("Location: inter1.php");
			exit();
		}
		

		if($prihlasen == false) {
			// Prihlaseni probehlo neuspesne
			// Vrat se na login stranku a oznac ji jako neuspesnou
			$_SESSION["loginFailed"] = true;
			header("Location: login.php");
			exit();
		}
	}

	// Presmerovani na stranku v pripade ze se jedna o prihlaseneho clena 
	if(isset($_SESSION["isLogin"])) {
		if($_SESSION["isLogin"] == true) {
			if($isInternPage == false) {
				// Opravneny pristup do systemu
				header("Location: inter1.php");
				exit();
			}
		}
	}
		
	if($isInternPage) {
		if(!isset($_SESSION["isLogin"])) {
			// Neopravneny pristup do systemu
			header("Location: index.php");
			exit();
		}
	}
?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8">
		<link rel="shortcut icon" href="favicon.ico" type="image/x-icon">
		<link rel="icon" href="favicon.ico" type="image/x-icon">	
		<link rel="stylesheet" type="text/css" href="css/main.css">
		<title>Bankovní informační systém<?php if(isset($title)) echo " - ".$title ?></title>
	</head>
	
	<body>
		<div class="main">
			<div class="menu">
				<a href="index.php" id="main-link"><h1><span id="dollars">&dollar;&dollar;</span>Bankovní informační systém</h1></a>
			</div>
			<div class="clear">
	

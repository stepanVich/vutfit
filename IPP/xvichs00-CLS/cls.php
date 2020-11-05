<?php
	/**
	 * @brief Hlavni modul skriptu cls.php do projektu IPP 2016/2017.
	 * @author Stepan Vich - xvichs00
	 * @date 17. March 2017
	 */

	// Nahrani potrebnych modulu
	require 'LexAnalyzator.php';
	require 'SyntaxAnalyzator.php';
	require 'View.php';
	require 'Model.php';

	// zpracovani argumentu
	// vyuziva knihovnu getopt
	$shortopts = "";
	$shortopts .= "h:";
	$shortopts .= "i:";
	$shortopts .= "o:";
	$shortopts .= "p:";
	$shortopts .= "d:";
	$longopts = array(
		"help",
		"input:",
		"output:",
		"pretty-xml:",
		"details::"
	);
	$options = getopt($shortopts, $longopts);
	// Kontrola zadani neznamych parametru
	if(count($options) != count($argv) - 1) {
		exit(1);
	}

	// Podrobne zpracovani vstupnich promenych pro bezchybne fungovani programu
	// Zpracovani help parametru
	if(array_key_exists("help", $options) || array_key_exists("h", $options)) {
		// Kontrola ze neni nastaven zadny dalsi parametr
		if(count($options) !== 1) {
			exit(1);
		}
		// Kontrola ze atribut help, nema zadny parametr
		// Vypsani napovedy
		printHelp();		
	}
	
	// Zpracovani pretty-xml parametru
	// Kontrola ze zadana hodnota je cislo
	if(array_key_exists("pretty-xml", $options)) {
		if(is_int($options["pretty-xml"])) {
			$option["pretty-xml"] = intval($options["pretty-xml"]);
		}
	} else {
		// Vychozi styl odsazeni jsou 4 mezery
		$options["pretty-xml"] = 4;
	}

	// Vytvoreni lexikalniho analyzatoru	
	$lexAnalyzator = new LexAnalyzator($options["input"]);

	// Predani lexikalniho analyzatoru Syntaktickemu analyzatoru
	$syntaxAnalyzator = new SyntaxAnalyzator($lexAnalyzator);

	// Seznam trid, jak jdou v souboru za sebou
	$listOfClasses = $syntaxAnalyzator->sestup();

	// Dodatecne semanticke kontroly
	$listOfClasses->checkOverloadInClassList();
	// Kontrola konfliktu 21
	$listOfClasses->check21();

	// Vykresleni seznamu trid, podle pozadovanych parametru
	$view;
	if(array_key_exists("details", $options)) {
		$view = new DetailView($listOfClasses, $options["details"], $options["pretty-xml"]);	
	} else {
		$view = new PrettyXMLView($listOfClasses, $options["pretty-xml"]);
	}	
	if($view) {
		$view->printView($options["output"]);
	}
	
	// Uzavreni souboru, dealokace pameti
	$lexAnalyzator->closeFile();
	exit(0);
	
	// Pomocne funkce
	/**
	 * @brief Vytiskne napovedu programu. Spousti se pomoci parametru --help
	 */
	function printHelp() {
		echo "\n";
		echo "Napoveda k php programu C++ Classes.\n";
		echo "Program se spousti prikazem interpretu PHP 5 napr. php5.6 cls.php\n";
		echo "Cilem programu je zprarsovat C++ hlavicky do XML formatu.\n";
		echo "\n";
		echo "Program lze spustit s nasledujicimi parametry: \n";
		echo "\t --details=class_name \t Vypise detaily o jednotlivych tridach. Pokud je zadan nazev tridy a trida neexistuje, vypisi se detaily o vsech tridach.\n";
		echo "\t --pretty-xml=k \t Urcuje s jak velkym odsazenim vytisknout vystup.\n";
		echo "\t --help Vytiskne tuto napovedu.\n";
		echo "\t --output=file \t Cesta k vystupnimu souboru.\n";
		echo "\t --input=file \t Cesta ke vstupnimu souboru.\n";
		exit(0);
	}
?>

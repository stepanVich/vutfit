<?php
class LexAnalyzator {
	private $fileDescriptor;

	public function __construct($fileName) {
		$this->setNewFile($fileName);
	}
	/**
	 * @brief Otevira soubor a nastavuje jeho deskriptor do vnitrni promenne objektu
	 * @param $fileName Retezec oznacujici relativni nebo absolutni cestu k souboru. Pokud neni urcen je vstup bran ze standartniho vstupu.
	 *
       	 */
	public function setNewFile($fileName = false) {	
		if($fileName == false) {
			$stdinStream = file_get_contents("php://stdin");
			$temp = tmpfile();
			fwrite($temp, $stdinStream);
			fseek($temp, 0);
			$this->fileDescriptor = $temp;
			return true;
		}
		if(!file_exists($fileName)) {
			exit(2);
		}
		$this->fileDescriptor = fopen($fileName, "r");
		if(!$this->fileDescriptor) {
			exit(2);
		}
		return true;
	}
	public function closeFile() {
		fclose($this->fileDescriptor);
	}
	/**
	* @brief Hlavni funkce lexikalniho analyzatoru. Vraci postupne jednotlive tokeny.
	* @return Token Vraci instanci tridy Token.
	*/
	public function nextToken() {
		$tokenValue = "";
		$state = 0;
		while(!feof($this->fileDescriptor)) {
			$char = fgetc($this->fileDescriptor);
			$char_num = ord($char);
			switch($state) {
				case 0:
					// Bile znaky
					if($char_num >= 0 && $char_num <= 32) {
						$state = 0;
					// Velke pismena, male pismena, _
					} elseif ( ($char_num >= 97 && $char_num <= 122) || ($char_num >= 65 && $char_num <= 90) || $char_num == 95) {
						$state = 1;
						$tokenValue .= $char;
					} else {
					// Operatory
						switch($char) {
							case "{":
							case "}":
							case ",":
							case ";":
							case ":":
							case "(":
							case ")":
							case "=":
							case "0":
							case "*":
							case "&":
							case "~":
								// Vracime token operatoru
								$tokenValue = $char;
								return new Token($tokenValue);
								break;
							default:
								// Zadna shoda -> lexikalni chyba
								$this->lexError();
						}
					}
					break;
				case 1:
					// male pismena, velke pismena, _, cislice
					if( ($char_num>= 97 && $char_num <= 122) || ($char_num >= 65 && $char_num <= 90) || $char_num == 95 || ($char_num >= 48 && $char_num <= 57)) {
						$state = 1;
						$tokenValue .= $char;
					} else {
						$state = 0;
						$this->fileUnget();	
						return new Token($tokenValue);
					}
					break;
			}
		}
		// Nastane jakmile se dostaneme na konec souboru
		return false;

	}
	private function lexError() {
		exit(4);
	}
	public function resetFile() { 
		rewind($this->fileDescriptor);
	}
	private function fileUnget() {
		fseek($this->fileDescriptor, -1, SEEK_CUR);
	}

}

/*
 * @brief Trida reprezentujici token. Uchovava hodnotu tokenu a typ tokenu. Typ tokenu urci pri konstrukci objektu.
 */
class Token {
	public $tokenValue;	
	public $tokenType;

	public function __construct($tokenValue) {
		$this->tokenValue = $tokenValue;
		$this->tokenType = $this->decideTokenType($tokenValue);
	}
	private function isKeyword() {
		$keywordArray = array("class", "public", "protected", "private", "using", "virtual", "static", "using" );
		foreach( $keywordArray as $key => $value) {
			if($value == $this->tokenValue) return true;
		}
		return false;
	}
	private function isDataType() {
		$dataTypesArray = array("void","short", "int", "signed", "unsigned", "long", "*", "&", "char16_t", "char32_t", "wchar_t", "double", "float", "char");
		foreach( $dataTypesArray as $key => $value) {
			if($value == $this->tokenValue) return true;
		}
		return false;
	}
	private function decideTokenType($tokenValue) {
		// Urci jestli je retezec operator, identifikator, nebo klicove slovo
		$tokenType = "";
		$firstChar = ord($tokenValue[0]);
		if(($firstChar >= 97 && $firstChar<= 122) || ($firstChar>= 65 && $firstChar<= 90) || $firstChar== 95 || $firstChar == 38 || $firstChar == 42) {
			$tokenType = "identifier";
			if($this->isKeyword()) $tokenType = "keyword";
			if($this->isDataType()) $tokenType = "datatype";
		} else {
			$tokenType = "operator";
		}
		return $tokenType;
	}
}
	
?>

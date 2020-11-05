<?php
/**
 * @brief Trida pro syntaktycky sestup shora dolu vstupnim souborem. Dochazi jen k jednomu pruchodu, jelikoz nemohou nastat dopredne deklarace trid.
 */
class SyntaxAnalyzator {
	private $lexAnalyzator;
	private $actualToken;
	private $classList;  
	private $walkerInfo = array();

	public function __construct($lexAnalyzator) {
		$this->lexAnalyzator = $lexAnalyzator;
		$this->classList = new ClassList();
		// Nastaveni defaultniho modifikatoru
		$this->walkerInfo["modifier"] = "private";
	}
	/*
	 * @brief Funkce ktera pracuje s lexikalnim analyzatorem, a nastavujici privatni hodnotu aktualniho tokenu.
	* @return bool True pokud se nacetl dalsi token, false pokud se dosahlo konce souboru.
	*/
	private function setNextToken() {
		if($this->lexAnalyzator != false) {
			$this->actualToken = $this->lexAnalyzator->nextToken();
			if($this->actualToken) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}
	private function actualTokenValue() {
		return $this->actualToken->tokenValue;
	}
	private function actualTokenType() {
		return $this->actualToken->tokenType;
	}
	/*
	 * @brief Funkce zahajujici syntaktycky sestup.
	 * @return List trid
	 */
	public function sestup() {

		// Kontrola nastaveneho lex analyzatoru
		if($this->lexAnalyzator == null) return false;
		
		// Syntakticky pruchod zhora dolu
		if($this->S() == false) {
			// Doslo k syntakticke chybe
			$this->syntaxError();
		} else {
			// Vstupni soubor je syntakticky spravne
			return $this->classList;
		}
			

	}
	
	private function syntaxError() {
		exit(4);
	}

	// Vypis jednotlivych funkci pouzivanych pri syntaktickem pruchodu, predstavuji neterminaly
	private function S() {
		// <S> => <list_of_classes>
		$this->setNextToken();
		
		if($this->actualTokenValue() == "class" || $this->actualTokenValue() == false)	{
			return $this->list_of_classes();	
		}
	}
	
	private function list_of_classes() {
		// <list_of_classes> => <class_f> <list_of_classes>
		if($this->actualTokenValue() == "class") {
			return ($this->class_f() && $this->list_of_classes());
		// <list_of_classes> => Epsilon
		} elseif($this->actualTokenValue() == false) {
			return true;	
		}
		return false;

	}

	private function class_f() {
		// <class_f> => class class_id { <class_obsah> };
		if($this->actualTokenValue() == "class") {
			
			$this->setNextToken();	
			if($this->actualTokenType() == "identifier") {
				// Nastaveni jmena aktualni tridy
				$this->walkerInfo["class_name"] = $this->actualTokenValue();
				// Pridani tridy do datoveho modelu
				$this->classList->addClass($this->actualTokenValue());
				$this->setNextToken();
				if($this->actualTokenValue() == "{") {
					$this->setNextToken();
					if($this->class_obsah()) {
						if($this->actualTokenValue() == "}") {
							$this->setNextToken();
							if($this->actualTokenValue() == ";") {
								$this->setNextToken();
								return true;
							}
						}	
					}
				}
				elseif($this->actualTokenValue() == ":") {
					$this->setNextToken();
					if($this->inheritances()) {
						if($this->actualTokenValue() == "{") {
							$this->setNextToken();
							if($this->class_obsah()) {
								if($this->actualTokenValue() == "}") {
									$this->setNextToken();
									if($this->actualTokenValue() == ";") {
										$this->setNextToken();
										return true;
									}
								}

							}
						}	
					}
				}
			}
		}
					
		return false;
	}

	private function inheritances() {
		// Epsilon pravidlo nelze aplikovat
		if($this->actualTokenValue() == "{") {
			return false;
		}
		if($this->modifier()) {
			if($this->actualTokenType() == "identifier") {
				// Zrejmen nemusime nastavovat, budeme s hodnotou pracovat jen na tomto miste
				$this->walkerInfo["inheritance_name"] = $this->actualTokenValue();
				$this->classList->addClassParent($this->walkerInfo["class_name"], $this->walkerInfo["inheritance_name"], $this->walkerInfo["modifier"]);
				// Vynulovat modifier
				$this->walkerInfo["modifier"] = "private";
				$this->setNextToken();
				if($this->actualTokenValue() == ",") {
					$this->setNextToken();
					return $this->inheritances();
				}
				return true;
			}
		}
		return false;
	}

	private function modifier() {
		if($this->actualTokenValue() == "public" || $this->actualTokenValue() == "protected" || $this->actualTokenValue() == "private") {
			$this->walkerInfo["modifier"] = $this->actualTokenValue();
			$this->setNextToken();
			return true;
		}
		// epsilon pravidlo
		// Defaultni modifier - private
		$this->walkerInfo["modifier"] = "private";
		return true;
	}

	private function class_obsah() {
		if($this->actualTokenValue() == "}") {
			// Vynulovat modifier
			$this->walkerInfo["modifier"] = "private";
			return true;
		}
		elseif($this->attribute_or_method_or_using()) {
			return $this->class_obsah();
		}
		elseif($this->not_empty_modifier()) {
			if($this->actualTokenValue() == ":") {
				$this->setNextToken();
				return $this->class_obsah();
			}
		}
		return false;
		
	}

	private function not_empty_modifier() {
		if($this->actualTokenValue() == "public" || $this->actualTokenValue() == "protected" || $this->actualTokenValue() == "private") {
			$this->walkerInfo["modifier"] = $this->actualTokenValue();
			$this->setNextToken();
			return true;
		}
		
		// Bez epsilon pravidla
		return false;
	}
	private function attribute_or_method_or_using() {
		return $this->attribute_or_method() || $this->using_keyword();
	}

	private function attribute_or_method() {
		if($this->static_f()) {
			if($this->actualTokenValue() == "virtual") {
				$this->walkerInfo["is_virtual"] = true;
				$this->setNextToken();
			} else {
				$this->walkerInfo["is_virtual"] = false;
			}
					

			if($this->data_type(true)) {
				if($this->actualTokenType() == "identifier") {
					$this->walkerInfo["identifier"] = $this->actualTokenValue();
					$this->setNextToken();
					if($this->actualTokenValue() == ";") {
						// Pridani atributu
						$this->classList->addAttribute($this->walkerInfo["class_name"], $this->walkerInfo["data_type"], $this->walkerInfo["identifier"], $this->walkerInfo["modifier"], $this->walkerInfo["is_static"], $this->walkerInfo["is_virtual"]);	
						$this->setNextToken();
						return true;
					}	
					if($this->actualTokenValue() == "(") {
						// Pridani metody
						$this->walkerInfo["current_method_link"] = $this->classList->addMethod($this->walkerInfo["class_name"], $this->walkerInfo["identifier"], $this->walkerInfo["modifier"], $this->walkerInfo["data_type"], $this->walkerInfo["is_static"], $this->walkerInfo["is_virtual"]);
						$this->setNextToken();
						if($this->function_args()) {
							if($this->actualTokenValue() == ")") {
								$this->classList->checkShadowing($this->walkerInfo["class_name"], $this->walkerInfo["current_method_link"]);
								$this->setNextToken();
								// Volitelne zavorky
								if($this->actualTokenValue() == "{") {
									$this->setNextToken();
									if($this->actualTokenValue() == "}") {
										$this->setNextToken();
									}
									// Volitelny strednik za definici metody
									if($this->actualTokenValue() == ";") {
										$this->walkerInfo["is_pure_virtual"] = false;
										$this->setNextToken();
									}
									return true;
								}
								// Nutny strednik za definici metody - bez {}
								elseif($this->actualTokenValue() == ";") {
									$this->walkerInfo["is_pure_virtual"] = false;
									$this->setNextToken();
									return true;
								}
								elseif($this->actualTokenValue() == "=") {
									$this->setNextToken();
									if($this->actualTokenValue() == "0") {
										$this->setNextToken();
										if($this->actualTokenValue() == ";") {
											$this->walkerInfo["is_pure_virtual"] = true;	
											$this->classList->methodIsPureVirtual($this->walkerInfo["class_name"], $this->walkerInfo["identifier"]);
											$this->setNextToken();
											return true;
										}
									}
								}
							}
						}
					}
				// Konstruktor
				} elseif($this->actualTokenValue() == "(") {
					$this->walkerInfo["identifier"] = $this->walkerInfo["data_type"];
					// Pridat konstruktor mezi funkce
					$this->walkerInfo["current_method_link"] = $this->classList->addConstructor($this->walkerInfo["class_name"], $this->walkerInfo["identifier"]);
					$this->setNextToken();
					if($this->function_args()) {
						if($this->actualTokenValue() == ")") {
							$this->classList->checkShadowing($this->walkerInfo["class_name"], $this->walkerInfo["current_method_link"]);
							$this->setNextToken();
							if($this->actualTokenValue() == "{") {
								$this->setNextToken();
								if($this->actualTokenValue() == "}") {
									$this->setNextToken();
									// volitelny strednik za definici konstruktoruA
									if($this->actualTokenValue() == ";") {
										$this->setNextToken();
									}
									return true;
								}	
							}
							elseif($this->actualTokenValue() == ";") {
								$this->setNextToken();
								return true;	
							}
						}
					}
				}
			// Destruktor
			// Nemusime vubec pouzivat data_type...
			} elseif($this->actualTokenValue() == "~") {
				$this->setNextToken();
				if($this->actualTokenType() == "identifier") {
					$this->walkerInfo["identifier"] = $this->actualTokenValue();
					$this->setNextToken();
					if($this->actualTokenValue() == "(") {
						$this->setNextToken();	
						if($this->actualTokenValue() == "void") {
							$this->setNextToken();
						}
						if($this->actualTokenValue() == ")") {
							$this->classList->addDestructor($this->walkerInfo["class_name"], $this->walkerInfo["identifier"]);
							$this->setNextToken();
							if($this->actualTokenValue() == "{") {
								$this->setNextToken();
								if($this->actualTokenValue() == "}") {
									$this->setNextToken();
									// volitelny strednik za definici destruktoru
									if($this->actualTokenValue() == ";") {
										$this->setNextToken();
									}
									// Pridat destruktor mezi funkce
									return true;
								}
							}
							elseif($this->actualTokenValue() == ";") {
								$this->setNextToken();
								return true;
							}
						}
					}
				}
			}
		}
		// Epsilon pravidla zde resi nadrazene funkce
		return false;
	}

	private function static_f() {
		if($this->actualTokenValue() == "static") {
			$this->walkerInfo["is_static"] = true;
			$this->setNextToken();
			return true;
		} else {
			$this->walkerInfo["is_static"] = false;
		}
		return true;
	}

	private function function_args() {
		// Epsilon pravidlo
		if($this->actualTokenValue() == ")") {
			return true;
		} elseif($this->actualTokenValue() == "void") {
			$this->setNextToken();
			if($this->actualTokenValue() == ")") {
				return true;
			}	
		} elseif($this->argument()) {
			if($this->actualTokenValue() == ",") {
				$this->setNextToken();
				return $this->argumenty();
			}
			return true;
		}
		return false;
	}

	private function argumenty() {
		if($this->argument()) {
			if($this->actualTokenValue() == ",") {
				$this->setNextToken();
				return $this->argumenty();
			}
			return true;	
		}	
		return false;
	}

	private function argument() {
		if($this->data_type(true)) {
			if($this->actualTokenType() == "identifier") {
				$this->walkerInfo["argument_name"] = $this->actualTokenValue();
				$this->classList->addMethodArgument($this->walkerInfo["class_name"], $this->walkerInfo["identifier"], $this->walkerInfo["data_type"], $this->walkerInfo["argument_name"], $this->walkerInfo["current_method_link"]);
				$this->setNextToken();
				return true;
			}
		}
		return false;
	}

	private function using_keyword() {
		if($this->actualTokenValue() == "using") {
			$this->setNextToken();
			if($this->actualTokenType() == "identifier") {
				$this->walkerInfo["using_class_name"] = $this->actualTokenValue();
				$this->setNextToken();
				if($this->actualTokenValue() == ":") {
					$this->setNextToken();
					if($this->actualTokenValue() == ":") {
						$this->setNextToken();
						if($this->actualTokenType() == "identifier") {
							$this->walkerInfo["using_attribute_id"] = $this->actualTokenValue();
							$this->setNextToken();
							if($this->actualTokenValue() == ";") {
								$this->classList->useUsing($this->walkerInfo["class_name"], $this->walkerInfo["using_class_name"], $this->walkerInfo["using_attribute_id"], $this->walkerInfo["modifier"]);
								$this->setNextToken();
								return true;
							}
						}
					}
				}	
			}	
		}
		return false;
	}

	private function data_type($firstToken) {
		if($firstToken) {
			if($this->actualTokenType() == "identifier") {
				// datovym typem je trida
				$this->walkerInfo["data_type"] = $this->actualTokenValue() . " ";
				$this->setNextToken();
				if($this->actualTokenValue() == "*" || $this->actualTokenValue() == "&") {
					$this->walkerInfo["data_type"] .= $this->actualTokenValue();	
					$this->setNextToken();
				}
				$this->walkerInfo["data_type"] = rtrim($this->walkerInfo["data_type"]);
				return true;
			}
		}
		if($this->actualTokenType() == "datatype") {
			// Vynulovani puvodniho datoveho typu
			if($firstToken) $this->walkerInfo["data_type"] = "";
			$this->walkerInfo["data_type"] .= $this->actualTokenValue() . " ";
			$this->setNextToken();
			// Resi cyklicke zadani datoveho typu. Napr. long long logn int...
			return $this->data_type(false);
		}
		// Trim end of the data_type value - odstrihnuti koncove mezery
		$this->walkerInfo["data_type"] = rtrim($this->walkerInfo["data_type"]);
		return !$firstToken;
	}
}

?>

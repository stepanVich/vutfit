<?php
	// Modul vyuziva knihovnu XMLWriter pro vypsani XML struktury
	// Obsahuje dva zakladni pohledy (pretty-xml a details)
	/**
	 * @brief Rozhrani nutne pro praci s Pohledy
	 */
	interface View {
		/**
		 * @brief Tiskne XMLObjekt. Stara se o otevreni vystupni parametru.
		 */
		public function printView($output = "");
		/**
		 * @brief Vytvari XMLObjekt, pri prochazeni listu trid.
		 */
		public function buildView($listOfClasses);
	}
	/**
	 * @brief Tiskne seznam trid, podle jejich dedicnosti.
	 */
	class PrettyXMLView implements View {
		private $xmlObject; 
		private $indentString;

		public function __construct($listOfClasses, $indent = 4) {
			$indentString = "";
			for($i=0;$i<$indent;$i++) {
				$indentString .= " ";
			}
			$this->indentString = $indentString;
			$this->buildView($listOfClasses);
		}

		public function buildView($listOfClasses) {
			$this->xmlObject = new XMLWriter();
			$this->xmlObject->openMemory();
			$this->xmlObject->startDocument("1.0", "UTF-8");
			$this->xmlObject->setIndent(true);
			$this->xmlObject->setIndentString($this->indentString);
			// Vyfiltruj tridy tak aby zbyli jen ty ktere nemaji zadne inheritances - jedna se o bazove tridy na nejvyssi urovni lesa
			$baseClasses = array();
			foreach($listOfClasses->getClassList() as $class) {
				if(count($class->getInheritances()) == 0) array_push($baseClasses, $class);
			}
			// <model>
			$this->xmlObject->startElement("model");
			// Cykleni napric vyfiltrovanymi tridami
			foreach($baseClasses as $class) {
				$this->recursiveClass($class);
			}
			// </model>
			$this->xmlObject->endElement();
			// Ukonci dokument
			$this->xmlObject->endDocument();
		}
		/**
		 * @brief Rekurzivne prochazi seznamem potomku tridy a vypisuje informace o nich.
		 */
		private function recursiveClass($currentClass) {
			$this->xmlObject->startElement("class");	
				$this->xmlObject->writeAttribute("name", $currentClass->getClassName());
				$this->xmlObject->writeAttribute("kind", ($currentClass->isAbstract() ? "abstract" : "concrete"));
				foreach($currentClass->getKids() as $class) {
					$this->recursiveClass($class);	
				}
			$this->xmlObject->endElement();
		}

		public  function printView($output = false) {
			$xmlString = $this->xmlObject->outputMemory();
			if($output == false) {
				echo $xmlString;
			} else {
				$outputFile = fopen($output, "w");
				if($outputFile) {
					fwrite($outputFile, $xmlString);
					fclose($outputFile);
				} else {
					exit(3);
				}
			}
		}
	}
	/**
	 * @brief Vytiskne detailni pohled na vsechny tridy nebo jen na jednu tridu.
	 */
	class DetailView implements View {
		private $xmlObject;
		private $detailedClassId;
		private $indentString;

		public function __construct($listOfClasses, $detailedClassIdentifier = false, $indent= 4)  {
			$this->detailedClassId = $detailedClassIdentifier;
			$indentString = "";
			for($i=0;$i<$indent;$i++) {
				$indentString .= " ";
			}
			$this->indentString = $indentString;
			$this->buildView($listOfClasses);
		}

		public function buildView($listOfClasses) {
			$this->xmlObject = new XMLWriter();
			$this->xmlObject->openMemory();
			$this->xmlObject->setIndent(true);
			$this->xmlObject->setIndentString($this->indentString);
			$this->xmlObject->startDocument('1.0','UTF-8');	
			// Identifikator tridy musi byt zadan
			if($this->detailedClassId) {
				$class = $listOfClasses->findClass($this->detailedClassId);
				if(!$class) {
				// Pokud trida neexistuje, je vypsana jen XML hlavicka
					$this->xmlObject->endDocument();
				} else {
				// Pokud trida existuje jsou vypsany detaily jen o ni	
					$this->classDetails($class);
					$this->xmlObject->endDocument();
				}
			} else {

				// Vytvoreni objektu model
				$this->xmlObject->startElement("model");
				// Vypsani vsech trid
				foreach($listOfClasses->getClassList() as $class) {
					$this->classDetails($class);
				}
				// Ukonceni elementu a dokumentu
				$this->xmlObject->endElement();
				$this->xmlObject->endDocument();
				
			}
		}
		/**
		 * @brief Tiskne informaci o dane tride z listu trid.
		 */
		private function classDetails($class) {
			// <class>
			$this->xmlObject->startElement("class");
			$this->xmlObject->writeAttribute("name", $class->getClassName());
			$this->xmlObject->writeAttribute("kind", ($class->isAbstract() ? "abstract" : "concrete"));
				// Vytiskne inheritance element jen pokud nejaka dedicnost existuje
				if(count($class->getInheritances()) != 0) {
					// <inheritance>
					$this->xmlObject->startElement("inheritance");
					// Cykleni pres dedicnosti
					foreach($class->getInheritances() as $inheritance) {
						// <from>
						$this->xmlObject->startElement("from");
						$this->xmlObject->writeAttribute("name", $inheritance->getBaseClass()->getClassName());
						$this->xmlObject->writeAttribute("privacy", $inheritance->getModifier());	
						$this->xmlObject->endElement();
						// </from>
					}	
					// </inheritance>
					$this->xmlObject->endElement();
				}

				if($class->hasAttributes("private") || $class->hasMethods("private")) {
					// Get private attributes from inheritance classes
					// <private>
					$this->xmlObject->startElement("private");
					if($class->hasAttributes("private")) {
						// vypsani privatnich atributu
						$this->xmlObject->startElement("attributes");
						$this->attributesDetail($class, "private");	
						$this->xmlObject->endElement();
					}
					if($class->hasMethods("private")) {
						// vypsani privatnich metod
						$this->xmlObject->startElement("methods");
						$this->methodsDetail($class, "private");
						$this->xmlObject->endElement();
					}
					// </private>
					$this->xmlObject->endElement();
				}

				if($class->hasAttributes("protected") || $class->hasMethods("protected")) {
					// Get protected attributes and methods
					// <protected>
					$this->xmlObject->startElement("protected");
					if($class->hasAttributes("protected")) {
						$this->xmlObject->startElement("attributes");
						// vypsani protected atribute	
						$this->attributesDetail($class, "protected");
						$this->xmlObject->endElement();
					}
					if($class->hasMethods("protected")) {
						$this->xmlObject->startElement("methods");
						$this->methodsDetail($class, "protected");
						$this->xmlObject->endElement();						
					}
					// </protected>
					$this->xmlObject->endElement();
				}

				if($class->hasAttributes("public") || $class->hasMethods("public")) {
					// Get public attributes and methods
					// <public>
					$this->xmlObject->startElement("public");
					if($class->hasAttributes("public")) {
						$this->xmlObject->startElement("attributes");
						// vypsani protected atribute	
						$this->attributesDetail($class, "public");
						$this->xmlObject->endElement();
					}
					if($class->hasMethods("public")) {
						$this->xmlObject->startElement("methods");
						$this->methodsDetail($class, "public");
						$this->xmlObject->endElement();						
					}
					// </public>
					$this->xmlObject->endElement();
				}

			// </class>
			$this->xmlObject->endElement();
		}
		/**
		 * @brief Tiskne podrobne informace o atributu tridy.
		 */
		private function attributesDetail($class, $modifier) {
			// Cykleni napric atributy dane tridy
			foreach($class->getAttributes($modifier) as $attribute) {

				// Privatni, zdedene atributy nevypisujeme
				if($attribute->isInherited && $attribute->wasPrivateInParent) continue;

				$this->xmlObject->startElement("attribute");
				$this->xmlObject->writeAttribute("name", $attribute->getName());
				$this->xmlObject->writeAttribute("type", $attribute->getDataType());
				$this->xmlObject->writeAttribute("scope", ($attribute->isStatic() ? "static" : "instance"));
				if($attribute->isInherited) {
					// <from>
					$this->xmlObject->startElement("from");
					$this->xmlObject->writeAttribute("name", $attribute->inheritedFrom);
					// </from>
					$this->xmlObject->endElement();
				}
				// </attribute>
				$this->xmlObject->endElement();
			}
		}
		/**
		 * @brief Tiskne podrobne informace o metode tridy.
		 */
		private function methodsDetail($class, $modifier) {
			// Cykleni napric metodami dane tridy
			foreach($class->getMethods($modifier) as $method) {

				// Privatni zdedene ciste virtualni metody nezobrazujeme
				if($method->isPureVirtual() && $method->isInherited && ($method->getModifier() == "private") && $method->wasPrivateInParent) continue;
				// Privatni zdedene metody nezobrazujeme
				if($method->isInherited && $method->wasPrivateInParent) continue;

				$this->xmlObject->startElement("method");
				$this->xmlObject->writeAttribute("name", $method->getName());
				$this->xmlObject->writeAttribute("type", $method->getReturnType());
				$this->xmlObject->writeAttribute("scope", ($method->isStatic() ? "static" : "instance"));
				if($method->isInherited) {
					// <from>
					$this->xmlObject->startElement("from");
					$this->xmlObject->writeAttribute("name", $method->inheritedFrom);
					// </from>
					$this->xmlObject->endElement();
				}
				// virtual argument
				if($method->isVirtual()) {
					// <virtual>
					$this->xmlObject->startElement("virtual");
					$this->xmlObject->writeAttribute("pure", ($method->isPureVirtual() ? "yes" : "no"));
					// </virtual>
					$this->xmlObject->endElement();
				}
				// argumenty funkce
				// <arguments>
				$this->xmlObject->startElement("arguments");
				foreach($method->getArguments() as $argument) {
					$this->xmlObject->startElement("argument");
					$this->xmlObject->writeAttribute("name", $argument->getName());
					$this->xmlObject->writeAttribute("type", $argument->getDataType());
					$this->xmlObject->endElement();
				}
				// </arguments>
				$this->xmlObject->endElement();
				// </method>
				$this->xmlObject->endElement();
			}
		}

		public function printView($output = false){
			$xmlString = $this->xmlObject->outputMemory();
			if($output == false) {
				echo $xmlString;
			} else {
				$outputFile = fopen($output, "w");
				if($outputFile) {
					fwrite($outputFile, $xmlString);
					fclose($outputFile);
				} else {
					exit(3);
				}
			}
		}
	}

?>

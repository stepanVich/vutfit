<?php
/** @brief Trida poskytujici rozhrani metod, pro ukladani vsech potrebnych informaci sesbiranych pri sestupu soubore shora dolu.
 * Odstinuje praci s daty od syntaktickeho analyzatoru.
 * Syntakticky analyzator, pracuje vyhradne s touto tridou pri sestupu shora dolu.
 */
class ClassList {
	// Jednoduche pole, uchovavajici objekty trid
	// Objekt tridy uchovava informace o dedicnostech, metodach a atributech
	private $classList = array();
	public function getClassList() {
		return $this->classList;
	}
	public function __construct() {
	}
	private function semanticError() {
		exit(4);
	}

	/**
	 * @brief Kontroluje zda datovy typ spada pod zakladni povolene typy. Pokud ne, spusti syntakticko chybu s navratovym kodem 4.
	 */
	public function checkDataType($dataType) {
		$isPrimitiveDataType = false;
		$isObjectType = false;
		// Zkontrolovat posledni znak datoveho typu
		$lastChar = substr($dataType, -1);
		if($lastChar == "*" || $lastChar == "&") {
			$dataType = substr($dataType, 0, strlen($dataType) - 1);
			$dataType = rtrim($dataType);
		}
		switch($dataType) {
			case "void":
			case "bool":
			case "char":
			case "char16_t":
			case "char32_t":
			case "wchar_t":
			case "signed char":
			case "short int":
			case "int":
			case "long int":
			case "long long int":
			case "unsigned char":
			case "unsigned short int":
			case "unsigned int":
			case "unsigned long int":
			case "unsigned long long int":
			case "float":
			case "double":
			case "long double":
				$isPrimitiveDataType = true;
				break;
		}
		foreach($this->classList as $class) {
			if($class->getClassName() == $dataType) $isObjectType = true;
		}
		if(!$isPrimitiveDataType && !$isObjectType) {
			$this->semanticError();
		}
	}

	/**
	 * @brief Postara se o pridani tridy do seznamu trid.
	 */
	public function addClass($classIdentifier) {
		// Kontrola, ze jiz neexistuje trida ze stejnym nazvem	
		foreach($this->classList as $class) {
			if($class->getClassName() == $classIdentifier) {
				$this->semanticError();
			}
		}	
		// Vytvoreni objektu tridy
		$class = new ClassObject($classIdentifier);
		// Pridani tridy do seznamu trid
		array_push($this->classList, $class);

	}

	/**
	  * @brief Nalezne tridu v seznamu trid podle jmena tridy.
	  * @return Objekt tridy nebo false pokud trida nebyla nalezena.
	 */
	public function findClass($classIdentifier) {
		foreach($this->classList as $class) {
			if($class->getClassName() == $classIdentifier) return $class;
		}
		return false;
	}

	/**
	 * @brief Prida dedicnost do systemu dedicnosti.
	 */
	public function addClassParent($subClassIdentifier, $parentIdentifier, $modifier="private") {
		// nalezeni bazove a dcerine tridy v seznamu trid
		// pokud neexistuji, je vyvolana syntaktycka chyba
		$baseClass = $this->findClass($parentIdentifier);
		$subClass = $this->findClass($subClassIdentifier);
		if(!$baseClass || !$subClass) {
			$this->semanticError();
		}
		
		// propojeni dcerine tridy z bazovou a naopak
		// problem je delegovan do objektu dcerine tridy - zde se i resi konflikty nazvu a dalsi konflikty
		$returnValue = $subClass->addInheritance($baseClass, $modifier);
		if(!$returnValue) $this->semanticError();
	}

	/**
	 * @brief Prida metodu do seznamu trid.
	 */
	public function addMethod($classIdentifier, $methodIdentifier, $modifier="private", $returnType, $isStatic, $isVirtual, $isConstructor = false, $isDestructor = false) {
		// Semanticka kontrola - pokud se jedna o konstruktor nebo destruktor, nemohou mit nastaveny navratove typy
		if($isConstructor || $isDestructor) {
			if($returnType) exit(4);
			$returnType = "void";
		}

		$this->checkDataType($returnType);

		// Semanticka kontrola - pokud je metoda virtualni, nemuze byt staticka
		if($isStatic && $isVirtual) {
			$this->semanticError();
		}

		//  Vytvoreni objektu metody
		$method = new MethodObject($methodIdentifier, $returnType, $modifier, $isStatic, $isVirtual, $isConstructor, $isDestructor);
		// Nalezeni vhodne tridy
		$class = $this->findClass($classIdentifier);
		
		// Delegovani problemu do dane Tridy
		if(!$class) $this->semanticError();
		return $class->addMethod($method);
	}

	/**
	 * @brief Prida kontruktor do seznamu metod.
	 */
	public function addConstructor($classId, $methodId) {
		if($classId != $methodId) exit(4);
		return $this->addMethod($classId, $methodId,"public",false , false, false, true, false);
	}

	/**
	 * @brief Prida destruktor do seznamu trid	
	 */
	public function addDestructor($classId, $methodId) {
		if($classId != $methodId) exit(4);
		$methodId = "~" . $methodId;
		return $this->addMethod($classId, $methodId, "public",false, false, false, false, true);
	}

	/**
	 * @brief Nastavi metodu na ciste virtualni.
	 */
	public function methodIsPureVirtual($classIdentifier, $methodIdentifier) {
		$class = $this->findClass($classIdentifier);
		if(!$class) $this->semanticError();
		$method = $class->findMethod($methodIdentifier);
		if(!$method) $this->semanticError();
		$method->setPureVirtual(true);
	}

	/**
	 * @brief Prida argument do metody.
	 */
	public function addMethodArgument($classIdentifier, $methodIdentifier, $dataType, $argIdentifier, $methodLink) {
		$this->checkDataType($dataType);
		$class = $this->findClass($classIdentifier);
		if(!$class) $this->semanticError();
		$method = $class->findMethod($methodIdentifier);
		if(!$method) $this->semanticError();
		$argument = new MethodArgument($argIdentifier, $dataType);
		$methodLink->addArgument($argument);
	}

	/**
	 * @brief  Prida atribut do tridy.
	 */
	public function addAttribute($classIdentifier, $dataType, $attrIdentifier, $modifier="private", $isStatic, $isVirtual) {
		$this->checkDataType($dataType);
		// Kontrola zda je atribut virtualni - pokud ano nastane semanticka chyba
		if($isVirtual) {
			$this->semanticError();
		}
		$attribute = new Attribute($attrIdentifier, $dataType, $modifier, $isStatic);
		$class = $this->findClass($classIdentifier);
		if(!$class) {
			$this->semanticError();
		}
		$class->addAttribute($attribute);
	}

	/**
	 * @brief Prida using.
	 */
	public function useUsing($classIdentifier, $baseClassId,  $identifier, $modifier) {
		$class = $this->findClass($classIdentifier);
		$baseClass = $this->findClass($baseClassId);
		if(!$class) exit(4);	
		if(!$baseClass) exit(4);
		$class->addUsing($baseClass, $identifier, $modifier);
	}

	/**
	 * @brief Zkontroluje zda jsou metoy vsech trid v listu trid dobre pretizeny. Nelze mit dve metody se stejnymi datovymi typy argument.
	 */
	public function checkOverloadInClassList() {
		foreach($this->classList as $class) {
			$class->checkOverloadInClass();
		}
	}

	/**
	 * @brief Skontroluje zda se v tride nenachazi nezdedenene vlastnosti, ktere zastini zdedene vlastnosti. Ty se odstrani.
	 */
	public function checkShadowing($className, $methodObject) {
		$class = $this->findClass($className);
		if($class) $class->checkShadowing($methodObject);
	}

	/**
	 * @brief Zkontroluje zda se ve tridach nevyskutuji konflikty s navratovym kodem 21.
	 */
	public function check21() {
		foreach($this->classList as $class) {
			foreach(array_merge($class->getMethods(), $class->getAttributes()) as $key1 => $prop1) {
				if(!$prop1->isInherited) continue;	
				foreach(array_merge($class->getMethods(), $class->getAttributes()) as $key2 => $prop2) {
					if(!$prop2->isInherited) continue;
					if($prop2->getName() == $prop1->getName() && $key1 != $key2) {
						if($prop1->isMethod && $prop2->isMethod) {
							// Muze se jednat o pretizene metody - nutno zkontrolovat argumenty metod - pokud se lisi jedna se o pretizeni
							$args1 = $prop1->getArguments();		
							$args2 = $prop2->getArguments();
							if(count($args1) != count($args2)) continue;
							$isSame = true;
							for($i = 0;$i < count($args1);$i++) {
								if($args1[$i]->getDataType() != $args2[$i]->getDataType()) $isSame = false; 	
							}	
							if($isSame) {
								exit(21);
							} else {
								continue;	
							}
						} else {
							exit(21);
						}
					}
				}
			}	
		}
	}
}


/** @brief Trida obsahujici informace o jednotlivych tridach.
*/
class ClassObject {


	/** 
	 * @brief Zkontroluje zda trida obsahuje alespon jeden atribut s danym modifikatorem pristupu. Odfiltruje privatni atribut, ktery byl privatni i v nadrazene tride.
	 * @return bool True pokud trida obsahuje alespon jeden atribut s danym modifikator, false pokud ne.
	*/
	public function hasAttributes($modifier) {
		// prohledani v aktualni tride atributy se shodnym modifikatorem
		foreach($this->attributes as $attr) {
			if($attr->getModifier() == $modifier && !($attr->getModifier() == "private" && $attr->wasPrivateInParent && $attr->isInherited)) return true;
		}

		// AKtualni trida nema zadny atribut se shodnym modifikatorem
		return false;
	}


	public function hasMethods($modifier) {
		// prohledani v aktualni tride atributy se shodnym modifikatorem
		foreach($this->methods as $method) {
			if($method->getModifier() == $modifier && !($method->isInherited == true && $method->getModifier() == "private" && $method->wasPrivateInParent)) return true;
		}
		return false;
	}

	private $className;
	private $methods = array();


	public function findMethod($methodIdentifier) {
		foreach($this->methods as $method) {
			if($method->getName() == $methodIdentifier) {
				return $method;
			}
		}
		return false;
	}


	private $attributes = array();


	public function findAttribute($attributeIdentifier) {
		foreach($this->attributes as $attribute) {
			if($attribute->getAttributeName() == $attributeIdentifier) {
				return $attribute;
			}
		}
	}


	
	// Seznam trid ze kterych trida dedi - seznam bazovych trid
	private $inheritances = array();


	public function inheritancesContain($classIdentifier) {
		foreach($this->inheritances as $inheritance) {
			if($inheritance->getBaseClass()->getClassName() == $classIdentifier) return true;
		}
		return false;
	}


	// Seznam trid ktere dedi z teto tridy - seznam "deti"
	private $kids = array();


	public function getKids() {
		return $this->kids;
	}


	public function kidsContain($classIdentifier) {
		foreach($this->kids as $class) {
			if($class->getClassName() == $classIdentifier) return true;
		}
		return false;
	}

	public function isAbstract() {
		// Abstraktni trida obsahuje alespon jednu pure virtual metodu
		foreach($this->methods as $method) {
			if($method->isPureVirtual()) return true;
		} 
		return false;
	}


	public function getMethods($modifier = false) {
		if($modifier == false) {
			return $this->methods;
		} else {
			$methodsMod = array();
			foreach($this->methods as $method) {
				if($method->getModifier() == $modifier) {
					array_push($methodsMod, $method);
				}
			}
			return $methodsMod;
		}
	}


	public function getAttributes($modifier = false) {
		if($modifier == false) {
			return $this->attributes;
		} else {
			$attributesMod = array();
			foreach($this->attributes as $attribute) {
				if($attribute->getModifier() == $modifier) {
					array_push($attributesMod, $attribute);
				}
			}
			return $attributesMod;
		}
	}

	public function getInheritances() {
		return $this->inheritances;
	}

	public function getClassName() {
		return $this->className;
	}

	public function __construct($classIdentifier = "") {
		$this->className = $classIdentifier;	
	}

	/**
	 * @brief Prida metody do pole metod. Kontroluje konflikt jmen.
	 */
	public function addMethod($methodObject) {
		// Kontrola ze v dane tride jiz neni stejny zdedeny objekt
		if($methodObject->isInherited) {
			foreach($this->getMethods() as $method) {
				$isSame = true;
				if($method->isInherited != $methodObject->isInherited) $isSame = false;
				if($method->getName() != $methodObject->getName()) $isSame = false;
				if($method->getReturnType() != $methodObject->getReturnType()) $isSame = false;
				if($method->isStatic() != $methodObject->isStatic()) $isSame = false;
				if($method->isVirtual() != $methodObject->isVirtual()) $isSame = false;
				if($method->isPureVirtual() != $methodObject->isPureVirtual()) $isSame = false;
				if($method->inheritedFrom != $methodObject->inheritedFrom) $isSame = false;
				if($method->lastClass != $methodObject->lastClass) $isSame = false;
				$args1 = $method->getArguments();
				$args2 = $methodObject->getArguments();
				for($i = 0;$i<count($args1);$i++) {
					if($args2[$i]) {
						if($args1[$i]->getDataType() != $args2[$i]->getDataType()) $isSame = false;
						if($args1[$i]->getName() != $args2[$i]->getName()) $isSame = false;
					} else {
						$isSame = false;
					}
				}	
				if($isSame) return;
			}
		}
		// Kontrola konfliktu mezi atributy v dane tride
		if(!$methodObject->isInherited) {
			foreach($this->attributes as $attribute) {
				if(!$attribute->isInherited) {
					if($methodObject->getName() == $attribute->getName()) exit(4);
				}
			}
			// Kontrola mezi zdedenymi cleny
			foreach($this->attributes as $key => $attr ) {
				if($attr->isInherited) {
					if($attr->getName() == $methodObject->getName()) {
						unset($this->attributes[$key]);
					}
				}
			}
			// Zastineni se kontroluje az na konci prochazeni - argumenty nejsou uplne
			foreach($this->methods as $key => $method) {
				if($method->isInherited) {
					if($method->getName() == $methodObject->getName()) {
						// Musime zkontrolovat zda se nejedna o pretizenou metodu
						//unset($this->methods[$key]);
					}
				}
			}
		}
		// Pridani metody do pole metod	
		array_push($this->methods, $methodObject);
		return $methodObject;
	}

	/*
	 * @brief Zjistime zda jsou vsechny metody spravne pretizeny. Metoda je spatne pretizena pokud se jmenuje stejne a ma stejne datove typy argumentu.
	 */
	public function checkOverloadInClass() {
		// Spousti se az po projeti celeho souboru
		// Jelikoz se argumenty pridavaji postupne, nemuze se spustit po pridani metody
		// Kontrola spravneho pretezeni mezi vsemi metodami vsech trid
		for($m = 0; $m < count($this->methods); $m++) {
			$method = $this->methods[$m];
			for($n = 0; $n < count($this->methods);$n++) {
				$methodObject = $this->methods[$n];
				if($n == $m) continue;
				if($method == false || $methodObject == false) continue;
				if(!$method->isInherited && !$methodObject->isInherited) {
					// Kontrola zda se nejedna o konstruktory a destruktory
					if($method->isConstructor == true && $methodObject->isDestructor == true) {
						continue;
					}
					if($method->isDestructor == true && $methodObject->isConstructor == true) {
						continue;
					}	

					if($method->getName() == $methodObject->getName()) {
						// Porovname argumenty metod, pokud jsou vsechny stejne, nejedna se o pretizeni metody ->chyba	
						$args1 = $method->getArguments();	
						$args2 = $methodObject->getArguments();
						$isSame = true;	
						if( count($args1) != count($args2) ) continue;
						for($i = 0; $i < count($args1); $i++) {
							if($args1[$i] && $args2[$i]){ 
								if($args1[$i]->getDataType() != $args2[$i]->getDataType()) $isSame = false;	
							}
						}

						if($isSame == true) exit(4);
					}
				}
			}
		}
	}
	/**
	 * @brief Skontroluje zda se ve tride nenachazi zastinene vlastnosti. Ty odstrani.
	 */
	public function checkShadowing($method1) {
		foreach($this->methods as $key => $method2) {
			if(!$method2->isInherited) continue;
			if($method1->getName() != $method2->getName()) continue;
			// Kontrola zda se nejedna o pretizenou tridu
			$args1 = $method1->getArguments();	
			$args2 = $method2->getArguments();
			if(count($args1) != count($args2)) continue;
			$isSame = true;
			for($i = 0; $i < count($args1) ; $i++) {
				if($args1[$i] && $args2[$i]) {
					if($args1[$i]->getDataType() != $args2[$i]->getDataType()) $isSame = false;
				}
			}
			if($isSame) unset($this->methods[$key]);
		}	
	}

	/**
	 * @brief Prida atribut do pole atributu.
	 */
	public function addAttribute($attributeObject) {
		if($attributeObject->isInherited) {
			foreach($this->getAttributes() as $attr) {
				$isSame = true;
				if($attr->getName() != $attributeObject->getName()) $isSame = false;
				if($attr->getDataType() != $attributeObject->getDataType()) $isSame = false;
				if($attr->isInherited != $attributeObject->isInherited) $isSame = false;
				if($attr->inheritedFrom != $attributeObject->inheritedFrom) $isSame = false;
				if($attr->isStatic() != $attributeObject->isStatic()) $isSame = false;
				if($attr->lastClass != $attributeObject->lastClass) $isSame = false;
				if($isSame) return;
			}
		}


		if(!$attributeObject->isInherited) {
			// Kontrola konfliktu jmen mezi atributy ve stejne tride
			foreach($this->attributes as $attribute) {
				if(!$attribute->isInherited) {
					if($attributeObject->getName() == $attribute->getName()) exit(4);
				}
			}
			// Kontrola konfliktu jmen mezi metodami ve stejne tride
			foreach($this->methods as $method) {
				if(!$method->isInherited) {
					if($attributeObject->getName() == $method->getName()) exit(4);
			}
			}	
			// Kontrola mezi zdedenymi cleny - pokud je ve tride zdedeny prvek se stejnym jmenem je smazan
			foreach($this->attributes as $key => $attr) {
				if($attr->isInherited) {
					if($attr->getName() == $attributeObject->getName()) {
						unset($this->attributes[$key]);
					}
				}
			}
			foreach($this->methods as $key => $method) {
				$method = $this->methods[$i];
				if($method->isInherited) {
					if($method->getName() == $attributeObject->getName()) {
						unset($this->methods[$key]);
					}
				}
			}
		}
		
		array_push($this->attributes, $attributeObject);
	}

	/**
	 * @brief Pridava nadrazenou tridu. Probiha klonovani vsech metod a atributu z nadrazene tridy do dcerine (aktualni) tridy. Pri kopirovani se zmeni modifikatory, podle modifikatoru dedicnosti.
	 */
	public function addInheritance($baseClass, $modifier = "private") {
		$inheritance = new Inheritance($baseClass, $modifier);
		// Semanticka kontrola ze trida jiz neni v bazovych tridach
		// Nezavisi ani na rozdilne hodnote modifikatoru				
		if($this->inheritancesContain($baseClass->getClassName())) {
			return false;
		}
		// Kontrola ze nepridavame tridu, ktera dedi sama od sebe
		if($baseClass->getClassName() == $this->getClassName()) {
			return false;
		}

		// addInheritance
		array_push($this->inheritances, $inheritance);	
		$baseClass->addKid($this, $modifier);


		// Prekopirovani atributu a metod z bazove tridy do teto tridy
		// Pri prekopirovani se zmeni modifikator pristupu
		// Prekopirovani:
		$attributes = $baseClass->getAttributes();
		foreach($attributes as $attr) {
			$cloneAttr = clone $attr;
			if($attr->getModifier() == "private") {
				$cloneAttr->wasPrivateInParent = true;
			} else {
				$cloneAttr->wasPrivateInParent = false;
			}
			// Privatni atributy zdedime, ale nebudeme k nim pak mit pristup
			// Zmena modifikatoru:
			if($modifier == "public") {} // Zadna zmena modifikatoru bazove tridy
			if($modifier == "protected") {
				if($cloneAttr->getModifier() == "public") {
					$cloneAttr->setModifier("protected");
				}
			}
			if($modifier == "private") {
				$cloneAttr->setModifier("private");
			}
			// Zaznamenani zdedeni
			if($cloneAttr->isInherited == false) {
				$cloneAttr->isInherited = true;
				$cloneAttr->inheritedFrom = $baseClass->getClassName();
			}
			$cloneAttr->lastClass = $baseClass->getClassName();

			$this->addAttribute($cloneAttr);
		}	
		// To same udelame u metod
		// Prekopirovani:
		$methods= $baseClass->getMethods();
		foreach($methods as $method) {
			$cloneMethod = clone $method;
			// Konstruktory ani destruktory nededime
			if($method->isConstructor || $method->isDestructor) continue;
			if($method->getModifier() == "private") {
				$cloneMethod->wasPrivateInParent = true;
			} else {
				$cloneMethod->wasPrivateInParent = false;
			}
			// Privatni metody nededime, krome ciste virtualnich metod, ktere sice dedime ale neukazujeme je ve vypise - jen zde zustavaji kvuli abstrakcnosti tridy
			// privatni metody dedime
			// tyto metody nezobrazujeme pri vypise = metody privatni a zdedene
			// Zmena modifikatoru:
			if($modifier == "public") {} // Zadna zmena modifikatoru bazove tridy
			if($modifier == "protected") {
				if($cloneMethod->getModifier() == "public") {
					$cloneMethod->setModifier("protected");
				}
			}
			if($modifier == "private") {
				$cloneMethod->setModifier("private");
			}
			// Zaznamanema dedeni
			if($cloneMethod->isInherited == false) {
				$cloneMethod->isInherited = true;
				$cloneMethod->inheritedFrom = $baseClass->getClassName();
			}
			$cloneMethod->lastClass = $baseClass->getClassName();
			$this->addMethod($cloneMethod);
		}	
	
		return true;
		
	}

	/**
	 * @brief Vola se pri prirazeni vztahu mezi bazovou tridou a podtridou. Do pole kids prida podtridu.
	 */
	public function addKid($subClass, $modifier = "private") {
		if($this->kidsContain($subClass->getClassName())) {
			return false;
		}
		array_push($this->kids, $subClass);
		return true;
	}

	public function addUsing($baseClass, $identifier, $newModifier = "private") {
		foreach($this->methods as $key => $method) {
			if(!$method->isInherited) continue;
			if($method->getName() == $identifier) {
				unset($this->methods[$key]);
			}	
		}
		// Projdi vsechny bazove tridy rekurzivne a jakmile narazis na tridu se jmenem shodnym s bazovou, vyhledej v ni metodu identifier a tu nakopiruj - pretezovani metod?
		$returnMethods = $this->addUsingRecursiveMethod($baseClass->getClassName(), $identifier, $newModifier);	
		
		foreach($this->attributes as $key => $attr) {
			if(!$attr->isInherited) continue;
			if($attr->getName() == $identifier) {
				unset($this->attributes[$key]);	
			}	
		}
		$returnAttr = $this->addUsingRecursiveAttr($baseClass->getClassName(), $identifier, $newModifier);

		if($returnAttr != false && $returnAttr != "NO_PARENT") {
			$this->addAttribute($returnAttr);
		}
		if($returnMethods != false && $returnMethods != "NO_PARENT"){
			foreach($returnMethods as $method) {
				$this->addMethod($method);	
			}
		}
		// Nenaslo se nic
		if(($returnMethods == false || $returnMethods == "NO_PARENT") && ($returnAttr == false || $returnAttr == "NO_PARENT")) exit(4);
	}

	/**
	 * @brief Vyhleda metodu z nadrazenych trid, kterou naklonuje a preda do aktualni tridy.
	 * @return Objekt metody.
	 */
	public function addUsingRecursiveMethod($searchedClass, $id, $newModifier) {
		// Projdu vsechny nadrazene tridy
		// Pokud nenajdu tridu s nazvem vracim naklonovanou metodu
		// Pokud najdu tridu se stejnym nazvem, bez vlastnosti, vracim false
		foreach($this->getInheritances() as $parent) {
			$parent = $parent->getBaseClass();
			if($parent->getClassName() == $searchedClass) {
				// Spusti vyhledavaci proceduru metody
				$methodsArray = array();
				foreach($parent->getMethods() as $method) {
					if($method->getName() == $id) {
						$cloneMethod = clone $method;
						$cloneMethod->setModifier($newModifier);
						if($cloneMethod->isInherited == false) {
							$cloneMethod->isInherited = true;
							$cloneMethod->inheritedFrom = $parent->getClassName();
						}
						$cloneMethod->lastClass = $parent->getClassName();
						array_push($methodsArray, $cloneMethod);
					}	
				}
				if(count($methodsArray) > 0) {
					// Vyhledavani uspelo
					return $methodsArray;
				} else {
					// Vyhledavani neuspelo - v dane tride neni dana metoda
					//exit(4);
					return false;
				}
			}
		}
		// Spust rekurzivne nad vsemi dedicnostmi danou tridou
		foreach($this->getInheritances() as $parent) {
			$parent = $parent->getBaseClass();
			$valueFromRecursion =  $parent->addUsingRecursiveMethod($searchedClass, $id, $newModifier);
			if($valueFromRecursion == "NO_PARENT") {
				continue;
			} else {
				return $valueFromRecursion;
			}
		}

		// Vrat pokud trida jiz nema zadnou dedicnost
		return "NO_PARENT";
			
	}

	/**
	 * @brief Vyhleda atribut z nadrazenych trid.
	 */
	public function addUsingRecursiveAttr($searchedClass, $id, $newModifier) {
		foreach($this->getInheritances() as $parent) {
			$parent = $parent->getBaseClass();
			if($parent->getClassName() == $searchedClass) {
				// Spusti vyhledavaci proceduru metody
				foreach($parent->getAttributes() as $attr) {
					if($attr->getName() == $id) {
						$cloneAttr = clone $attr;
						$cloneAttr->setModifier($newModifier);
						// Zaznamenani zdedeni
						if($cloneAttr->isInherited == false) {
							$cloneAttr->isInherited = true;
							$cloneAttr->inheritedFrom = $parent->getClassName();
						}
						$cloneAttr->lastClass = $parent->getClassName();
						// Vyhledavani uspelo
						return $cloneAttr;
					}	
				}
				// Vyhledavani neuspelo
				//exit(4);
				return false;
			}
		}
		// Spust rekurzivne nad danou tridou
		foreach($this->getInheritances() as $parent) {
			$parent = $parent->getBaseClass();
			$valueFromRecursion = $parent->addUsingRecursiveAttr($searchedClass, $id, $newModifier);
			if($valueFromRecursion == "NO_PARENT") {
				continue;
			} else {
				return $valueFromRecursion;
			}
		}

		return "NO_PARENT";

	}
}

/**
 * @brief Objekt dedicnosti. Vznikl aby uchoval data o modifikatoru dedicnosti.
 */
class Inheritance {
	
	private $baseClass;
	private $modifier;

	public function __construct($classObject, $modifier) {
		$this->baseClass = $classObject;
		$this->modifier = $modifier;
	}

	public function getBaseClass() {
		return $this->baseClass;
	}

	public function getModifier() {
		return $this->modifier;
	}

}

/**
 * @brief Trida predstavujici metodu tridy.
 */
class MethodObject {
	private $identifier;
	public $isMethod = true;
	public $isAttribute = false;
	public $wasPrivateInParent = false;
	public $isInherited = false;
	public $inheritedFrom;
	public $lastClass = false;
	private $args = array();
	private $returnType;
	private $modifier = "private";
	private $isStatic = false;
	private $isVirtual = false;
	private $isPureVirtual = false;
	public $isConstructor = false;
	public $isDestructor = false;

	public function getName() {
		return $this->identifier;
	}

	public function getModifier() {
		return $this->modifier;
	}

	public function setModifier($modifier) {
		$this->modifier = $modifier;
	}

	public function getReturnType() {
		return $this->returnType;
	}

	public function isStatic() {
		return ($this->isStatic ? true : false);
	}

	public function isVirtual() {
		return ($this->isVirtual ? true : false);
	}

	public function __construct($methodIdentifier = "", $returnType, $modifier = "private", $isStatic, $isVirtual, $isConstructor = false, $isDestructor = false) {
		$this->identifier = $methodIdentifier;
		$this->returnType = $returnType;
		$this->modifier = $modifier;
		$this->isStatic = $isStatic;
		$this->isVirtual = $isVirtual;
		$this->isConstructor = $isConstructor;
		$this->isDestructor = $isDestructor;
	}

	/**
	 * @brief Pridava argument metode. Pri pridani kontroluje jestli metoda neobsahuje argument se stejnym jmenem.
	 */
	public function addArgument($argument) {
		// Semanticka kontrola argument jiz deklarovanych
		foreach($this->args as $arg) {
			if($arg->getName() == $argument->getName()) exit(4);
		}
		array_push($this->args, $argument);
	}

	public function getArguments() {
		return $this->args;
	}

	public function setPureVirtual($isPureVirtual) {
		if($this->isVirtual == false) exit(4);
		$this->isPureVirtual = $isPureVirtual;
	}
	public function isPureVirtual() {
		return $this->isPureVirtual;
	}
}

/**
 * @brief Trida predstavujici argument metody
 */
class MethodArgument {
	private $dataType;
	private $identifier;
	public function __construct($identifier, $dataType) {
		$this->dataType = $dataType;
		$this->identifier = $identifier;
	}
	public function getDataType() {
		return $this->dataType;
	}
	public function getName() {
		return $this->identifier;
	}
}

/**
 * @brief Trida popisujici atribut tridy.
 */
class Attribute {
	private $dataType;
	private $identifier;
	private $modifier = "private";
	private $isStatic;
	public $isInherite = false;
	public $inheritedFrom;
	public $lastClass = false;
	public $wasPrivateInParent = false;
	public $isAttribute = true;
	public $isMethod = false;

	public function getModifier() {
		return $this->modifier;
	}
	public function setModifier($modifier) {
		$this->modifier = $modifier;
	}
	public function getName() {
		return $this->identifier;
	}
	public function getDataType() {
		return $this->dataType;
	}
	/**
	 * @brief Skontroluje zda je atribut statis
	 */
	public function isStatic() {
		if($this->isStatic) {
			return true;
		} else {
			return false;
		}
	}

	public function __construct($identifier, $dataType, $modifier = "private", $isStatic) {
		$this->identifier = $identifier;
		$this->dataType = $dataType;
		$this->modifier = $modifier;
		$this->isStatic = $isStatic;
	}
}

?>

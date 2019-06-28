import imp
import sys
import argparse
import os
import xml.etree.ElementTree as ET 
import xml.dom.minidom as dom
import re
# Importovani csv souboru 
f, pathname, desc = imp.find_module("csv", sys.path[1:])
csv = imp.load_module("csv", f, pathname, desc)


# Importovani html parseru ktery pouzijeme na prevedeni entit z ET objektu
from html.parser import HTMLParser
htmlparser = HTMLParser()

# Konec importu

# Pomocne funkce
def is_valid(ETObject):
	""" Kontroluje zda Element Tree objekt vytvari validni XML objekt """
	try:
		dom.parseString(htmlparser.unescape(ET.tostring(root).decode("utf-8")))
	except:
		# Objekt je nevalidni
		return False
	# Objekt je validni
	return True

def is_valid_xml_name(name):
	""" Kontroluje zda string name tvori validni XML jmeno """
	pattern = re.compile("^(?!XML)(?!\d)([\w]|\_)[\w\-_.]*$", re.IGNORECASE)
	return pattern.match(name)

def replace_chars(string, replaceChar = "-"):
	""" Nahradi nechtene znaky za replaceChar """
	string = list(string)
	for i, znak in enumerate(string):
		ordValue = ord(znak)
		if not 65 <= ordValue <= 90 and not 97 <= ordValue <= 122 and not 48 <= ordValue <= 57 and not ordValue == 45 and not ordValue == 46 and not ordValue == 95 and not ordValue >= 128:
			string[i] = replaceChar
	string = "".join(string)
	return string

# Zpracovani argumentu
parser = argparse.ArgumentParser(prog="CSV", add_help = False)

# Zakladni parametry
parser.add_argument("--input", help = "Adresa vstupního souboru.")
parser.add_argument("--output", help = "Adresa výstupního souboru.")

# Pokrocile parametry
parser.add_argument("-n", action = "store_true", help = "Vymazat XML hlavičku z výstupu.") 
parser.add_argument("-r", help = "Kořenový tag obalující výsledek.")
parser.add_argument("-s", default = ",", help = "Separátor v cvs souboru.")
parser.add_argument("-h", help = "Výplň v jednotlivých názvech tagů v řádku")
parser.add_argument("-c", default = "col", help = "Jméno elementů v řadku s indexem.")

parser.add_argument("-l", help = "Jméno řádkového elementu.")
parser.add_argument("-i", action = "store_true",  help = "Index řádkového elementu.")
parser.add_argument("--start", type = int, help = "Počáteční hodnota čítače řádků.")

parser.add_argument("-e", "--error-recovery", action = "store_true", help = "Zotavení se ze špatného počtu sloupců.")
parser.add_argument("--missing-field", help = "Hodnota chybějícího sloupce.")
parser.add_argument("--all-columns", action = "store_true", help = "Do výstupu přidá i sloupce, které jsou navíc.")
parser.add_argument("--help", action = "store_true", help = "Vytiskne nápovědu k tomuto programu.")

# Odstraneni volby -h z pole argumentu predavanych skriptu
useCSVHeader = True
try:
	sys.argv.remove("-h")
except:
	# -h volba nebyla zadana
	useCSVHeader = False 

# Parsovani vstupnich argumentu argv
try:
	args = parser.parse_args()
except SystemExit:
	# Pokud pri parsovani dojde k chybe, vracime hodnotu 1 podle zadani
	# Jinak bychom vraceli hodnotu 2, defaultne implementovanou v modulu argparse
	exit(1)

# Prevedeni objektu Namespace na dict
args = vars(args)

# Zpracovani hodnot argumentu
# Nacteni vstupniho souboru
# Pri neuspechu ukoncime program s navratovym kodem 2

# Zpracovani help argumentu
if(args["help"] is True):
	# Kontrola ze byl zadan pouze tento parametr
	if(len(sys.argv) is 2):
		parser.print_help()	
		exit(0)
	else:
		sys.stderr.write("Byly zadany spatne parametry. S parametrem --help nelze kombinovat dalsi parametry.\n")
		exit(1)


# Kontrola input a output argumentu
if(args["input"] is None or args["output"] is None):
	sys.stderr.write("Chybi argument input nebo output.\n")
	exit(1)

# Kontrola zda soubor nechybi
if(os.path.isfile(args["input"])):
	
	try:
		csvFile = open(args["input"], "r")
	except IOError:
		sys.stderr.write("Vstupni soubor se nepodarilo otervir.\n")
		exit(2)

else:
	# Vstupni soubor neexistuje
	sys.stderr.write("Vstupni soubor neexistuje.\n")
	exit(2)

# Kontrola spravne kombinace parametru
# Kombinace -l, -i, --start
if(args["i"] is not False and args["l"] is None):
	sys.stderr.write("Parametr -i se musi zkombinovat s parametrem -l.\n")
	exit(1)

if(args["start"] is not None and args["i"] is False):
	sys.stderr.write("Parametr start se musi zkombinovat s parametrem -i a -l.\n")
	exit(1)

# Kombinace -e, --missing-field
if(args["missing_field"] is not None and args["error_recovery"] is False):
	sys.stderr.write("Parametr --missing-field se musi zkombinovat s parametrem -e.\n")
	exit(1)
# Kombinace -e, --all-columns
if(args["all_columns"] is not False and args["error_recovery"] is False):
	sys.stderr.write("Parametr all-columns se musi zkombinovat s parametrem -e.\n")
	exit(1)

# Nastaveni defaultnich hodnot argumentu
# Hodnoty se musi nastavit zde, jinak bychom nevedeli jestli je zadal uzivatel nebo modul argparse. To je potrebne predevsim pro testy vyse. Predevsim u testu spravne kombinace parametru.
if(args["l"] is None):
	args["l"] = "row"
if(args["start"] is None):
	args["start"] = 1
if(args["missing_field"] is None):
	args["missing_field"] = ""



# Kontrola kombinace argumentu -h=XXX a volby -h
if(useCSVHeader and args["h"] is not None):
	sys.stderr.write("Nelze kombinovat volbu -h a argument -h=XXX.\n")
	exit(1)
# Nastaveni -h volby - ta muze byt do skriptu byt zadana jako volba nebo jako argument
if(useCSVHeader):
	args["h"] = "-"

# Nastaveni separatoru
if(args["s"] == "\\t"):
	args["s"] = '\t'

# Zparsovani vstupniho souboru
csvRows = csv.reader(csvFile, delimiter = args["s"], quotechar = '"')

# Vytvoreni xml objektu
if(args["r"] is not None):
	root = ET.Element(args["r"])
	# Kontrola validity xml objektu
	if(is_valid_xml_name(args["r"]) is None):
		sys.stderr.write("Korenovy prvek elementu je nevalidni.\n")
		exit(30)

else:
	root = ET.Element("root")

# Iterace napric radky csv souboru
numberOfCols = 0
tags = []
for row_index, row in enumerate(csvRows):
	if(row_index is 0):
		# Zjisteni poctu sloupcu
		numberOfCols = len(row)
		if(args["h"] is not None):
			# Zpracovani hlavicky csv souboru - prvni radek
			for col_index, col in enumerate(row):
				# Nahrazeni nechtenych znaku
				col = replace_chars(col, args["h"])
				# Kontrola validity jmena
				if(is_valid_xml_name(col) is None):
					sys.stderr.write("Jmeno sloupce je nevalidni.\n")
					exit(31)
				# Pridani do pole hlavicek
				tags.append(col)
			# Preskoc tento radek - jednalo se o hlavicky
			continue

	# Pridavani ostatnich radku
	# Kontrola validity jmena radku
	if(is_valid_xml_name(args["l"]) is None):
		sys.stderr.write("Jmeno radku je nevalidni.\n")
		exit(30)

	rowElement = ET.SubElement(root, args["l"])

	# Zavedeni parametru -i
	if(args["i"] is not False):
		index = row_index + args["start"] 
		if(args["h"] is not None):
			index -= 1	
		rowElement.set("index", str(index))

	# Kontrola spravneho poctu sloupcu
	if numberOfCols is not len(row) and args["error_recovery"] is False:
		sys.stderr.write("Spatny pocet sloupcu na radku.\n")
		exit(32)
	# Pocet sloupcu je mensi nez pocet sloupcu v hlavicce
	while len(row) < numberOfCols:
		row.append(args["missing_field"])
	# Pocet sloupcu je vetsi nez pocet sloupcu v hlavicce
	while len(row) > numberOfCols and args["all_columns"] is False:
		row.pop()

	#  Iterace napric sloupci v csv souboru
	for col_index, col in enumerate(row):
		# Pridani sloupce
		if(args["h"] is not None and col_index < numberOfCols):
			# Sloupce maji hodnotu z hlavicky xml
			colElement = ET.SubElement(rowElement, tags[col_index]) 
		else:
			# Sloupce maji hodnotu podle argumentu -c
			# Validace jmena elementu
			elName = args["c"] + str(col_index + 1)
			if(is_valid_xml_name(elName) is None):
				sys.stderr.write("Jmeno sloupce je nevalidni.\n")
				exit(30)
			colElement = ET.SubElement(rowElement, elName)
		# Pridani obsahu sloupce
		colElement.text = col



# Finalni kontrola validity xml objektu
if(is_valid(root) is False):
	sys.stderr.write("Element je nevalidni.\n")
	exit(30)

# Ziskani xml retezce
# Retezec musime prevest zpet z utf-8 kodovani
xmlString = dom.parseString(htmlparser.unescape(ET.tostring(root).decode("utf-8"))).toprettyxml(indent = "    ", encoding = "UTF-8").decode("utf-8")

# Zpracovani argumentu -n - odstraneni prvniho radku z xml
if(args["n"] is True):
	xmlString = "\n".join(xmlString.splitlines()[1:])
	# Pridani koncoveho radku
	xmlString += "\n"

# Odstraneni root tagu -> odstranime prvni a posledni radek
# Pokud se na prvnim radku nachazi XML hlavicka odstranime druhy radek
if(args["r"] is None):
	# Odstraeni posledniho radku
	xmlString = "\n".join(xmlString.splitlines()[:-1])
	if(args["n"] is True):
		# Odstraneni prvniho radku	
		xmlString = "\n".join(xmlString.splitlines()[1:]) + "\n"
	else:
		# Odstraneni druheho radku		
		xmlString = "\n".join(xmlString.splitlines()[0:1] + xmlString.splitlines()[2:]) + "\n"

# Prevedeni retezce zpet do utf-8 kodovani
xmlString = xmlString.encode("utf-8")

# Otevreni vystupniho souboru
outputFile = open(args["output"], "wb")

# Vypsani XML Objektu do vystupniho souboru
outputFile.write(xmlString)


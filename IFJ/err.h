/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * err.h
 * 
 * Antonin Mazanek,		xmazan07
 */


// navratova hodnota
extern int errCode;

/*
0 - vse v poradku
1 - chyba v programu v ramci lexikalni analyzy (chybna struktura aktualniho lexemu).
2 - chyba v programu v ramci syntaktické analyzy (chybna syntaxe programu).
3 - semanticka chyba v programu – nedefinovana trida/funkce/promenna, 
	pokus o re-definici tridy/funkce/promenne, atd.
4 - semanticka chyba typove kompatibility v aritmetickych, retezcovych
	a relacnich vyrazech, prip. spatny pocet ci typ parametru u volani funkce.
6 - ostatni semanticke chyby.
7 - behova chyba pri nacitani ciselne hodnoty ze vstupu.
8 - behova chyba pri praci s neinicializovanou promennou.
9 - behova chyba deleni nulou.
10 - ostatni behove chyby.
99 - interni chyba interpretu tj. neovlivnena vstupnim programem (napr. chyba alokace
	 pameti, chyba pri otvirani souboru s ridicim programem, spatne parametry
	 prikazove radky atd.).
 */


/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * syntax_ll.h
 * @brief			Hlavickovy soubor obsahujici definice funkci pro syntakticky analyzator.
 * 
 * Monika Muzikovska,	xmuzik05
 * Dominik Turecek,		xturec06
 * Stepan Vich,			xvichs00
 */

#ifndef SYNTAX_LL_H
#define SYNTAX_LL_H

#include <stdbool.h>
#include <string.h>
#include "lexikalni_analyzator.h"
#include "ial.h"
#include "syntax_expr.h"
#include "interpret.h"
#include "err.h"

/**
 * Vyctovy typ pro navraceni informace o hledani v TS nebo vytvareni polozky v TS
 */
enum{ ERR_MATCHING,
      ERR_NOT_FOUND,
      CREATED,
      FOUND_LOCAL,
      FOUND_GLOBAL,
      PROGRAM_ERROR
};

/**
 * Vytvori zaznam v tabulce symbolu
 * @param root ukazatel na koren stomu (tabulka symbolu)
 * @param item vkladana data
 * @param data ukazatel na vytvorena data
 * @return hodnotu z enum ERR_MATCHING,...
 */
int CREATE(tBTNodePtr *root, struct tSymTable item, tSymTablePtr* data);

/**
 * Vyhledava v globalni a lokalni tabulce
 * @param tk token jehoz jmeno se hleda v tabulkach
 * @param className jmeno tridy
 * @param lok lokalni tabulka symbolu
 * @param glob globalni tabulka symbolu
 * @param data ukazatel na data nalezeneho klice
 * @return hodnotu z enum ERR_MATCHING,...
 */
int FIND(Token *tk, char *className, tBTNodePtr lok, tBTNodePtr glob, tSymTablePtr *data);

/**
 * Vola jednotlive funkce pro vlozeni zaznamu o vestavenych funkcich
 * @return True v pripade, ze se podarilo korektne vytvorit zaznamy o vsech funkcich, jinak false
 */
bool insertFunctions();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.readInt
 * @return povedlo-li se vytvorit vraci true, jinak false
 */
bool insertReadInt();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.readDouble
 * @return povedlo-li se vytvorit vraci true, jinak false
 */
bool insertReadDouble();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.readString
 * @return povedlo-li se vytvorit vraci true, jinak false
 */
bool insertReadString();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.print
 * @return povedlo-li se vytvorit vraci true, jinak false
 */
bool insertPrint();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.length
 * @return povedlo-li se vytvorit vraci true, jinak false
 */
bool insertLength();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.substr
 * @return povedlo-li se vytvorit vraci true, jinak false
 */
bool insertSubstr();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.compare
 * @return povedlo-li se vytvorit vraci true, jinak false
 */
bool insertCompare();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.find
 * @return povedlo-li se vytvorit vraci true, jinak false
 */
bool insertFind();

/**
 * Vytvari v tabulce symbolu zaznam pro funkci ifj16.sort
 * @return nenastane-li chyba vraci true, jinak false
 */
bool insertSort();

// Funkce reprezentujici neterminaly v pravidlech, provadi pravidla dle LL gramatiky
/**
 * Reprezentuje neterminal <start>
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool START(int pruchod);

/**
 * Reprezentuje neterminal <classes>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param classes list jmen trid
 * @return nenastane-li chyba vraci true, jinak false
 */
bool CLASSES(TokenPtr* tk, int pruchod, tClassNameListPtr classes);

/**
 * Reprezentuje neterminal <class>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param classes list jmen trid
 * @return nenastane-li chyba vraci true, jinak false
 */
bool CLASS(TokenPtr* tk, int pruchod, tClassNameListPtr classes);

/**
 * Reprezentuje neterminal <definice>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param class_name nazev aktualni tridy
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DEFINICE(TokenPtr* tk, int pruchod, char* class_name);

/**
 * Reprezentuje neterminal <jedna_definice>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param class_name nazev aktualni tridy
 * @return nenastane-li chyba vraci true, jinak false
 */
bool JEDNA_DEFINICE(TokenPtr* tk, int pruchod, char* class_name);

/**
 * Reprezentuje neterminal <jedna_definice_2>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param class_name nazev aktualni tridy
 * @return nenastane-li chyba vraci true, jinak false
 */
bool JEDNA_DEFINICE_2(TokenPtr* tk, int pruchod, char* class_name);

/**
 * Reprezentuje neterminal <jedna_definice_3>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param pkid plne kvalifikovany identifikator
 * @param data_type posledni zjisteny datovy typ
 * @return nenastane-li chyba vraci true, jinak false
 */
bool JEDNA_DEFINICE_3(TokenPtr* tk, int pruchod, char* pkid, int data_type);

/**
 * Reprezentuje neterminal <jedna_definice_4>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param pkid plne kvalifikovany identifikator
 * @param data_type datovy typ
 * @return nenastane-li chyba vraci true, jinak false
 */
bool JEDNA_DEFINICE_4(TokenPtr* tk, int pruchod, char* pkid, int data_type);

/**
 * Reprezentuje neterminal <id>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool ID(TokenPtr* tk, int pruchod, tSymTablePtr* data);

/**
 * Reprezentuje neterminal <definice_promenne>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DEFINICE_PROMENNE(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <typ>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data_type ukazatel na datovy typ, kam se ulozi aktualne ten zpracovavany 
 * @return nenastane-li chyba vraci true, jinak false
 */
bool TYP(TokenPtr* tk, int pruchod, int* data_type);

/**
 * Reprezentuje neterminal <seznam_parametru>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool SEZNAM_PARAMETRU(TokenPtr* tk, int pruchod, tSymTablePtr data);

/**
 * Reprezentuje neterminal <def_parametru>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DEF_PARAMETRU(TokenPtr* tk, int pruchod, tSymTablePtr data);

/**
 * Reprezentuje neterminal <def_paramtru_2>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DEF_PARAMETRU_2(TokenPtr* tk, int pruchod, tSymTablePtr data);

/**
 * Reprezentuje neterminal <jedna_def_parametru>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool JEDNA_DEF_PARAMETRU(TokenPtr* tk, int pruchod, tSymTablePtr data);

/**
 * Reprezentuje neterminal <prikazy>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool PRIKAZY(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <prikaz>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool PRIKAZ(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <prirazeni>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool PRIRAZENI(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <dilci_prikaz>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DILCI_PRIKAZ(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <dilci<_prikaz_2>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DILCI_PRIKAZ_2(TokenPtr* tk, int pruchod, tSymTablePtr data);

/**
 * Reprezentuje neterminal <dilci_prikazy>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DILCI_PRIKAZY(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <vyvolani_funkce>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool VYVOLANI_FUNKCE(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <seznam_vstupnich_parametru>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool SEZNAM_VSTUPNICH_PARAMETRU(TokenPtr* tk, int pruchod, tSymTablePtr data);

/**
 * Reprezentuje neterminal <vstupni_parametr>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool VSTUPNI_PARAMETR(TokenPtr* tk, int pruchod, tSymTablePtr data);

/**
 * Reprezentuje neterminal <dalsi_vstupni_parametr>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @param data ukazatel na data v tabulce symbolu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DALSI_VSTUPNI_PARAMETR(TokenPtr* tk, int pruchod, tSymTablePtr data);

/**
 * Reprezentuje neterminal <term>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool TERM(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <vyraz_navratu>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool VYRAZ_NAVRATU(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <lokalni_promena>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool LOKALNI_PROMENNA(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <term_konkatenace>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool TERM_KONKATENACE(TokenPtr* tk, int pruchod);

/**
 * Reprezentuje neterminal <dalsi_term>
 * @param tk aktualne zpracovavany token
 * @param pruchod cislo pruchodu
 * @return nenastane-li chyba vraci true, jinak false
 */
bool DALSI_TERM(TokenPtr* tk, int pruchod);

/**
 * Vytvari unikatni nazev navesti
 * @return unikatni nazev
 */
char * uniqueLabel();

typedef struct aktualniData {
	char * class_name;
	char * pkid;
	char * f_assign_to;
	char * function_name;
	int data_type;
	int return_type;
	int expr_type;
} aktualniData;

#endif

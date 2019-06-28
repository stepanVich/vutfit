/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * ial.h
 *
 * Antonin Mazanek,     xmazan07
 */

#ifndef IAL_H
#define IAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "built_in_functions.h"

/**
 * Union pro konstanty
 */
typedef union {
    int cele_cislo;
    double desetinne_cislo;
    char *retezec;
    bool prav_hodnota;
} uVal;

/**
 * Vyctovy typ pro datove typy
 */
enum { DATA_TYPE_VOID,
       DATA_TYPE_INT,
       DATA_TYPE_DOUBLE,
       DATA_TYPE_STRING,
       DATA_TYPE_BOOL};

/**
 * Vyctovy typ pro typ zaznamu v tabulce
 */
enum { TABLE_FUNCTION,
       TABLE_VARIABLE};

// ----- zaznam -----
/**
* Struktura pro polozku seznamu parametru
*/
typedef struct tParamElem {
   int type; /**< datovy typ parametru*/
   char *name; /**< nazev parametru*/
   int index; /**<index parametru*/
   struct tParamElem *next; /**< ukazatel na dalsi prvek seznamu*/
} *tParamElemPtr;

/**
* Struktura pro seznam parametru
*/
typedef struct tParamList{
   tParamElemPtr First; /**< ukazatel na prvni prvek seznamu*/
   tParamElemPtr Last; /**< ukazatel na posledni prvek seznamu*/
} *tParamListPtr;

/**
* Struktura pro polozku seznamu nazvu trid
*/
typedef struct tClassNameElem {
   int type; /**< datovy typ konstanty*/
   char* name; /**< nazev tridy*/
   struct tClassNameElem *next; /**< ukazatel na dalsi prvek seznamu*/
} *tClassNameElemPtr;

/**
* Struktura pro seznam nazvu trid
*/
typedef struct tClassNameList{
   tClassNameElemPtr First; /**< ukazatel na prvni prvek seznamu*/
   tClassNameElemPtr Last; /**< ukazatel na posledni prvek seznamu*/
} *tClassNameListPtr;

// ----- tabulka symbolu -----
/**
 * Struktura pro ukladani informaci o funkci
 */
typedef struct dataFunction {
    int return_type; /**< navratovy typ*/
    int no_params; /**< pocet parametru*/
    tParamListPtr params; /**< list parametru*/
} *dataFunctionPtr;

/**
 * Struktura pro ukladani informaci o promenne
 */
typedef struct dataVariable {
    int data_type; /**< datovy typ promenne*/
    bool init; /**< promenna inicializovana [interpret]*/
    uVal value; /**< hodnota promenne [interpret]*/
} *dataVariablePtr;

/**
 * Struktura pro typ do tabulky symbolu
 */
typedef struct tSymTable {
    int type; /**< typ - funkce, promenna, nedefinovano*/
    char* name; /**< nazev identifikatoru v programu*/
    dataFunctionPtr function; /**< typ = funkce*/
    dataVariablePtr variable; /**< typ = promenna*/
} *tSymTablePtr;

// ----- binarni vyhledavaci strom -----
/**
 * Struktura pro polozky tabulky symbolu (uzely binarniho stromu)
 */
typedef struct tBTNode {
    char *key; /**< klic TODO*/
    tSymTablePtr data; /**< data tabulky symbolu*/
    struct tBTNode *LPtr; /**< ukazatel na levy podstrom*/
    struct tBTNode *RPtr; /**< ukazatel na pravy podstrom*/
} *tBTNodePtr;

// ----- zasobnik -----
/**
 * Struktura pro dynamicky alokovany zasobnik
 */
typedef struct {
    int top; /**< pocet prvku (-1)->prazdny*/
    int max; /**< max. pocet prvku (naalokovana pamet)*/
    tBTNodePtr *a; /**< dynamicke pole ukazatelu*/
} tStack;

/**
 * Inicializuje binarniho stromu
 * @param rootPtr ukazatel na na ukazatel koren stromu
 */
void BTInit (tBTNodePtr *rootPtr);

/**
 * Vytvori novy list v binarnim strome
 * @param rootPtr ukazatel na ukazatel na koren stromu
 * @param key klic
 * @param data ukazatel na vkladana data
 * @return ukazatel na vlozena data (pri chybe NULL)
 */
tSymTablePtr BTInsert (tBTNodePtr *rootPtr, char *key, struct tSymTable data);

/**
 * Vyhleda polozku v binarnim strome podle klice
 * @param root ukazatel na koren stromu
 * @param key klic
 * @param data ukazatel na data nalezene polozky (nebo NULL)
 * @return je-li nalezen zaznam s danym klicem vraci true, jinak false
 */
bool BTSearch (tBTNodePtr root, char *key, tSymTablePtr *data);

/**
 * Rusi strom a nastavuje jej do stavu po inicializaci
 * @param rootPtr ukazatel na ukazatel na koren stromu
 */
void BTDispose (tBTNodePtr *rootPtr);

// ----- zasobnik -----
/**
 * Inicializuje zasobnik. Alokuje misto pro 10 prvku.
 * @param S zasobnik
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SInit (tStack *S);

/**
 * Zvetsuje zasobnik (na dvojnasobek sve velikosti)
 * @param S zasobnik
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SAddSpace (tStack *S);

/**
 * Vklada data na vrchol zasobniku
 * @param S zasobnik
 * @param data hodnota
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SPush (tStack *S, tBTNodePtr data);

/**
 * Odstranuje prvek z vrcholu zasobniku a vraceni jeho hodnotu
 * @param S zasobnik
 * @return Vraci prvek na vrcholu zasobniku
 */
tBTNodePtr STopPop (tStack *S);

/**
 * Zjistuje prazdnotu zasobniku
 * @param S zasobnik
 * @return Je-li zasobnik prazdny vraci true, jinak false
 */
bool SEmpty (tStack *S);

/**
 * Uvolnuje alokovanou pamet pro zasobnik
 * @param S zasobnik
 */
void SFree (tStack *S);

// ----- seznam -----
/**
 * Inicializuje seznam parametru
 * @param L list
 */
void InitParamList (tParamListPtr L);

/**
 * Zusi seznam parametru a nastavuje jej do stavu po inicalizaci
 * @param L list
 */
void DisposeParamList (tParamListPtr L);

/**
 * Vklada parametr na konec seznamu parametru
 * @param L list
 * @param type typ vkladaneho parametru
 * @param name jmeno parametru
 * @param index index parametru
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool InsertLastParam (tParamListPtr L, int type, char *name, int index);

/**
 * Hleda v seznamu parametru polozku s danym indexem
 * @param L list
 * @param index hledany index
 * @return Vraci nalezenou polozku, nenalezne-li vraci NULL
 */
tParamElemPtr SearchParamList (tParamListPtr L, int index);

/**
 * Inicializuje seznam nazvu trid
 * @param L list
 */
void InitClassNameList (tClassNameListPtr L);

/**
 * Zusi seznam nazvu trid a nastavuje jej do stavu po inicalizaci
 * @param L list
 */
void DisposeClassNameList (tClassNameListPtr L);

/**
 * Vklada dalsi nazev na konec seznamu nazvu trid
 * @param L list
 * @param name nazev tridy
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool InsertLastClassName (tClassNameListPtr L, char* name);

/**
 * Hleda zadany nazev v zadanem liste nazvu trid
 * @param L liste
 * @param name hledany nazev
 * @return Nalezne-li dany nazev vraci true, jinak false
 */
bool isInClassNameList (tClassNameListPtr L, char *name);

// ----- vestavene funkce v ramci IAL -----
/**
 * Seradi znaky v retezci vzestupne dle ordinalni hodnoty
 * (varianta 3: Shell sort)
 * @param s retezec k serazeni
 * @return serazeny retezec
 */
char* sort (char* s);

/**
 * Najde podretezec v retezci
 * (varianta b: Boyer-Mooreuv algoritmus)
 * @param s retezec
 * @param search podretezec
 * @return pozici zacatku podretezce (nenalezne-li, pak -1)
 */
int find (char *s, char *search);

#endif // IAL_H

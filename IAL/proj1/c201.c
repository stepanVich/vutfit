
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2016
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
** 
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu před prvním použitím,
**      DisposeList ... zrušení všech prvků seznamu,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int solved;
int errflg;

void Error() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

 // Nastaveni prvotnich hodnot struktury
 // Zpocatku vsechny maji hodnotu NULL
 L->Act = NULL;
 L->First = NULL;
 // Memory allocation? - nope - pouze u elementu seznamu
}

void DisposeList (tList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
** uvolněna voláním operace free.
***/
 while( L->First ) {
  tElemPtr helpPointer = L->First;
  L->First = L->First->ptr;
  if(helpPointer != NULL)
   free(helpPointer);
 } 
 // Nastavi list do stavu po inicializaci;
 L->Act = NULL;
 L->First = NULL;
}

void InsertFirst (tList *L, int val) {
/*
** Vloží prvek s hodnotou val na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci Error().
**/

// Jedina operace, ktera muze vlozit prvek do prazdneho seznamu!!
// Alokace pameti pro prvek seznamu    
 tElemPtr element = (tElemPtr)malloc(sizeof(struct tElem));
 if(element == NULL) { 
  Error();
 } else {
  element->data = val;
  element->ptr = L->First;
  L->First = element;
 }
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný příkaz, aniž byste testovali,
** zda je seznam L prázdný.
**/
	
 L->Act = L->First;
}

void CopyFirst (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
 if( !(L->First) ) {
  Error();
 } else {
  *val =  L->First->data; 
 }
}

void DeleteFirst (tList *L) {
/*
** Zruší první prvek seznamu L a uvolní jím používanou paměť.
** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
 if( L->First ) {
  // Zajistit ze aktivita seznamu se ztrati 
  if( L->First == L->Act ) L->Act = NULL;
  tElemPtr helpPointer = L->First;
  L->First = L->First->ptr;
  free(helpPointer);
 }  
}

void PostDelete (tList *L) {
/* 
** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se neděje.
**/

 if( L->Act ) {
  if( L->Act->ptr) {
   tElemPtr helpPointer = L->Act->ptr->ptr;
   free(L->Act->ptr);
   L->Act->ptr = helpPointer;
  }
 }
}

void PostInsert (tList *L, int val) {
/*
** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje!
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** zavolá funkci Error().
**/
 if(L->Act) {
  tElemPtr element = (tElemPtr)malloc(sizeof(struct tElem));
  if(element == NULL) { 
   Error();
  } else {
   element->data = val;
   element->ptr = L->Act->ptr; 
   L->Act->ptr = element;
  }
 }
}

void Copy (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
 if(!(L->Act)) {
  Error();
 } else {
  *val = L->Act->data;
 }
}

void Actualize (tList *L, int val) {
/*
** Přepíše data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedělá nic!
**/
 if(L->Act) {
  L->Act->data = val; 
 }
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud není předaný seznam L aktivní, nedělá funkce nic.
**/
 if( L->Act ) {
  L->Act = L->Act->ptr;
 }
}

int Active (tList *L) {		
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním příkazem return. 
**/
 return (L->Act == NULL ? 0 : 1); // Pouzit ternarni operator
}

/* Konec c201.c */

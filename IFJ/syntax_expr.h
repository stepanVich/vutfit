/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * syntax_expr.h
 *
 * Antonin Mazanek,     xmazan07
 */

#ifndef SYNTAX_EXPR
#define SYNTAX_EXPR

#include <stdio.h>
#include <stdbool.h>

#include "lexikalni_analyzator.h"
#include "syntax_ll.h"
#include "interpret.h"
#include "ial.h"

typedef enum {
    S, // shift '<'  (0)
    R, // reduce '>' (1)
    E, // equals '=' (2)
    F, // fault ' '  (3)
} char_prec_tabulka;

// Item type ... index do tabulky
enum { IT_ADD,      // '+'  (0)
       IT_SUB,      // '-'  (1)
       IT_MUL,      // '*'  (2)
       IT_DIV,      // '/'  (3)
       IT_EQ,       // '==' (4)
       IT_NOT_EQ,   // '!=' (5)
       IT_LESS,     // '<'  (6)
       IT_LESS_EQ,  // '<=' (7)
       IT_MORE,     // '>'  (8)
       IT_MORE_EQ,  // '>=' (9)
       IT_L_BRAC,   // '('  (10)
       IT_R_BRAC,   // ')'  (11)
       IT_ID,       // 'id' (12)
       IT_END,      // '$'  (13)
       IT_NETERMINAL // 'E' (14)
};

/**
 * Struktura pro (ne)terminaly analyzy zdola nahoru
 */
typedef struct tItemExpr {
    char *name; /**< nazev v kodu*/
    int data_type; /**< datovy typ*/
    bool pkid; /**< je-li pkid => true*/
    bool konst; /**< konstanty = true*/
    uVal konstData; /**< data pro konstanty*/
    int type; /**< typ (ne)terminalu (typ operatoru, id, $)*/
    bool stop; /**< zarazka "<" */
} *tItemExprPtr;

/**
 * Struktura pro zasobnik na analyzu zdola nahoru
 */
typedef struct {
    int top; /**< pocet prvku (-1)->prazdny*/
    int max; /**< max. pocet prvku (naalokovana pamet)*/
    tItemExprPtr *a; /**< dynamicke pole (ne)terminalu*/
} tStackExpr;

/**
 * Inicializuje zasobnik. Alokuje misto pro 10 prvku.
 * @param S zasobnik
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SExprInit (tStackExpr *S);

/**
 * Zvetsuje zasobnik (na dvojnasobek sve velikosti)
 * @param S zasobnik
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SExprAddSpace (tStackExpr *S);

/**
 * Vklada data na vrchol zasobniku
 * @param S zasobnik
 * @param data hodnota
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SExprPush (tStackExpr *S, tItemExprPtr data);

/**
 * Vraci data na vrcholu zasobniku
 * @param S zasobnik
 * @return Vraci prvek na vrcholu zasobniku
 */
tItemExprPtr SExprTop (tStackExpr *S);

/**
 * Odstrani data z vrcholu zasobniku neni-li prazdny nebo se zarazkou na vrcholu
 * @param S zasobnik
 * @return true povedlo-li se odstranit, jinak false
 */
bool SExprPop (tStackExpr *S);

/**
 * Zjistuje prazdnotu zasobniku
 * @param S zasobnik
 * @return Je-li zasobnik prazdny vraci true, jinak false
 */
bool SExprEmpty (tStackExpr *S);

/**
 * Uvolnuje alokovanou pamet pro zasobnik
 * @param S zasobnik
 */
void SExprFree (tStackExpr *S);

/**
 * Syntakticka analyza zdola nahoru (vyrazy)
 * @param tk token predany z (do) syntakticke analyzy
 * @param className jmeno tridy
 * @param lok lokalni tabulka
 * @param glob globalni tabulka
 * @param data_type datovy typ vyrazu
 * @param pruchod cislo pruchodu <1,2>
 * @param L list instrukci
 * @return Je-li vyraz bez syn. chyby vracu true, jinak false
 */
bool syntax_expr (TokenPtr *tk, char *className, tBTNodePtr lok,
                tBTNodePtr glob, int *data_type, int pruchod, tLInst *L);

/**
 * Prevede token na ekvivalenti item
 * @param token token z lexikalni analyzy
 * @return item pro syntaktickou analyzu vyrazu
 */
tItemExprPtr tokenToItem (Token* token);

/**
 * Vyhleda odpovidajici reakci na kombinaci tokenu na vstupu a na zasobniku dle
 * precedencni tabulky
 * @param row radek tobulky
 * @param column sloupec tabulky
 * @return pravidlo dle precedencni tabylky (shift, reduce, ...)
 */
int charFromTable (int row, int column);

/**
 * Redukuje zasobnik dle pravidel pro vyrazy
 * @param S zasobnik precedencni analyzy
 * @param className jmeno tridy
 * @param lok lokalni tabulka
 * @param glob globalni tabulka
 * @param data_type datovy typ vyrazu
 * @param order cislo pro #tmpX
 * @param pruchod cislo pruchodu <1,2>
 * @param L list instrukci
 * @return Je-li pravidlo nalezeno a uspesne aplikovano vraci true, jinak false
 */
bool reduceOnStack (tStackExpr *S, char *className, tBTNodePtr lok,
        tBTNodePtr glob, int *data_type, int *order, int pruchod, tLInst *L);

/**
 * prevadi enum operace z casti syntax_expr na enum z casti interpret
 * @param ope kod operace v syntax_expr
 * @return kod oprace v interpret
 */
int transOpe (int ope);

/**
 * prevadi enum datoveho typu z casti syntax_expr na enum z casti interpret
 * @param data_type kod datoveho typu v syntax_expr
 * @return kod operace v interpret
 */
int transType (int data_type);

/**
 * Nastavuje strukturu instrukce pro vlozeni do pasky instrukci
 * @param inst struktura instrukce
 * @param op1 prvni operand
 * @param ope kod operace
 * @param op1 druhy operand
 * @param data_type datovy typ vyrazu
 * @param tmp_pom pomocna promenna pro zpracovani slozitejsich vyrazu
 */
void setInstruction (tInstPtr inst, tItemExprPtr op1, int ope,
                     tItemExprPtr op2, int data_type, char *tmp_pom);
/**
 * Kontroluje semantiku a generuje instrukci
 * @param op1 prvni operand
 * @param ope operace
 * @param op2 druhy operand
 * @param className jmeno tridy
 * @param lok lokalni tabulka
 * @param glob globalni tabulka
 * @param data_type datovy typ vyrazu
 * @param order cislo pro #tmpX
 * @param L list instrukci
 * @return nazev docasne promenne
 */
char* checkAndGener (tItemExprPtr op1, int ope, tItemExprPtr op2,
                int *data_type, int *order, tLInst *L);

#endif // SYNTAX_EXPR

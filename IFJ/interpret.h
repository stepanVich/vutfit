/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * interpret.h
 * 
 * Petr Mohelnik,       xmohel02
 * Dominik Turecek,     xturec06
 */


#ifndef INTERPRET_H
#define INTERPRET_H

#include <string.h>
#include "ial.h"
#include "err.h"
#include "built_in_functions.h"


/*
 *	Struktura instrukce
 */
typedef struct tInst{
    int name;			// Nazev instrukce, vyuziva enum instructionName, napr. I_ADD, I_READINT
    int instType;		// U aritmeticko-logickych instrukci jde o typ OPERANDU, vyuziva enum instructionType, napr. INST_INT
    int operandType1;	// Vyuziva enum operandType. Operandy 1 a 2 jsou typu CONST, VAR nebo NDEF, urcuje jestli je operand konstanta, 
    int operandType2;	// promenna nebo s danym operandem instrukce nepracuje (NDEF)
    int operandType3;	// vysledek instrukce; Operand 3 je u aritmeticko-logickych vzdy typu VAR;
    uVal value1;		// Pokud jde o CONST, tak je zde hodnota operandu, ktera je v prislusne polozce unionu (pokud jde o instType 
    uVal value2;		// INST_DOUBLE, tak budu v unionu pristupovat k double. U VAR zde bude nazev promenne
    uVal value3;		// hodnota vysledku, u AL instrukci jde o NAZEV PROMENNE, do ktere se uklada vysledek
    struct tInst * lptr;
    struct tInst * rptr;
} *tInstPtr;

/*
 *	Struktura seznamu instrukci
 */
typedef struct tLInst{
	tInstPtr Act;
	tInstPtr First;
	tInstPtr Last;
	tInstPtr Return;
} tLInst;

/**
 *	struktura polozek zasobniku navratovych hodnot
 */
typedef struct tRetVal {
	int retType;
	char * retVar;	// promenna, do ktere se priradi vysledek
	tInstPtr a;
} *tRetValPtr;

/**
 * 	Struktura pro dynamicky alokovany zasobnik navratovych hodnot
 */
typedef struct {
    int top; /**< pocet prvku (-1)->prazdny*/
    int max; /**< max. pocet prvku (naalokovana pamet)*/
    tRetValPtr *retVal; /**< dynamicke pole ukazatelu*/
} tRetStack;

/*
 *	Spousti interpret
 *	@param LInst seznam instrukci
 */
bool inter(tLInst * LInst);

/*
 *	Inicializuje seznam instrukci
 *	@param LInst seznam instrukci
 */
void InitInstList (tLInst * LInst);

/*
 *	Zrusi seznam instrukci a nastavi jej do stavu po inicializaci
 *	@param LInst seznam instrukci
 */
void DisposeInstList (tLInst * LInst);

/*
 *	Prida instrukci na konec seznamu
 *	@param LInst seznam instrukci
 *	@param instr vkladana instrukce
 *	@return vraci true, pokud se podarilo pridat instrukci, jinak false
 */
bool InsertInst(tLInst * LInst, struct tInst instr);

/*
 *	Nastavi aktivitu seznamu na danou instrukci
 *	@param LInst seznam instrukci
 *	@param ptr ukazatel na instrukci, ktera ma byt aktivni
 */
void ActiveInst (tLInst *LInst, tInstPtr ptr);

/*
 *	Presune aktivitu na nasledujici instrukci
 *	@param LInst seznam instrukci
 */
void NextInst (tLInst *LInst);

/*
 *	Najde instrukci s danym navestim a nastavi ji na aktivni
 *	@param LInst seznam instrukci
 * 	@param label navesti
 *	@return vraci true, pokud se podarilo danou instrukci najit, jinak false
 */
bool GotoInst (tLInst *LInst, char * label);

/*
 *	Nastavuje errCode.
 *	@param errNo kod chyby
 *	@return vzdy vraci false
 */
bool ReturnError (int errNo);

/*
 *	Pridava do seznamu instrukce potrebne pro vykonani vestavenych funkci
 *	@param LInst seznam instrukci
 *	@return vraci true, pokud vse probehlo v poradku
 */
bool AddBuiltIns (tLInst * LInst);

/*
 *	Prelozi hodnotu enumu INST_TYPE na prislusny DATA_TYPE
 *	@param type Hodnota enumu INST_TYPE
 *	@return Hodnota enumu DATA_TYPE
 */
int translateType(int type);

/******************* zasobnik navratu z funkci *******************/

/**
 * Inicializuje zasobnik. Alokuje misto pro 10 prvku.
 * @param S zasobnik
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SRetInit (tRetStack *S);

/**
 * Zvetsuje zasobnik (na dvojnasobek sve velikosti)
 * @param S zasobnik
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SRetAddSpace (tRetStack *S);

/**
 * Vklada data na vrchol zasobniku
 * @param S zasobnik
 * @param data hodnota
 * @return Nenastane-li chyba vraci true, jinak false
 */
bool SRetPush (tRetStack *S, tRetValPtr data);

/**
 * Odstranuje prvek z vrcholu zasobniku a vraci jeho hodnotu
 * @param S zasobnik
 * @return Vraci prvek na vrcholu zasobniku
 */
tRetValPtr SRetTopPop (tRetStack *S);

/**
 * Zjistuje prazdnotu zasobniku
 * @param S zasobnik
 * @return Je-li zasobnik prazdny vraci true, jinak false
 */
bool SRetEmpty (tRetStack *S);

/**
 * Uvolnuje alokovanou pamet pro zasobnik
 * @param S zasobnik
 */
void SRetFree (tRetStack *S);

/*
 *	enum pro promennou instType struktury tInst
 */
enum {
	INST_VOID,
    INST_INT, 
    INST_DOUBLE, 
    INST_STRING,
    INST_BOOL,			
    INST_FUNCTION,
    INST_LABEL,
    INST_JMP
} instructionType;

/*
 *	enum pro promenne operandType struktury tInst
 */
enum {
    CONST,
    VAR,
    NDEF
} operandType; 

/*
 *	enum pro promennou name struktury tInst
 */
enum {
	I_ADD,		// add 												0
	I_SUB,		// subtract 										1
	I_MUL,		// multiply 										2
	I_DIV,		// divide 											3
	I_LT, 		// less than 										4
	I_GT,		// greater than 									5 	
	I_LEQ,		// less or equal 									6
	I_GEQ,		// greater or equal								 	7
	I_EQ,		// equal 											8
	I_NEQ,		// not equal 										9
	I_READINT,	// read int 										10
	I_READDBL,	// read double 										11
	I_READSTR,	// read string 										12
	I_PRINT,	// print 											13
	I_LEN,		// length 											14
	I_SUBSTR,	// substring 										15
	I_CMP,		// compare 											16
	I_FIND,		// find 											17
	I_SORT,		// sort 											18
	I_LABEL,	// label 											19
	I_GOTO,		// unconditional jump 								20
	I_JMP,		// conditional jump 								21
	I_JMPN,		// conditional jump, jumps if statement is false 	22
	I_ASSIGN,	// assign 											23
	I_RET,		// return  											24
	I_CONCAT,	// concatenate strings 								25
	I_FUNC,		// function 										26
	I_PARAM,	// fucntion parameters 								27
	I_FSTART,	// create local TS, goto function 					28
	I_END,		// end of program 									29
	I_FEND,		// end of function 									30
	I_DECSTART,	// start of declaration outside of function 		31
	I_DECEND	// end of declaration outside of function 			32
} instructionName;

#endif // INTERPRET_H

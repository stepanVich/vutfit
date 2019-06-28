/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * syntax_expr.c
 *
 * Antonin Mazanek,     xmazan07
 */

#include "syntax_expr.h"

#define CONCAT 146 //pro instrukci CONCAT misto ADD
#define ASSIGN 147 //pro instrukci I_ASSIGN
int errCode;

// funkce obdobna funkci FIND (v syntax_ll), ktera misto token, prepira
// slozeny (bool hodnota, je-li identifikator slozeny) a
// name (nazev identifikatoru v programu)
int myFIND(bool slozeny, char *name, char *className, tBTNodePtr lok,
            tBTNodePtr glob, tSymTablePtr *data) {

    if (!slozeny) { // najedna-li se o plne kvalifikovany identifikator
        if (BTSearch (lok, name, data))
            return FOUND_LOCAL;
        else {  // nanajde-li v lok. tabulce vytvori plne kvalifikovany
                // identifikator a pokusi se najit v glob. tabulce
            char *pkid;
            pkid = malloc (strlen(className) + 1 + strlen(name) + 1);
            if (pkid == NULL) {
                errCode = 99;
                fprintf (stderr, "E: malloc v myFind\n");
                return PROGRAM_ERROR;
            }
            pkid[0] = '\0';
            strcat (pkid, className);
            strcat (pkid, ".");
            strcat (pkid, name);

            if(BTSearch (glob, pkid, data))
                return FOUND_GLOBAL;
            else
                return ERR_NOT_FOUND;
        }
    }
    else { // nejedna-li se o slozeny plne kvalifikovany prohleda glob. tabulku
        if(BTSearch (glob, name, data))
            return FOUND_GLOBAL;
        else
            return ERR_NOT_FOUND;
    }
}

// Syntakticka analyza zdola nahoru (vyrazy)
bool syntax_expr (TokenPtr *tk, char *className, tBTNodePtr lok,
        tBTNodePtr glob, int *data_type, int pruchod, tLInst *L) {

    // nejedna-li se o platny token, analyza konci
    if ((*tk)->type == TOKEN_TYPE_ERROR || (*tk)->type == TOKEN_TYPE_SYSTEM_ERROR) {
        return false;
    }

    *data_type = DATA_TYPE_VOID;// datovy typ predany zpet do syntax_ll
    int order = 1;              // pomocna promenna pro generovani instrukci

    tStackExpr Stack;           // zasobnik pro precedencni analyzu
    if (!SExprInit (&Stack))
        return false;

    tItemExprPtr item_in;       // item na vstupu
    tItemExprPtr item_stack;    // item na vrcholu zasobniku

    item_stack = malloc (sizeof (struct tItemExpr));
    if (item_stack == NULL) {
        errCode = 99;
        fprintf (stderr, "E: malloc\n");
        *tk = NULL;
        return false;
    }

    // vlozeni na vrchol zasobniku '$'
    item_stack->name = "$";
    item_stack->stop = false;
    item_stack->konst = false;
    item_stack->pkid = false;
    item_stack->type = IT_END;
    if (!SExprPush (&Stack, item_stack))
        return false;

    // prvni token predan jako parametr
    item_in = tokenToItem (*tk);
    if (item_in == NULL) {
        return false;
    }

    Token *token = *tk;         // token ze syntax_ll
    bool read = false;          // pom. promenna, ma-li se cist dalsi token

    tItemExprPtr item_tmp;      // pom. item, pro kontrolu chyby
    tItemExprPtr neterm_tmp = NULL; // pom. item pro detekci konce vyrazu

    do {
        item_tmp = item_stack;

        // dle tabulky vyber pravidlo: redukce, shift, ...
        switch (charFromTable (item_stack->type, item_in->type)) {
            // rovno (equal)
            case E:
                if (!SExprPush (&Stack, item_in))
                    return false;
                read = true;
                break;
            // posun (shift)
            case S:
                item_stack->stop = true;
                if (!SExprPush (&Stack, item_in))
                    return false;
                read = true;
                break;
            // redukce (reduce)
            case R:
                if (!reduceOnStack (&Stack, className, lok, glob, data_type, &order, pruchod, L)) {
                    // nastane-li chyba v redukovani zasobniku
                    if (item_in->type == IT_R_BRAC && item_tmp->type == IT_NETERMINAL) {
                        // jedna se o konce vyrazu -> generuj instrukci
                        *tk = token;
                        // instrukce
                        if (pruchod == 2) {
                            struct tInst instrukce;
                            instrukce.name = I_ASSIGN;
                            instrukce.operandType2 = NDEF;
                            instrukce.operandType3 = VAR;
                            instrukce.value3.retezec = "#tmp";
                            if (order == 1) {
                                // jedna se o vyraz prirazeni (a = x;)
                                instrukce.instType = transType (neterm_tmp->data_type);
                                if (neterm_tmp->konst) {
                                    instrukce.operandType1 = CONST;
                                    switch (neterm_tmp->data_type) {
                                        case DATA_TYPE_INT: instrukce.value1.cele_cislo = neterm_tmp->konstData.cele_cislo; break;
                                        case DATA_TYPE_DOUBLE: instrukce.value1.desetinne_cislo = neterm_tmp->konstData.desetinne_cislo; break;
                                        case DATA_TYPE_STRING: instrukce.value1.retezec = neterm_tmp->konstData.retezec; break;
                                    }
                                }
                                else {
                                    instrukce.operandType1  = VAR;
                                    instrukce.value1.retezec = neterm_tmp->name;
                                }
                                *data_type = neterm_tmp->data_type;
                            }
                            else {
                                // jedna se o slozitejsi vyraz (a = x <op> ...)
                                instrukce.instType = transType (*data_type);
                                instrukce.operandType1 = VAR;
                                char *tmp_pom = malloc (10);
                                if (tmp_pom == NULL) {
                                    errCode = 99;
                                    fprintf (stderr, "E: malloc\n");
                                    *tk = NULL;
                                    return false;
                                }
                                sprintf (tmp_pom, "#tmp%d", --order);
                                instrukce.value1.retezec = tmp_pom;
                            }
                            // vlozeni instrukce do pasky instrukci
                            if (!InsertInst (L, instrukce))
                                return false;
                        } // konec intstrukce
                        return true;
                    }
                    // nastane-li chyba v redukovani zas. a nejedna se o konec
                    return false;
                }
                read = false;
                break;
            // chyba (fault)
            default:
                if (item_in->type == IT_R_BRAC && item_tmp->type == IT_END) {
                    // jedna se o konec vyrazu -> generuj instrukci
                    *tk = token;
                    // instrukce
                    if (pruchod == 2) {
                        struct tInst instrukce;
                        instrukce.name = I_ASSIGN;
                        instrukce.operandType2 = NDEF;
                        instrukce.operandType3 = VAR;
                        instrukce.value3.retezec = "#tmp";
                        if (order == 1) {
                            // jedna se o vyraz prirazeni (a = x;)
                            if (neterm_tmp != NULL)
                            {
                                instrukce.instType = transType (neterm_tmp->data_type);
                                if (neterm_tmp->konst) {
                                    instrukce.operandType1 = CONST;
                                    switch (neterm_tmp->data_type) {
                                        case DATA_TYPE_INT: instrukce.value1.cele_cislo = neterm_tmp->konstData.cele_cislo; break;
                                        case DATA_TYPE_DOUBLE: instrukce.value1.desetinne_cislo = neterm_tmp->konstData.desetinne_cislo; break;
                                        case DATA_TYPE_STRING: instrukce.value1.retezec = neterm_tmp->konstData.retezec; break;
                                    }
                                }
                                else {
                                    instrukce.operandType1  = VAR;
                                    instrukce.value1.retezec = neterm_tmp->name;
                                }
                                *data_type = neterm_tmp->data_type;
                            }
                            else
                            {
                                instrukce.instType = INST_VOID;
                                instrukce.operandType1 = CONST;
                                instrukce.value1.cele_cislo = 0;
                                *data_type = DATA_TYPE_VOID;
                            }
                        }
                        else {
                            // jedna se o slozitejsi vyraz (a = x <op> ...)
                            instrukce.instType = transType (*data_type);
                            instrukce.operandType1 = VAR;
                            char *tmp_pom = malloc (10);
                            if (tmp_pom == NULL) {
                                errCode = 99;
                                fprintf (stderr, "E: malloc\n");
                                *tk = NULL;
                                return false;
                            }
                            sprintf (tmp_pom, "#tmp%d", --order);
                            instrukce.value1.retezec = tmp_pom;
                        }
                        // vlozeni instrukce do pasky instrukci
                        if (!InsertInst (L, instrukce))
                            return false;
                    }
                    // konec instrukce
                    return true;
                }
                if (token != NULL)
                    *tk = token;
                fprintf (stderr, "E: syntakticka chyba\n");
                errCode = 2;
                return false;
        }

        // nacteni dalsiho tokenu
        if (read) {
            free (token);
            token = get_token();
            if (token->type == TOKEN_TYPE_ERROR || token->type == TOKEN_TYPE_SYSTEM_ERROR) {
                // nacteni neplatneho tokenu
                return false;
            }
            item_in = tokenToItem (token);
            if (item_in == NULL) {
                return false;
            }
        }

        // je-li na vrcholu zasobniku nereminal, nacti nasledujici
        item_stack = SExprTop (&Stack);
        if (item_stack->type == IT_NETERMINAL) {
            int top = Stack.top;
            neterm_tmp = item_stack; // uchovani neterminalu pokud konec vyrazu
            item_stack = Stack.a[top-1];
        }

        // konci jakmile je na vstupu i na vrcholu zasobniku $ (IT_END)
    } while (!(item_in->type == IT_END  && item_stack->type == IT_END));

    if (pruchod == 2) {
        // tvorba instrukce
        struct tInst instrukce;
        instrukce.name = I_ASSIGN;
        instrukce.operandType2 = NDEF;
        instrukce.operandType3 = VAR;
        instrukce.value3.retezec = "#tmp";
        if (order == 1) {
            // jedna se o vyraz prirazeni (a = x;)
            instrukce.instType = transType (item_tmp->data_type);
            if (item_tmp->konst) {
                instrukce.operandType1 = CONST;
                switch (item_tmp->data_type) {
                    case DATA_TYPE_INT: instrukce.value1.cele_cislo = item_tmp->konstData.cele_cislo; break;
                    case DATA_TYPE_DOUBLE: instrukce.value1.desetinne_cislo = item_tmp->konstData.desetinne_cislo; break;
                    case DATA_TYPE_STRING: instrukce.value1.retezec = item_tmp->konstData.retezec; break;
                }
            }
            else {
                instrukce.operandType1  = VAR;
                instrukce.value1.retezec = item_tmp->name;
            }
            *data_type = item_tmp->data_type;
        }
        else {
            // jedna se o slozitejsi vyraz (a = x <op> ...)
            instrukce.instType = transType (*data_type);
            instrukce.operandType1 = VAR;
            char *tmp_pom = malloc (10);
            if (tmp_pom == NULL) {
                errCode = 99;
                fprintf (stderr, "E: malloc\n");
                *tk = NULL;
                return false;
            }
            sprintf (tmp_pom, "#tmp%d", --order);
            instrukce.value1.retezec = tmp_pom;
        }
        // vlozeni instrukce do pasky instrukci
        if (!InsertInst (L, instrukce))
            return false;
    }

    SExprFree (&Stack);
    *tk = token;        // predani posledniho nacteneho tokenu
    return true;        // analyza vyrazu probehla v poradku
}

// Prevede token na ekvivalenti item
tItemExprPtr tokenToItem (Token* token) {

    tItemExprPtr item; // vraceny item
    if ((item = malloc (sizeof (struct tItemExpr))) == NULL) {
        errCode = 99;
        fprintf (stderr, "E: malloc\n");
        return NULL;
    }

    //inicializace nazvu a zarazky
    item->name = token->value;
    item->stop = false;
    item->konst = false;
    item->pkid = false;

    //inicializace typu, urceni zda je konstanta, urceni typu, nastaveni hodnoty
    switch (token->type) {
        case TOKEN_TYPE_OPERATOR:
            if (!strcmp (token->value, "+")) item->type = IT_ADD;
            else if (!strcmp (token->value, "-")) item->type = IT_SUB;
            else if (!strcmp (token->value, "*")) item->type = IT_MUL;
            else if (!strcmp (token->value, "/")) item->type = IT_DIV;
            else if (!strcmp (token->value, "==")) item->type = IT_EQ;
            else if (!strcmp (token->value, "!=")) item->type = IT_NOT_EQ;
            else if (!strcmp (token->value, "<")) item->type = IT_LESS;
            else if (!strcmp (token->value, "<=")) item->type = IT_LESS_EQ;
            else if (!strcmp (token->value, ">")) item->type = IT_MORE;
            else if (!strcmp (token->value, ">=")) item->type = IT_MORE_EQ;
            else if (!strcmp (token->value, "(")) item->type = IT_L_BRAC;
            else if (!strcmp (token->value, ")")) item->type = IT_R_BRAC;
            else item->type = IT_END;
            break;
        case TOKEN_TYPE_INT:
            item->type = IT_ID;
            item->konst = true;
            item->data_type = DATA_TYPE_INT;
            item->konstData.cele_cislo = atoi (token->value);
            break;
        case TOKEN_TYPE_DOUBLE:
            item->type = IT_ID;
            item->konst = true;
            item->data_type = DATA_TYPE_DOUBLE;
            item->konstData.desetinne_cislo = strtod (token->value, NULL);
            break;
        case TOKEN_TYPE_STRING:
            item->type = IT_ID;
            item->konst = true;
            item->data_type = DATA_TYPE_STRING;
            item->konstData.retezec = token->value;
            break;
        case TOKEN_TYPE_IDENTIFIKATOR_SLOZENY:
            item->pkid = true;
        case TOKEN_TYPE_IDENTIFIKATOR:
            item->type = IT_ID;
            break;
        default:
            item->type = IT_END;
    }

    return item;
}

// Vyhleda odpovidajici reakci na kombinaci tokenu na vstupu a na zasobniku dle
// precedencni tabulky
int charFromTable (int row, int column) {

    // Precedencni tabulka
    static char_prec_tabulka prec_tabulka [14][14] = {
        //        +  -  *  / == !=  < <=  > >=  (  ) id  $
        /*'+'*/ { R, R, S, S, R, R, R, R, R, R, S, R, S, R},
        /*'-'*/ { R, R, S, S, R, R, R, R, R, R, S, R, S, R},
        /*'*'*/ { R, R, R, R, R, R, R, R, R, R, S, R, S, R},
        /*'/'*/ { R, R, R, R, R, R, R, R, R, R, S, R, S, R},
        /*'=='*/{ S, S, S, S, F, F, F, F, F, F, S, R, S, R},
        /*'!='*/{ S, S, S, S, F, F, F, F, F, F, S, R, S, R},
        /*'<'*/ { S, S, S, S, F, F, F, F, F, F, S, R, S, R},
        /*'<='*/{ S, S, S, S, F, F, F, F, F, F, S, R, S, R},
        /*'>'*/ { S, S, S, S, F, F, F, F, F, F, S, R, S, R},
        /*'>='*/{ S, S, S, S, F, F, F, F, F, F, S, R, S, R},
        /*'('*/ { S, S, S, S, S, S, S, S, S, S, S, E, S, F},
        /*')'*/ { R, R, R, R, R, R, R, R, R, R, F, R, F, R},
        /*'id'*/{ R, R, R, R, R, R, R, R, R, R, F, R, F, R},
        /*'$' */{ S, S, S, S, S, S, S, S, S, S, S, F, S, F}
    };

    return prec_tabulka[row][column];
}

// Redukuje zasobnik dle pravidel pro vyrazy
bool reduceOnStack (tStackExpr *Stack, char *className, tBTNodePtr lok,
        tBTNodePtr glob, int *data_type, int *order, int pruchod, tLInst *L) {

    // ziskani prvniho itemu na zasobniku
    tItemExprPtr item_stack;
    item_stack = SExprTop (Stack);

    // urceni vzdalenosti mezi vrcholem a zarazkou
    int top = Stack->top;
    while (item_stack->stop == false) {
        top--;
        item_stack = Stack->a[top];
        if (item_stack->type == IT_END && item_stack->stop == false) {
            fprintf (stderr, "E: zpracovani vyrazu, nenalezena zarazka\n");
            errCode = 99;
            return false;
        }
    }
    // zruseni zarazky
    item_stack->stop = false;

    // pravidlo dle vzdalenosti vrcholu a zarazky:
    // 0: E->i, 1: E->E<op>E, 2: E->(E)
    int pravidlo = Stack->top - top;

    // osetreni pravidla E->(E)
    item_stack = SExprTop (Stack);
    if (item_stack->type == IT_R_BRAC)
        pravidlo = 2;

    // E->i (i=id, pkid, int, double, String)
    if (pravidlo == 1) {

        item_stack = SExprTop (Stack);
        if (!SExprPop (Stack))
            return false;

        if (pruchod == 2 && item_stack->konst == false) {
            // neni-li konstanta prohledej tabulkz symbolu
            tSymTablePtr data;
            int priznak = myFIND (item_stack->pkid, item_stack->name, className, lok, glob, &data);

            if (priznak == PROGRAM_ERROR) {
                return false;
            }
            if (priznak == ERR_NOT_FOUND) {
                // nenasel-li zaznam v tabulce, nastav chybu (sem - 3) a zkonci
                errCode = 3;
                fprintf(stderr, "E: nedefinovana promenna\n");
                return false;
            }
            if (priznak == FOUND_GLOBAL && item_stack->pkid == false) {
                // pokud nalezeno v glob. tabulce symbolu, ale nejedna se o
                // plne kvalifikovany identifikator, uprav nazev aby se jednalo
                char *pomoc = malloc (strlen(className) + 1 + strlen (item_stack->name)+1);
                if (pomoc == NULL) {
                    errCode = 99;
                    fprintf (stderr, "E: malloc\n");
                    return false;
                }
                pomoc[0] = '\0';
                strcat (pomoc, className);
                strcat (pomoc, ".");
                strcat (pomoc, item_stack->name);
                item_stack->name = pomoc;
            }
            item_stack->data_type = data->variable->data_type;
        }

        item_stack->type = IT_NETERMINAL;
        item_stack->stop = false;

        if (!SExprPush (Stack, item_stack))
            return false;

        return true;
    }
    // E->(E)
    else if (pravidlo == 2) {

        if (!SExprPop (Stack))  // '('
            return false;
        item_stack = SExprTop (Stack);  // 'E'
        if (!SExprPop (Stack) || !SExprPop (Stack)) // ')'
            return false;

        item_stack->type = IT_NETERMINAL;
        item_stack->stop = false;

        if(!SExprPush (Stack, item_stack))
            return false;
        return true;
    }
    // E->E<op>E
    else if (pravidlo == 3) {

        tItemExprPtr operand1 = SExprTop (Stack); // 'E'
        if (!SExprPop (Stack))
            return false;
        tItemExprPtr operator = SExprTop (Stack); // '<op>'
        if (!SExprPop (Stack))
            return false;
        tItemExprPtr operand2 = SExprTop (Stack); // 'E'
        if (!SExprPop (Stack))
            return false;

        if (pruchod == 2) {
            // jedna-li se o 2. pruchod, zkontroluj semantiku a vygeneruj instr.
            operand1->name = checkAndGener (operand1, operator->type, operand2,
                                            data_type, order, L);
            if (operand1->name == NULL)
                return false;
            operand1->data_type = *data_type;
        }
        operand1->pkid = false;
        operand1->konst = false;
        operand1->type = IT_NETERMINAL;
        operand1->stop = false;

        if (!SExprPush (Stack, operand1))
            return false;
        return true;
    }

    // nenalezeno pravidlo
    errCode = 2;
    return false;
}

// prevadi enum operace z casti syntax_expr na enum z casti interpret
int transOpe (int ope) {

    int ret;
    switch (ope) {
        case IT_ADD: ret = I_ADD; break;
        case IT_SUB: ret = I_SUB; break;
        case IT_MUL: ret = I_MUL; break;
        case IT_DIV: ret = I_DIV; break;
        case IT_EQ: ret = I_EQ; break;
        case IT_NOT_EQ: ret = I_NEQ; break;
        case IT_LESS: ret = I_LT; break;
        case IT_LESS_EQ: ret = I_LEQ; break;
        case IT_MORE: ret = I_GT; break;
        case IT_MORE_EQ: ret = I_GEQ; break;
        case CONCAT: ret = I_CONCAT; break;
        case ASSIGN: ret = I_ASSIGN; break;
        default:
            ret = 99;
    }
    return ret;
}

// prevadi enum datoveho typu z casti syntax_expr na enum z casti interpret
int transType (int data_type) {

    int ret;
    switch (data_type) {
        case DATA_TYPE_INT: ret = INST_INT; break;
        case DATA_TYPE_DOUBLE: ret = INST_DOUBLE; break;
        case DATA_TYPE_STRING: ret = INST_STRING; break;
        case DATA_TYPE_BOOL: ret = INST_BOOL; break;
        case DATA_TYPE_VOID: ret = INST_VOID; break;
        default:
            ret = 99;
    }
    return ret;
}

// Nastavuje strukturu instrukce pro vlozeni do pasky instrukci
void setInstruction (tInstPtr inst, tItemExprPtr op1, int ope,
                     tItemExprPtr op2, int data_type, char *tmp_pom) {


    inst->name = transOpe (ope);
    inst->instType = transType (data_type);
    inst->operandType1 = (op1->konst) ? CONST : VAR;
    inst->operandType2 = (op2->konst) ? CONST : VAR;
    inst->operandType3 = VAR;
    if (op1->konst) {
        switch (op1->data_type) {
            case DATA_TYPE_INT: inst->value1.cele_cislo = op1->konstData.cele_cislo; break;
            case DATA_TYPE_DOUBLE: inst->value1.desetinne_cislo = op1->konstData.desetinne_cislo; break;
            case DATA_TYPE_STRING: inst->value1.retezec = op1->konstData.retezec; break;
        }
    } else {
        inst->value1.retezec = op1->name;
    }
    if (op2->konst) {
        switch (op2->data_type) {
            case DATA_TYPE_INT: inst->value2.cele_cislo = op2->konstData.cele_cislo; break;
            case DATA_TYPE_DOUBLE: inst->value2.desetinne_cislo = op2->konstData.desetinne_cislo; break;
            case DATA_TYPE_STRING: inst->value2.retezec = op2->konstData.retezec; break;
        }
    } else {
        inst->value2.retezec = op2->name;
    }
    inst->value3.retezec = tmp_pom;
}

// Kontroluje semantiku a generuje instrukci
char* checkAndGener (tItemExprPtr op1, int ope, tItemExprPtr op2,
                            int *data_type, int *order, tLInst *L){

    int inst_type;

    if (ope == IT_ADD) { // +
        if (op1->data_type == DATA_TYPE_STRING) { // String + xxx
            if (op2->data_type == DATA_TYPE_STRING) { // String + String
                *data_type = DATA_TYPE_STRING;
                inst_type = DATA_TYPE_STRING;
                ope = CONCAT;
            }
            else if (op2->data_type == DATA_TYPE_DOUBLE) { // String + double
                if (op2->konst) {
                    // je-li konstanta, preved na retezec --> konkatenace
                    op2->data_type = DATA_TYPE_STRING;
                    char *pomoc = malloc (15);
                    if (pomoc == NULL) {
                        fprintf (stderr, "E: malloc\n");
                        errCode = 99;
                        return NULL;
                    }
                    sprintf (pomoc, "%g", op2->konstData.desetinne_cislo);
                    op2->konstData.retezec = pomoc;
                }
                *data_type = DATA_TYPE_STRING;
                inst_type = DATA_TYPE_STRING;
                ope = CONCAT;
            }
            else if (op2->data_type == DATA_TYPE_INT) { // String + int
                if (op2->konst) {
                    // je-li konstanta, preved na retezec --> konkatenace
                    op2->data_type = DATA_TYPE_STRING;
                    char *pomoc = malloc (15);
                    if (pomoc == NULL) {
                        errCode = 99;
                        fprintf (stderr, "E: malloc\n");
                        return NULL;
                    }
                    sprintf (pomoc, "%d", op2->konstData.cele_cislo);
                    op2->konstData.retezec = pomoc;
                }
                *data_type = DATA_TYPE_STRING;
                inst_type = DATA_TYPE_STRING;
                ope = CONCAT;
            }
            else { // String + error
                errCode = 4;
                fprintf (stderr, "E: semanticka chyba\n");
                return NULL;
            }
        }
        else if (op2->data_type == DATA_TYPE_STRING) { // xxx+ String
            if (op1->data_type == DATA_TYPE_DOUBLE) { // double + String
                if (op1->konst) {
                    // je-li konstanta, preved na retezec --> konkatenace
                    op1->data_type = DATA_TYPE_STRING;
                    char *pomoc = malloc (15);
                    if (pomoc == NULL) {
                        //TODO reakce na chybu
                        errCode = 99;
                        fprintf (stderr, "E: malloc\n");
                        return false;
                    }
                    sprintf (pomoc, "%g", op1->konstData.desetinne_cislo);
                    op1->konstData.retezec = pomoc;
                }
                *data_type = DATA_TYPE_STRING;
                inst_type = DATA_TYPE_STRING;
                ope = CONCAT;
            }
            else if (op1->data_type == DATA_TYPE_INT) { // int + String
                if (op1->konst) {
                    op1->data_type = DATA_TYPE_STRING;
                    char *pomoc = malloc (15);
                    if (pomoc == NULL) {
                        errCode = 99;
                        fprintf (stderr, "E: malloc\n");
                        return false;
                    }
                    sprintf (pomoc, "%d", op1->konstData.cele_cislo);
                    op1->konstData.retezec = pomoc;
                }
                *data_type = DATA_TYPE_STRING;
                inst_type = DATA_TYPE_STRING;
                ope = CONCAT;
            }
            else { // error + String
                errCode = 4;
                fprintf (stderr, "E: semanticka chyba\n");
                return NULL;
            }
        }
        else if (op1->data_type == DATA_TYPE_INT && op2->data_type == DATA_TYPE_INT) {
            // int + int
            *data_type = DATA_TYPE_INT;
            inst_type = DATA_TYPE_INT;
        }
        else if (op1->data_type == DATA_TYPE_INT && op2->data_type == DATA_TYPE_DOUBLE) {
            // int + double
            if (op1->konst) {
                // je-li konstanta, preved double
                op1->data_type = DATA_TYPE_DOUBLE;
                op1->konstData.desetinne_cislo = (double)op1->konstData.cele_cislo;
            }
            *data_type = DATA_TYPE_DOUBLE;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else if (op1->data_type == DATA_TYPE_DOUBLE && op2->data_type == DATA_TYPE_INT) {
            // double + int
            if (op2->konst) {
                // je-li konstanta, preved na double
                op2->data_type = DATA_TYPE_DOUBLE;
                op2->konstData.desetinne_cislo = (double)op2->konstData.cele_cislo;
            }
            *data_type = DATA_TYPE_DOUBLE;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else if (op1->data_type == DATA_TYPE_DOUBLE && op2->data_type == DATA_TYPE_DOUBLE) {
            // double + double
            *data_type = DATA_TYPE_DOUBLE;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else { // error + error
            errCode = 4;
            fprintf (stderr, "E: semanticka chyba\n");
            return NULL;
        }
    }
    else if (ope == IT_SUB || ope == IT_MUL || ope == IT_DIV) { // -, * , /
        if (op1->data_type == DATA_TYPE_INT && op2->data_type == DATA_TYPE_INT) {
            // int <-,*,/> int
            *data_type = DATA_TYPE_INT;
            inst_type = DATA_TYPE_INT;
        }
        else if (op1->data_type == DATA_TYPE_INT && op2->data_type == DATA_TYPE_DOUBLE) {
            // int <-,*,/> double
            if (op1->konst) {
                // je-li konstanta, preved na double
                op1->data_type = DATA_TYPE_DOUBLE;
                op1->konstData.desetinne_cislo = (double)op1->konstData.cele_cislo;
            }
            *data_type = DATA_TYPE_DOUBLE;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else if (op1->data_type == DATA_TYPE_DOUBLE && op2->data_type == DATA_TYPE_INT) {
            // double <-,*,/> int
            if (op2->konst) {
                // je-li konstanta, preved na double
                op2->data_type = DATA_TYPE_DOUBLE;
                op2->konstData.desetinne_cislo = (double)op2->konstData.cele_cislo;
            }
            *data_type = DATA_TYPE_DOUBLE;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else if (op1->data_type == DATA_TYPE_DOUBLE && op2->data_type == DATA_TYPE_DOUBLE) {
            // double <-,*,/> double
            *data_type = DATA_TYPE_DOUBLE;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else { // error <-,*,/> error
            errCode = 4;
            fprintf (stderr, "E: semanticka chyba\n");
            return NULL;
        }
    }
    else if (ope == IT_EQ || ope == IT_NOT_EQ || ope == IT_LESS ||
            ope == IT_LESS_EQ || ope == IT_MORE || ope == IT_MORE_EQ) {
                // == , !=, <, <=, >, >=
        if (op1->data_type == DATA_TYPE_INT && op2->data_type == DATA_TYPE_INT) {
            // int <==,!=,<,<=,>,>=> int
            *data_type = DATA_TYPE_BOOL;
            inst_type = DATA_TYPE_INT;
        }
        else if (op1->data_type == DATA_TYPE_INT && op2->data_type == DATA_TYPE_DOUBLE) {
            // int <==,!=,<,<=,>,>=> doube
            if (op1->konst) {
                // je-li konstanta, preved na double
                op1->data_type = DATA_TYPE_DOUBLE;
                op1->konstData.desetinne_cislo = (double)op1->konstData.cele_cislo;
            }
            *data_type = DATA_TYPE_BOOL;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else if (op1->data_type == DATA_TYPE_DOUBLE && op2->data_type == DATA_TYPE_INT) {
            // double <==,!=,<,<=,>,>=> int
            if (op2->konst) {
                // je-li konstanta, preved na double
                op2->data_type = DATA_TYPE_DOUBLE;
                op2->konstData.desetinne_cislo = (double)op2->konstData.cele_cislo;
            }
            *data_type = DATA_TYPE_BOOL;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else if (op1->data_type == DATA_TYPE_DOUBLE && op2->data_type == DATA_TYPE_DOUBLE) {
            // double <==,!=,<,<=,>,>=> double
            *data_type = DATA_TYPE_BOOL;
            inst_type = DATA_TYPE_DOUBLE;
        }
        else { // error <==,!=,<,<=,>,>=> error
            errCode = 4;
            fprintf (stderr, "E: semanticka chyba\n");
            return NULL;
        }
    }

    char *tmp_pom = malloc (10);
    if (tmp_pom == NULL) {
        errCode = 99;
        fprintf (stderr, "E: malloc\n");
        return NULL;
    }
    // pomocna promenna pro slozitejsi vyrazy
    //[# - zvoleno, protoze program nemuze obsahovat tento znak]
    sprintf (tmp_pom, "#tmp%d", (*order));
    (*order)++;

    // nastaveni instrukce (op2, op1 otoceno -> zasobnikova struktura)
    struct tInst instrukce;
    setInstruction (&instrukce, op2, ope, op1, inst_type, tmp_pom);
    // vlozeni instrukce do pasky instrukci
    if (!InsertInst (L, instrukce))
        return NULL;

    return tmp_pom;
}

// Inicializuje zasobnik. Alokuje misto pro 10 prvku.
bool SExprInit (tStackExpr *Stack) {

    if ((Stack->a = malloc (10 * sizeof (struct tItemExpr))) == NULL) {
        errCode = 99;
        fprintf (stderr, "E: malloc\n");
        return false;
    }
    Stack->top = -1;
    Stack->max = 10;
    return true;
}

// Zvetsuje zasobnik (na dvojnasobek sve velikosti)
bool SExprAddSpace (tStackExpr *Stack) {

    Stack->max *= 2;
    Stack->a = realloc (Stack->a, Stack->max * sizeof (struct tItemExpr));
    if (Stack->a == NULL) {
        errCode = 99;
        fprintf (stderr, "E: realloc\n");
        return false;
    }
    return true;
}

// Vklada data na vrchol zasobniku
bool SExprPush (tStackExpr *Stack, tItemExprPtr data) {

    if (Stack->top == Stack->max-1)
        if (!SExprAddSpace (Stack))
            return false;

    Stack->top++;
    Stack->a[Stack->top] = data;
    return true;
}

// Vraci data na vrcholu zasobniku
tItemExprPtr SExprTop (tStackExpr *Stack) {

    return Stack->a[Stack->top];
}

// Odstrani data z vrcholu zasobniku neni-li prazdny nebo se zarazkou na vrcholu
bool SExprPop (tStackExpr *Stack) {

    tItemExprPtr item_stack = SExprTop (Stack);
    if (SExprEmpty (Stack) || item_stack->type == IT_END) {
        errCode = 2;
        fprintf (stderr, "E: syntakticka chyba\n");
        return false;
    }
    Stack->top--;
    return true;
}

// Zjistuje prazdnotu zasobniku
bool SExprEmpty (tStackExpr *Stack) {

    return (Stack->top == -1);
}

// Uvolnuje alokovanou pamet pro zasobnik
void SExprFree (tStackExpr *Stack) {

    free (Stack->a);
    Stack->a = NULL;
}

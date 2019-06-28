/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * interpret.c
 * 
 * Petr Mohelnik,       xmohel02
 * Monika Muzikovska,   xmuzik05
 * Dominik Turecek,     xturec06
 */


#include "interpret.h"
bool inter(tLInst * instr) {

    // globalni tabulka pro staticke promenne
    tBTNodePtr Global_TS; 
    BTInit(&Global_TS);

    // zasobnik pro funkce
    tStack TSstack;
    SInit (&TSstack);

    // lokalni tabulka funkce Main.run
    tBTNodePtr Local_TS;
    BTInit (&Local_TS);
    SPush (&TSstack, Local_TS); 

    // zasobnik pro ukazatel navratu z funkce
    tRetStack retStack;
    if (!SRetInit (&retStack))
        return ReturnError (99);

    // Pomocne promenne pro vkladani do TS a hledani v TS
    tSymTablePtr item1;
    tSymTablePtr item2;
    tSymTablePtr item3;
    bool found1;
    bool found2;
    bool found3;

    // pomocne promenne
    int int1, int2;
    double dbl1, dbl2;
    char *str1;
    char *str2;
    char *str3;
    int targetType;
    bool retype;

    // pridani instrukci pro vestavene funkce do instrukcni pasky
    if (!AddBuiltIns (instr)) 
        return ReturnError (99);

    int pruchod = 1;                    // prvni pruchod resici deklarace promennych mimo funkce
    bool declarationMode = false;       // umoznuje deklaraci mimo funkce v prvnim pruchodu
    ActiveInst (instr, instr->First);   // prvni pruchod zacina prvni instrukci v seznamu

    while (instr->Act != NULL)
    {
        // Inicializace pomocnych promennych
        item1 = NULL;
        item2 = NULL;
        item3 = NULL;
        int1=0;
        int2=0;
        dbl1=0;
        dbl2=0;
        retype = false;
        found1 = 0;
        found2 = 0;
        found3 = 0;

        switch (instr->Act->name)
        {
            case I_ADD:     // add
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    // Hledani promenne vysledku v TS
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {
                        found3 = BTSearch(Global_TS, instr->Act->value3.retezec, &item3);
                    } else {
                        found3 = BTSearch(TSstack.a[TSstack.top], instr->Act->value3.retezec, &item3);
                    }

                    if(found3) {
                        targetType = item3->variable->data_type;
                        if (strchr(instr->Act->value3.retezec, '#') != NULL)
                            retype = true;
                        else
                            retype = false;
                    }

                    switch(instr->Act->instType) {
                        case INST_INT:  // scitani integer

                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR

                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }

                                if(found1 && item1->variable->init) {

                                    if (item1->variable->data_type == DATA_TYPE_INT)
                                        int1 = item1->variable->value.cele_cislo;
                                    else if (item1->variable->data_type == DATA_TYPE_DOUBLE)
                                        int1 = (int) item1->variable->value.desetinne_cislo;
                                } 
                                else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR

                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }

                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT)
                                        int2 = item2->variable->value.cele_cislo;
                                    else if (item2->variable->data_type == DATA_TYPE_DOUBLE)
                                        int2 = (int) item2->variable->value.desetinne_cislo;
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            if(found1 && found2) {
                                if (!found3) {

                                    item3 = malloc(sizeof(struct tSymTable));
                                    if(item3 == NULL)
                                        return ReturnError (99);
                                    item3->type = TABLE_VARIABLE;
                                    item3->name = instr->Act->value3.retezec;
                                    item3->variable = malloc(sizeof(struct dataVariable));
                                    if(item3->variable == NULL)
                                        return ReturnError (99);
                                    item3->variable->init = true;  
                                    item3->variable->data_type = DATA_TYPE_INT;
                                    item3->variable->value.cele_cislo = int1 + int2;
                                }
                                else {

                                    if(retype) {  // Vysledek je ulozen do promenne typu #tmpN, ktera uz je v TS
                                        item3->variable->data_type = DATA_TYPE_INT;
                                        item3->variable->value.cele_cislo = int1 + int2;
                                    }
                                    else {
                                        if (targetType == DATA_TYPE_INT)
                                            item3->variable->value.cele_cislo = int1 + int2;
                                        else if (targetType == DATA_TYPE_DOUBLE)
                                            item3->variable->value.desetinne_cislo = (double) int1 + int2;
                                    }
                                }

                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }

                                if (!found3)
                                {
                                    free(item3);
                                    item3 = NULL;
                                }

                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:  // scitani double

                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR

                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }

                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT)
                                        dbl1 = (double) item1->variable->value.cele_cislo;
                                    else if (item1->variable->data_type == DATA_TYPE_DOUBLE)
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR

                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }

                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT)
                                        dbl2 = (double) item2->variable->value.cele_cislo;
                                    else if (item2->variable->data_type == DATA_TYPE_DOUBLE)
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            if(found1 && found2) {

                                if (!found3) {
                                    item3 = malloc(sizeof(struct tSymTable));
                                    if(item3 == NULL)
                                        return ReturnError (99);

                                    item3->type = TABLE_VARIABLE;
                                    item3->name = instr->Act->value3.retezec;
                                    item3->variable = malloc(sizeof(struct dataVariable));
                                    if(item3->variable == NULL)
                                        return ReturnError (99);

                                    item3->variable->init = true; 
                                    item3->variable->data_type = DATA_TYPE_DOUBLE;
                                    item3->variable->value.desetinne_cislo = dbl1 + dbl2;
                                }
                                else {

                                    if(retype) {  // Vysledek je ulozen do promenne typu #tmpN, ktera uz je v TS
                                        item3->variable->data_type = DATA_TYPE_DOUBLE;
                                        item3->variable->value.desetinne_cislo = dbl1 + dbl2;
                                    }
                                    else {

                                        if (targetType == DATA_TYPE_INT)
                                            return ReturnError (10); 
                                        else if (targetType == DATA_TYPE_DOUBLE)
                                            item3->variable->value.desetinne_cislo = dbl1 + dbl2;
                                    }
                                }

                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }

                                if (!found3)
                                {
                                    free(item3);
                                    item3 = NULL;
                                }
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // Nespravny typ instrukce
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_SUB:     // sub
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    // Hledani vysledne promenne v tabulce
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {
                        found3 = BTSearch(Global_TS, instr->Act->value3.retezec, &item3);
                    } else {
                        found3 = BTSearch(TSstack.a[TSstack.top], instr->Act->value3.retezec, &item3);
                    }

                    if(found3) {
                        targetType = item3->variable->data_type;
                        if (strchr(instr->Act->value3.retezec, '#') != NULL)
                            retype = true;
                        else
                            retype = false;
                    }
                    
                    switch(instr->Act->instType) {
                        case INST_INT:  // odcitani integer
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                if (!found3) {
                                    item3 = malloc(sizeof(struct tSymTable));
                                    if(item3 == NULL)
                                        return ReturnError (99);
                                    item3->type = TABLE_VARIABLE;
                                    item3->name = instr->Act->value3.retezec;
                                    item3->variable = malloc(sizeof(struct dataVariable));
                                    if(item3->variable == NULL)
                                        return ReturnError (99);
                                    item3->variable->init = true;
                                    item3->variable->data_type = DATA_TYPE_INT;
                                    item3->variable->value.cele_cislo = int1 - int2;
                                } else {
                                    if (retype) {
                                        item3->variable->data_type = DATA_TYPE_INT;
                                        item3->variable->value.cele_cislo = int1 - int2;
                                    } else {
                                        if (targetType == DATA_TYPE_INT)
                                            item3->variable->value.cele_cislo = int1 - int2;
                                        else if (targetType == DATA_TYPE_DOUBLE)
                                            item3->variable->value.desetinne_cislo = (double) (int1 - int2);
                                        else {
                                            // nepodporovany datovy typ
                                            return ReturnError (10);
                                        }
                                    }
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                if (!found3)
                                {
                                    free(item3);
                                    item3 = NULL;
                                }
                            } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:  // odcitani double
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                if (!found3) {
                                    item3 = malloc(sizeof(struct tSymTable));
                                    if(item3 == NULL)
                                        return ReturnError (99);
                                    item3->type = TABLE_VARIABLE;
                                    item3->name = instr->Act->value3.retezec;
                                    item3->variable = malloc(sizeof(struct dataVariable));
                                    if(item3->variable == NULL)
                                        return ReturnError (99);
                                    item3->variable->init = true;  
                                    item3->variable->data_type = DATA_TYPE_DOUBLE;
                                    item3->variable->value.desetinne_cislo = dbl1 - dbl2;
                                }
                                else {
                                    if (retype) {
                                        item3->variable->data_type = DATA_TYPE_DOUBLE;
                                        item3->variable->value.desetinne_cislo = dbl1 - dbl2;
                                    } else {
                                        if (targetType == DATA_TYPE_INT)
                                            return ReturnError (10); 
                                        else if (targetType == DATA_TYPE_DOUBLE)
                                            item3->variable->value.desetinne_cislo = dbl1 - dbl2;
                                        else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                        }
                                    }
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                if (!found3)
                                {
                                    free(item3);
                                    item3 = NULL;
                                }
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // Nespravny typ instrukce
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_MUL:     // multiply
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    // Hledani vysledne promenne v TS
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {
                        found3 = BTSearch(Global_TS, instr->Act->value3.retezec, &item3);
                    } else {
                        found3 = BTSearch(TSstack.a[TSstack.top], instr->Act->value3.retezec, &item3);
                    }
                    if(found3) {
                        targetType = item3->variable->data_type;
                        if (strchr(instr->Act->value3.retezec, '#') != NULL)
                            retype = true;
                        else
                            retype = false;
                    }
                    switch(instr->Act->instType) {
                        case INST_INT:  // nasobeni integer
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                if (!found3) {
                                    item3 = malloc(sizeof(struct tSymTable));
                                    if(item3 == NULL)
                                        return ReturnError (99);
                                    item3->type = TABLE_VARIABLE;
                                    item3->name = instr->Act->value3.retezec;
                                    item3->variable = malloc(sizeof(struct dataVariable));
                                    if(item3->variable == NULL)
                                        return ReturnError (99);
                                    item3->variable->init = true;  
                                    item3->variable->data_type = DATA_TYPE_INT;
                                    item3->variable->value.cele_cislo = int1 * int2;
                                }
                                else {
                                    if (retype) {
                                        item3->variable->data_type = DATA_TYPE_INT;
                                        item3->variable->value.cele_cislo = int1 * int2;
                                    } else {
                                        if (targetType == DATA_TYPE_INT)
                                            item3->variable->value.cele_cislo = int1 * int2;
                                        else if (targetType == DATA_TYPE_DOUBLE)
                                            item3->variable->value.desetinne_cislo = (double) (int1 * int2);
                                        else {
                                            // nepodporovany datovy typ
                                            return ReturnError (10);
                                        }
                                    }
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                if (!found3)
                                {
                                    free(item3);
                                    item3 = NULL;
                                }
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:  // nasobeni double
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                if (!found3) {
                                    item3 = malloc(sizeof(struct tSymTable));
                                    if(item3 == NULL)
                                        return ReturnError (99);
                                    item3->type = TABLE_VARIABLE;
                                    item3->name = instr->Act->value3.retezec;
                                    item3->variable = malloc(sizeof(struct dataVariable));
                                    if(item3->variable == NULL)
                                        return ReturnError (99);
                                    item3->variable->init = true;  
                                    item3->variable->data_type = DATA_TYPE_DOUBLE;
                                    item3->variable->value.desetinne_cislo = dbl1 * dbl2;
                                }
                                else {
                                    if (retype) {
                                        item3->variable->data_type = DATA_TYPE_DOUBLE;
                                        item3->variable->value.desetinne_cislo = dbl1 * dbl2;
                                    } else {
                                        if (targetType == DATA_TYPE_INT)
                                            return ReturnError (10);  
                                        else if (targetType == DATA_TYPE_DOUBLE)
                                            item3->variable->value.desetinne_cislo = dbl1 * dbl2;
                                        else {
                                            // nepodporovany datovy typ
                                            return ReturnError (10);
                                        }
                                    }
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                if(!found3)
                                {
                                    free(item3);
                                    item3 = NULL;
                                }
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // Nespravny typ instrukce
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_DIV:     // divide
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    // Hledani vysledne promenne v TS
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {
                        found3 = BTSearch(Global_TS, instr->Act->value3.retezec, &item3);
                    } else {
                        found3 = BTSearch(TSstack.a[TSstack.top], instr->Act->value3.retezec, &item3);
                    }
                    if(found3) {
                        targetType = item3->variable->data_type;
                        if (strchr(instr->Act->value3.retezec, '#') != NULL)
                            retype = true;
                        else
                            retype = false;
                    }
                    switch(instr->Act->instType) {
                        case INST_INT:  // deleni integer
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                if(int2 == (int)0) {
                                    // chyba deleni nulou
                                    return ReturnError (9);
                                }
                                if (!found3) {
                                    item3 = malloc(sizeof(struct tSymTable));
                                    if(item3 == NULL)
                                        return ReturnError (99);
                                    item3->type = TABLE_VARIABLE;
                                    item3->name = instr->Act->value3.retezec;
                                    item3->variable = malloc(sizeof(struct dataVariable));
                                    if(item3->variable == NULL)
                                        return ReturnError (99);
                                    item3->variable->init = true;  
                                    item3->variable->data_type = DATA_TYPE_INT;
                                    item3->variable->value.cele_cislo = int1 / int2;
                                }
                                else {
                                    if (retype) {
                                        item3->variable->data_type = DATA_TYPE_INT;
                                        item3->variable->value.cele_cislo = int1 / int2;
                                    } else {
                                        if (targetType == DATA_TYPE_INT)
                                            item3->variable->value.cele_cislo = int1 / int2;
                                        else if (targetType == DATA_TYPE_DOUBLE)
                                            item3->variable->value.desetinne_cislo = (double) (int1 / int2);
                                        else {
                                            // nepodporovany datovy typ
                                            return ReturnError (10);
                                        }
                                    }
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                if(!found3)
                                {
                                    free(item3);
                                    item3 = NULL;
                                }
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:  // deleni double
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                if(dbl2 == (double)0) {
                                    // chyba deleni nulou
                                    return ReturnError (9);
                                }
                                if (!found3) {
                                    item3 = malloc(sizeof(struct tSymTable));
                                    if(item3 == NULL)
                                        return ReturnError (99);
                                    item3->type = TABLE_VARIABLE;
                                    item3->name = instr->Act->value3.retezec;
                                    item3->variable = malloc(sizeof(struct dataVariable));
                                    if(item3->variable == NULL)
                                        return ReturnError (99);
                                    item3->variable->init = true; 
                                    item3->variable->data_type = DATA_TYPE_DOUBLE;
                                    item3->variable->value.desetinne_cislo = dbl1 / dbl2;
                                }
                                else {
                                    if (retype) {
                                        item3->variable->data_type = DATA_TYPE_DOUBLE;
                                        item3->variable->value.desetinne_cislo = dbl1 / dbl2;
                                    } else {
                                        if (targetType == DATA_TYPE_INT)
                                            return ReturnError (10);
                                        else if (targetType == DATA_TYPE_DOUBLE)
                                            item3->variable->value.desetinne_cislo = dbl1 / dbl2;
                                        else {
                                            // nepodporovany datovy typ
                                            return ReturnError (10);
                                        }
                                    }
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                if(!found3)
                                {
                                    free(item3);
                                    item3 = NULL;
                                }
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // Nespravny typ instrukce
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_LT:      // less than
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    switch(instr->Act->instType) {
                        case INST_INT:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true;  
                                if(int1 < int2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }

                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));

                                if(item3 == NULL)
                                    return ReturnError (99);

                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));

                                if(item3->variable == NULL)
                                    return ReturnError (99);

                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true; 

                                if(dbl1 < dbl2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // nepodporovany datovy typ
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_GT:      // greater than
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    switch(instr->Act->instType) {
                        case INST_INT:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true; 
                                if(int1 > int2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true;  
                                if(dbl1 > dbl2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // nepodporovany datovy typ
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_LEQ:     // less or equal
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    switch(instr->Act->instType) {
                        case INST_INT:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true;  
                                if(int1 <= int2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true;  
                                if(dbl1 <= dbl2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // nepodporovany datovy typ
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_GEQ:     // greater or equal
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    switch(instr->Act->instType) {
                        case INST_INT:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true;  
                                if(int1 >= int2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true; 
                                if(dbl1 >= dbl2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // nepodporovany datovy typ
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_EQ:      // equal
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    switch(instr->Act->instType) {
                        case INST_INT:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true;  
                                if(int1 == int2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true; 
                                if(dbl1 == dbl2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // nepodporovany datovy typ
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_NEQ:     // not equal
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    switch(instr->Act->instType) {
                        case INST_INT:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_INT CONST
                                int1 = instr->Act->value1.cele_cislo;
                                found1 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        int1 = item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int1 = (int)item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }

                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_INT CONST
                                int2 = instr->Act->value2.cele_cislo;
                                found2 = true;
                            } else {  // INST_INT VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        int2 = item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        int2 = (int)item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true;  
                                if(int1 != int2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        case INST_DOUBLE:
                            // hledani hodnoty prvniho operandu
                            if(instr->Act->operandType1 == CONST) {  // INST_DOUBLE CONST
                                dbl1 = instr->Act->value1.desetinne_cislo;
                                found1 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                                    found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                                } else {  // je to ID
                                    found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                                }
                                if(found1 && item1->variable->init) {
                                    if (item1->variable->data_type == DATA_TYPE_INT) {
                                        dbl1 = (double)item1->variable->value.cele_cislo;
                                    } else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl1 = item1->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            // hledani hodnoty druheho operandu
                            if(instr->Act->operandType2 == CONST) {  // INST_DOUBLE CONST
                                dbl2 = instr->Act->value2.desetinne_cislo;
                                found2 = true;
                            } else {  // INST_DOUBLE VAR
                                if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                                    found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                                } else {  // je to ID
                                    found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                                }
                                if(found2 && item2->variable->init) {
                                    if (item2->variable->data_type == DATA_TYPE_INT) {
                                        dbl2 = (double)item2->variable->value.cele_cislo;
                                    } else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                        dbl2 = item2->variable->value.desetinne_cislo;
                                    } else {
                                        // nepodporovany datovy typ
                                        return ReturnError (10);
                                    }
                                } else {
                                    // potrebna hodnota nenalezena
                                    return ReturnError (8);
                                }
                            }
                            if(found1 && found2) {
                                item3 = malloc(sizeof(struct tSymTable));
                                if(item3 == NULL)
                                    return ReturnError (99);
                                item3->type = TABLE_VARIABLE;
                                item3->name = instr->Act->value3.retezec;
                                item3->variable = malloc(sizeof(struct dataVariable));
                                if(item3->variable == NULL)
                                    return ReturnError (99);
                                item3->variable->data_type = DATA_TYPE_BOOL;
                                item3->variable->init = true; 
                                if(dbl1 != dbl2) {
                                    item3->variable->value.prav_hodnota = true;
                                } else {
                                    item3->variable->value.prav_hodnota = false;
                                }
                                if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                                    BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                                } else {  // je to ID
                                    BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                                }
                                free(item3);
                                item3 = NULL;
                            } else {
                                // potrebna hodnota nenalezena
                                return ReturnError (8);
                            }
                            break;
                        default:
                            // nepodporovany datovy typ
                            return ReturnError (10);
                            break;
                    }
                }
                break;
            }

            case I_READINT: // read int
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    int1 = readInt();
                    if(int1<0) {
                        // chyba pri nacitani hodnoty
                        return ReturnError (7);
                    }
                    item3 = malloc(sizeof(struct tSymTable));
                    if(item3 == NULL)
                        return ReturnError (99);
                    item3->type = TABLE_VARIABLE;
                    item3->variable = malloc(sizeof(struct dataVariable));
                    if(item3->variable == NULL)
                        return ReturnError (99);
                    item3->variable->data_type = DATA_TYPE_INT;
                    item3->variable->init = true;  
                    item3->name = instr->Act->value3.retezec;
                    item3->variable->value.cele_cislo = int1;
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                        BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                    } else {  // je to ID
                        BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                    }
                    free(item3);
                    item3 = NULL;
                }
                break;
            }

            case I_READDBL: // read double
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    dbl1 = readDouble();
                    if(dbl1<0) {
                        // chyba pri nacitani hodnoty
                        return ReturnError (7);
                    }
                    item3 = malloc(sizeof(struct tSymTable));
                    if(item3 == NULL)
                        return ReturnError (99);
                    item3->type = TABLE_VARIABLE;
                    item3->variable = malloc(sizeof(struct dataVariable));
                    if(item3->variable == NULL)
                        return ReturnError (99);
                    item3->variable->data_type = DATA_TYPE_DOUBLE;
                    item3->variable->init = true;  
                    item3->name = instr->Act->value3.retezec;
                    item3->variable->value.desetinne_cislo = dbl1;
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                        BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                    } else {  // je to ID
                        BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                    }
                    free(item3);
                    item3 = NULL;
                }
                break;
            }

            case I_READSTR: // read string
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    str1 = readString();
                    if(str1 == NULL) {
                        // chyba pri nacitani hodnoty
                        return ReturnError (7);
                    }
                    item3 = malloc(sizeof(struct tSymTable));
                    if(item3 == NULL)
                        return ReturnError (99);
                    item3->type = TABLE_VARIABLE;
                    item3->variable = malloc(sizeof(struct dataVariable));
                    if(item3->variable == NULL)
                        return ReturnError (99);
                    item3->variable->data_type = DATA_TYPE_STRING;
                    item3->variable->init = true;  
                    item3->name = instr->Act->value3.retezec;
                    item3->variable->value.retezec = str1;
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                        BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                    } else {  // je to ID
                        BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                    }
                    free(item3);
                    item3 = NULL;
                }
                break;
            }

            case I_PRINT:   // print
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    if (!BTSearch(TSstack.a[TSstack.top], instr->Act->value3.retezec, &item1))
                       return ReturnError (99);
                    if (item1->variable->data_type == DATA_TYPE_INT)
                        printf ("%d", item1->variable->value.cele_cislo);
                    if (item1->variable->data_type == DATA_TYPE_DOUBLE)
                        printf ("%g", item1->variable->value.desetinne_cislo);
                    if (item1->variable->data_type == DATA_TYPE_STRING)
                        printf ("%s", convert_str(item1->variable->value.retezec));
                }
                break;
            }

            case I_LEN:     // length
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    if(instr->Act->operandType1 == CONST) {
                        int1 = length(instr->Act->value1.retezec);
                    } else {  // VAR
                        if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                            found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                        } else {  // je to ID
                            found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                        }
                        if(found1 && item1->variable->init) {
                            int1 = length(item1->variable->value.retezec);
                        } else {
                            // potrebna hodnota nenalezena
                            return ReturnError (8);
                        }
                    }
                    if(int1 < 0) {
                        // chyba delky retezce
                        return ReturnError (10);
                    }
                    item3 = malloc(sizeof(struct tSymTable));
                    if(item3 == NULL)
                        return ReturnError (99);
                    item3->type = TABLE_VARIABLE;
                    item3->variable = malloc(sizeof(struct dataVariable));
                    if(item3->variable == NULL)
                        return ReturnError (99);
                    item3->variable->data_type = DATA_TYPE_INT;
                    item3->variable->init = true;  
                    item3->variable->value.cele_cislo = int1;
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                        BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                    } else {  // je to ID
                        BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                    }
                    free(item3);
                    item3 = NULL;
                }
                break;
            }

            case I_SUBSTR:  // substring
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    if (!BTSearch (TSstack.a[TSstack.top], "s", &item1))
                        return ReturnError (8);
                    if (!BTSearch (TSstack.a[TSstack.top], "i", &item2))
                        return ReturnError (8);
                    if (!BTSearch (TSstack.a[TSstack.top], "n", &item3))
                        return ReturnError (8);

                    str1 = substr (item1->variable->value.retezec, item2->variable->value.cele_cislo, item3->variable->value.cele_cislo);
                    if (str1 == NULL)
                        return ReturnError (10);

                    tSymTablePtr lokTab = NULL;
                    if (!BTSearch (TSstack.a[TSstack.top], "n", &lokTab))
                    {
                        lokTab = malloc (sizeof(struct tSymTable));
                        if (lokTab == NULL)
                            return ReturnError (99);

                        dataVariablePtr data = malloc (sizeof(struct dataVariable));
                        if (data == NULL)
                            return ReturnError (99);

                        lokTab->name = "#tmp";
                        lokTab->function = NULL;
                        lokTab->variable = data;
                        lokTab->type = TABLE_VARIABLE;
                    }

                    lokTab->variable->data_type = DATA_TYPE_STRING;
                    lokTab->variable->value.retezec = str1;
                    lokTab->variable->init = true;

                    if (!BTInsert (&(TSstack.a[TSstack.top]), "#tmp", *lokTab))
                        return ReturnError (99);
                    free (lokTab);
                    lokTab = NULL;
                }
                break;
            }

            case I_CMP:     // compare
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    if(instr->Act->operandType1 == CONST) {  // hledam prvni retezec
                        str1 = instr->Act->value1.retezec;
                    } else {  // VAR
                        if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                            found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                        } else {  // je to ID
                            found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                        }
                        if(found1 && item1->variable->init) {
                            str1 = item1->variable->value.retezec;
                        } else {
                            // potrebna hodnota nenalezena
                            return ReturnError (8);
                        }
                    }
                    if(instr->Act->operandType2 == CONST) {  // hledam druhy retezec
                        str2 = instr->Act->value2.retezec;
                    } else {  // VAR
                        if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                            found1 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                        } else {  // je to ID
                            found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                        }
                        if(found1 && item1->variable->init) {
                            str2 = item2->variable->value.retezec;
                        } else {
                            // potrebna hodnota nenalezena
                            return ReturnError (8);
                        }
                    }
                    int1 = compare(str1, str2);
                    item3 = malloc(sizeof(struct tSymTable));
                    if(item3 == NULL)
                        return ReturnError (99);
                    item3->type = TABLE_VARIABLE;
                    item3->variable = malloc(sizeof(struct dataVariable));
                    if(item3->variable == NULL)
                        return ReturnError (99);
                    item3->variable->data_type = DATA_TYPE_INT;
                    item3->variable->init = true;  
                    item3->variable->value.cele_cislo = int1;
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                        BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                    } else {  // je to ID
                        BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                    }
                    free(item3);
                    item3 = NULL;
                }
                break;
            }

            case I_FIND:    // find
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    if(instr->Act->operandType1 == CONST) {  // hledam prvni retezec
                        str1 = instr->Act->value1.retezec;
                    } else {  // VAR
                        if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                            found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                        } else {  // je to ID
                            found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                        }
                        if(found1 && item1->variable->init) {
                            str1 = item1->variable->value.retezec;
                        } else {
                            // potrebna hodnota nenalezena
                            return ReturnError (8);
                        }
                    }
                    if(instr->Act->operandType2 == CONST) {  // hledam druhy retezec
                        str2 = instr->Act->value2.retezec;
                    } else {  // VAR
                        if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                            found1 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                        } else {  // je to ID
                            found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                        }
                        if(found1 && item1->variable->init) {
                            str2 = item2->variable->value.retezec;
                        } else {
                            // potrebna hodnota nenalezena
                            return ReturnError (8);
                        }
                    }
                    int1 = find(str1, str2);
                    item3 = malloc(sizeof(struct tSymTable));
                    if(item3 == NULL)
                        return ReturnError (99);
                    item3->type = TABLE_VARIABLE;
                    item3->variable = malloc(sizeof(struct dataVariable));
                    if(item3->variable == NULL)
                        return ReturnError (99);
                    item3->variable->data_type = DATA_TYPE_INT;
                    item3->variable->init = true; 
                    item3->variable->value.cele_cislo = int1;
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                        BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                    } else {  // je to ID
                        BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                    }
                    free(item3);
                    item3 = NULL;
                }
                break;
            }

            case I_SORT:    // sort
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    if(instr->Act->operandType1 == CONST) {  // hledam prvni retezec
                        str1 = instr->Act->value1.retezec;
                    } else {  // VAR
                        if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                            found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                        } else {  // je to ID
                            found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                        }
                        if(found1 && item1->variable->init) {
                            str1 = item1->variable->value.retezec;
                        } else {
                            // potrebna hodnota nenalezena
                            return ReturnError (8);
                        }
                    }
                    str2 = sort(str1);
                    item3 = malloc(sizeof(struct tSymTable));
                    if(item3 == NULL)
                        return ReturnError (99);
                    item3->type = TABLE_VARIABLE;
                    item3->variable = malloc(sizeof(struct dataVariable));
                    if(item3->variable == NULL)
                        return ReturnError (99);
                    item3->variable->data_type = DATA_TYPE_STRING;
                    item3->variable->init = true;  
                    item3->variable->value.retezec = str2;
                    if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                        BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                    } else {  // je to ID
                        BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                    }
                    free(item3);
                    item3 = NULL;
                }
                break;
            }

            case I_LABEL:   // label
                break;

            case I_GOTO:    // unconditional jump
            {
                if (pruchod == 2)
                {
                    if (!GotoInst (instr, instr->Act->value3.retezec))
                        return ReturnError (99);
                }
                break;
            }

            case I_JMPN:     // conditional jump
            {
                if (pruchod == 2)
                {
                    if(strchr(instr->Act->value1.retezec, '.') != NULL)  // je to PKID
                    {
                        if (!BTSearch(Global_TS, instr->Act->value1.retezec, &item1))
                            return ReturnError (99); // nenalezeno v tabulce
                    }
                    else // je to ID
                    {
                        if (!BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1))
                            return ReturnError (99); // nenalezeno v tabulce
                    }

                    if (!(item1->variable->value.prav_hodnota))
                    {
                        if (!GotoInst (instr, instr->Act->value3.retezec))
                            return ReturnError (99); // navesti nenalezeno
                    }
                }
                break;
            }

            case I_ASSIGN:  // assign
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    if(strchr(instr->Act->value3.retezec, '.') != NULL)  // je to PKID
                        found3 = BTSearch(Global_TS, instr->Act->value3.retezec, &item3);
                    else // je to ID 
                        found3 = BTSearch(TSstack.a[TSstack.top], instr->Act->value3.retezec, &item3);

                    if (found3)
                    {
                        if(strchr(item3->name, '#') != NULL)
                        {
                            item3->variable->data_type = translateType(instr->Act->instType);
                        }

                        if ((instr->Act->operandType2 == CONST) && (item3->variable->data_type != instr->Act->value2.cele_cislo))
                        {
                            item3->variable->data_type = instr->Act->value2.cele_cislo;
                        }
                    }
                    else
                    {
                        item3 = malloc(sizeof(struct tSymTable));
                        if(item3 == NULL)
                        {
                            return ReturnError (99);
                        }
                        item3->name = instr->Act->value3.retezec;
                        item3->type = TABLE_VARIABLE;
                        item3->variable = malloc(sizeof(struct dataVariable));
                        if(item3->variable == NULL)
                        {
                            return ReturnError (99);
                        }

                        if (instr->Act->operandType2 == CONST)
                            item3->variable->data_type = instr->Act->value2.cele_cislo; 
                        else{
                            item3->variable->data_type = translateType(instr->Act->instType);
                        }

                    }
                    item3->variable->init = true;  

                    if (instr->Act->operandType1 == CONST)
                    {
                        if (item3->variable->data_type == translateType(instr->Act->instType))
                        {
                            item3->variable->value = instr->Act->value1;
                        }
                        else if (item3->variable->data_type == DATA_TYPE_INT)
                            return ReturnError (4); 
                        else 
                            item3->variable->value.desetinne_cislo = (double) instr->Act->value1.cele_cislo;
                    }
                    else if (instr->Act->operandType1 == VAR)
                    {
                        if(strchr(instr->Act->value1.retezec, '.') != NULL)  // je to PKID
                            found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                        else // je to ID 
                            found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                        if (!found1)
                        {
                            if (pruchod == 1)
                                return ReturnError (6);
                            else
                                return ReturnError (8);
                        }

                        else
                        {
                            if (!item1->variable->init)
                            {
                                // potrebna hodnota nenalezena
                                if (pruchod == 1)
                                    return ReturnError (6);
                                else
                                    return ReturnError (8);
                            }
                            else
                            {
                                if (item3->variable->data_type == item1->variable->data_type){
                                    item3->variable->value = item1->variable->value;
                                }
                                else if (item3->variable->data_type == DATA_TYPE_INT)
                                    return ReturnError (4); 
                                else
                                    item3->variable->value.desetinne_cislo = (double) item1->variable->value.cele_cislo;
                            }
                        }
                    }
                    else
                    {
                        // spatny typ operandu
                        return ReturnError (10);
                    }

                    if (!found3)
                    {
                        if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                            BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                        } else {  // je to ID
                            BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                        }
                        free(item3);
                        item3 = NULL;
                    }
                }
                break;
            }

            case I_RET:     // return from function
            {
                if (pruchod == 2)
                {
                    if (!SRetEmpty (&retStack))
                    {
                        // Zjistuje se, zda ma funkce vracet hodnotu
                        if (retStack.retVal[retStack.top]->retType != INST_VOID)
                        {
                            // Kontrola, ze se vysledek volani funkce prirazuje do promenne
                            if (retStack.retVal[retStack.top]->retVar != NULL)
                            {
                                if (!BTSearch(TSstack.a[TSstack.top], "#tmp", &item1))               
                                    return ReturnError (99);    // promenna tmp neni v tabulce

                                // Nazev promenne do ktere se prirazuje vysledek volani funkce
                                char* retVariable = retStack.retVal[retStack.top]->retVar;
                                int retType = retStack.retVal[retStack.top]->retType; // datovy typ, ktery funkce vraci

                                if (strchr(retVariable, '.') != NULL) // PKID -> globalni TS
                                {
                                    if (BTSearch(Global_TS, retVariable, &item3))
                                    {
                                        if (item3->variable->data_type == DATA_TYPE_INT) // promenna, do ktere se prirazuje, je typu int
                                        {
                                            if (retType == INST_STRING) // funkce vraci string
                                                return ReturnError (99);

                                            else // funkce vraci int/double
                                            {
                                                if (item1->variable->data_type == DATA_TYPE_INT) // #tmp je typu int
                                                    item3->variable->value.cele_cislo = item1->variable->value.cele_cislo;
                                                else // #tmp je typu double/string
                                                    return ReturnError (4);
                                            }
                                        }

                                        if (item3->variable->data_type == DATA_TYPE_DOUBLE) // promenna, do ktere se prirazuje, je double
                                        {
                                            if (retType == INST_STRING) // funkce vraci string
                                                return ReturnError (99);

                                            else // funkce vraci int/double
                                            {
                                                if (item1->variable->data_type == DATA_TYPE_INT) // #tmp je typu int
                                                    item3->variable->value.desetinne_cislo = (double)item1->variable->value.cele_cislo;
                                                else if (item1->variable->data_type == DATA_TYPE_DOUBLE) // #tmp je typu double
                                                    item3->variable->value.desetinne_cislo = item1->variable->value.desetinne_cislo;
                                                else    // #tmp je typu string
                                                    return ReturnError (4);
                                            }
                                        }

                                        if (item3->variable->data_type == DATA_TYPE_STRING) // promenna, do ktere se prirazuje, je string
                                        {
                                            if (retType == INST_STRING) // funkce vraci String
                                            {
                                                if (item1->variable->data_type == DATA_TYPE_STRING) // #tmp je typu string
                                                    item3->variable->value.retezec = item1->variable->value.retezec;
                                                else // #tmp je int/double
                                                    return ReturnError (4);
                                            }
                                            else // funkce vraci int/double
                                                return ReturnError (4);
                                        }

                                        item3->variable->init = true;
                                    }
                                    else // Promenna do ktere se vraci vysledek volani funkce jeste neni v globalni TS
                                    {
                                        dataVariablePtr data = malloc (sizeof(struct dataVariable));
                                        if (data == NULL)
                                            return ReturnError (99); // selhani alokace
                                  
                                        tSymTablePtr symTab = malloc (sizeof(struct tSymTable));
                                        if (symTab == NULL)
                                            return ReturnError (99); // selhani alokace
                                        
                                        symTab->type = TABLE_VARIABLE;
                                        symTab->function = NULL;
                                        symTab->variable = data;
                                        symTab->name = retVariable;

                                        data->data_type = translateType (retType);
                                        if (retType == INST_INT) //fce vraci int
                                        {
                                            if (item1->variable->data_type == DATA_TYPE_INT) // #tmp je int
                                                data->value.cele_cislo = item1->variable->value.cele_cislo;
                                            else // #tmp je double/string
                                                return ReturnError (4); 
                                        }
                                        else if (retType == INST_DOUBLE) // fce vraci double
                                        {
                                            if (item1->variable->data_type == DATA_TYPE_INT) // #tmp je int
                                                data->value.desetinne_cislo = (double)item1->variable->value.cele_cislo;
                                            else if (item1->variable->data_type == DATA_TYPE_DOUBLE) // #tmp je double
                                                data->value.desetinne_cislo = item1->variable->value.desetinne_cislo;
                                            else // #tmp je string
                                                return ReturnError (4); 
                                        }
                                        else if (retType == INST_STRING) // fce vraci string
                                        {
                                            if (item1->variable->data_type == DATA_TYPE_STRING) // #tmp je string
                                                data->value.retezec = item1->variable->value.retezec;
                                            else  // #tmp je int/double
                                                return ReturnError (4); 
                                        }

                                        data->init = true;
                                        if (!BTInsert(&Global_TS, symTab->name, *symTab))
                                            return ReturnError (99);
         
                                    }
                                }
                                else // ID -> lokalni TS
                                {
                                    if (BTSearch(TSstack.a[TSstack.top-1], retVariable, &item3)) 
                                    {
                                        if (item3->variable->data_type == DATA_TYPE_INT) // promenna, do ktere se prirazuje, je int
                                        {
                                            if (retType == INST_STRING) // funkce vraci string
                                                return ReturnError (99);

                                            else // funkce vraci int/double
                                            {
                                                if (item1->variable->data_type == DATA_TYPE_INT) // #tmp je typu int
                                                    item3->variable->value.cele_cislo = item1->variable->value.cele_cislo;
                                                else // #tmp je typu double/string
                                                    return ReturnError (4); 
                                            }
                                        }

                                        if (item3->variable->data_type == DATA_TYPE_DOUBLE) // promenna, do ktere se prirazuje, je double
                                        {
                                            if (retType == INST_STRING) // funkce vraci string
                                                return ReturnError (4);

                                            else // funkce vraci int/double
                                            {
                                                if (item1->variable->data_type == DATA_TYPE_INT) // #tmp je typu int
                                                    item3->variable->value.desetinne_cislo = (double)item1->variable->value.cele_cislo;
                                                else if (item1->variable->data_type == DATA_TYPE_DOUBLE) // #tmp je typu double
                                                    item3->variable->value.desetinne_cislo = item1->variable->value.desetinne_cislo;
                                                else // #tmp je string
                                                    return ReturnError (4);
                                            }
                                        }

                                        if (item3->variable->data_type == DATA_TYPE_STRING) // promenna, do ktere se prirazuje, je string
                                        {
                                            if (retType == INST_STRING) // funkce vraci String
                                            {
                                                if (item1->variable->data_type == DATA_TYPE_STRING) // #tmp je typu string
                                                    item3->variable->value.retezec = item1->variable->value.retezec;
                                                else // #tmp je int/double
                                                    return ReturnError (4);
                                            }
                                            else // funkce vraci int/double
                                                return ReturnError (4);
                                        }

                                        item3->variable->init = true;
                                    }
                                    else // Promenna do ktere se prirazuje vysledek volani funkce jeste neni v lokalni TS
                                    {
                                        dataVariablePtr data = malloc (sizeof(struct dataVariable));
                                        if (data == NULL)
                                            return ReturnError (99); // selhani alokace
                                  
                                        tSymTablePtr symTab = malloc (sizeof(struct tSymTable));
                                        if (symTab == NULL)
                                            return ReturnError (99); // selhani alokace
                                        
                                        symTab->type = TABLE_VARIABLE;
                                        symTab->function = NULL;
                                        symTab->variable = data;
                                        symTab->name = retVariable;

                                        data->data_type = translateType (retType);  // promenna je datoveho typu, ktery funkce vraci
                                        if (retType == INST_INT) //fce vraci int
                                        {
                                            if (item1->variable->data_type == DATA_TYPE_INT) // #tmp je int
                                                data->value.cele_cislo = item1->variable->value.cele_cislo;
                                            else // #tmp je double/string
                                                return ReturnError (4); 
                                        }
                                        else if (retType == INST_DOUBLE) // fce vraci double
                                        {
                                            if (item1->variable->data_type == DATA_TYPE_INT) // #tmp je int
                                                data->value.desetinne_cislo = (double)item1->variable->value.cele_cislo;
                                            else if (item1->variable->data_type == DATA_TYPE_DOUBLE) // #tmp je double
                                                data->value.desetinne_cislo = item1->variable->value.desetinne_cislo;
                                            else // #tmp je string
                                                return ReturnError (4); 
                                        }
                                        else if (retType == INST_STRING) // fce vraci string
                                        {
                                            if (item1->variable->data_type == DATA_TYPE_STRING) // #tmp je string
                                                data->value.retezec = item1->variable->value.retezec;
                                            else  // #tmp je int/double
                                                return ReturnError (4); 
                                        }

                                        data->init = true;
                                        if (!BTInsert(&(TSstack.a[TSstack.top-1]), symTab->name, *symTab))
                                            return ReturnError (99);
                                    }
                                }
                            }
                        }
                    }
                    else // Jedna se o return ve funkci Main.run
                    {
                        return true; // program se cely zpracoval
                    }

                    tBTNodePtr tmpTS = STopPop (&TSstack);
                    free(tmpTS);
                    tmpTS = NULL;

                    tRetValPtr tmp = SRetTopPop (&retStack);


                    ActiveInst (instr, tmp->a); // nastaveni navratu na instrukci, odkud se volala funkce
                    free (tmp);
                    tmp = NULL;
                }
                break;
            }

            case I_CONCAT:  // concatenate strings
            {
                if ((pruchod == 1 && declarationMode == true) || pruchod == 2)
                {
                    if(instr->Act->operandType1 == CONST) {  // hleda se prvni retezec
                        str1 = instr->Act->value1.retezec;
                        found1 = true;
                    } else {  // VAR
                        if(strchr(instr->Act->value1.retezec, '.') != NULL) {  // je to PKID
                            found1 = BTSearch(Global_TS, instr->Act->value1.retezec, &item1);
                        } else {  // je to ID
                            found1 = BTSearch(TSstack.a[TSstack.top], instr->Act->value1.retezec, &item1);
                        }
                        if(found1 && item1->variable->init) {
                            if (item1->variable->data_type == DATA_TYPE_STRING)
                                str1 = item1->variable->value.retezec;
                            else if (item1->variable->data_type == DATA_TYPE_INT) {
                                char *pomoc = malloc (15);
                                if (pomoc == NULL) {
                                    return ReturnError (99);
                                }
                                sprintf (pomoc, "%d", item1->variable->value.cele_cislo);
                                str1 = pomoc;
                            }
                            else if (item1->variable->data_type == DATA_TYPE_DOUBLE) {
                                char *pomoc = malloc (15);
                                if (pomoc == NULL) {
                                    return ReturnError (99);
                                }
                                sprintf (pomoc, "%g", item1->variable->value.desetinne_cislo);
                                str1 = pomoc;
                            }
                            else
                            {
                                // nepodporovany datovy typ
                                return ReturnError (10);
                            }
                        } else {
                            // potrebna hodnota nenalezena
                            return ReturnError (8);
                        }
                    }
                    if(instr->Act->operandType2 == CONST) {  // hleda se druhy retezec
                        str2 = instr->Act->value2.retezec;
                        found2 = true;
                    } else {  // VAR
                        if(strchr(instr->Act->value2.retezec, '.') != NULL) {  // je to PKID
                            found2 = BTSearch(Global_TS, instr->Act->value2.retezec, &item2);
                        } else {  // je to ID
                            found2 = BTSearch(TSstack.a[TSstack.top], instr->Act->value2.retezec, &item2);
                        }
                        if(found2 && item2->variable->init) {
                            if (item2->variable->data_type == DATA_TYPE_STRING)
                                str2 = item2->variable->value.retezec;
                            else if (item2->variable->data_type == DATA_TYPE_INT) {
                                char *pomoc = malloc (15);
                                if (pomoc == NULL) {
                                    return ReturnError (99);
                                }
                                sprintf (pomoc, "%d", item2->variable->value.cele_cislo);
                                str2 = pomoc;
                            }
                            else if (item2->variable->data_type == DATA_TYPE_DOUBLE) {
                                char *pomoc = malloc (15);
                                if (pomoc == NULL) {
                                    return ReturnError (99);
                                }
                                sprintf (pomoc, "%g", item2->variable->value.desetinne_cislo);
                                str2 = pomoc;
                            }
                            else
                            {
                                // nepodporovany datovy typ
                                return ReturnError (10);
                            }
                        } else {
                            // potrebna hodnota nenalezena
                            return ReturnError (8);
                        }
                    }

                    if (found1 && found2)
                    {
                        str3 = malloc(strlen(str1) + strlen(str2) + 1);
                        strcpy(str3, str1);
                        strcat(str3, str2);
                        str3[strlen(str1) + strlen(str2)] = 0;
                        if(str3 == NULL)  //chyba pri mallocu
                            return ReturnError (99);

                        if (strchr(instr->Act->value3.retezec, '.') != NULL )
                            found3 = BTSearch(Global_TS, instr->Act->value3.retezec, &item3);
                        else
                            found3 = BTSearch(TSstack.a[TSstack.top], instr->Act->value3.retezec, &item3);

                        if (!found3) {
                            item3 = malloc(sizeof(struct tSymTable));
                            if(item3 == NULL)
                                return ReturnError (99);
                            item3->type = TABLE_VARIABLE;
                            item3->name = instr->Act->value3.retezec;
                            item3->variable = malloc(sizeof(struct dataVariable));
                            if(item3->variable == NULL)
                                return ReturnError (99);
                            item3->variable->init = true;  
                            item3->variable->data_type = DATA_TYPE_STRING;
                            item3->variable->value.retezec = str3;
                        }
                        else {
                            if (strchr(instr->Act->value3.retezec, '#') != NULL)
                                retype = true;
                            else
                                retype = false;

                            // Hodnota se uklada do jiz existujici promenne typu #tmpN
                            if(retype) {  
                                item3->variable->data_type = DATA_TYPE_STRING;
                            }
                            item3->variable->value.retezec = str3;

                        }
                        if(strchr(instr->Act->value3.retezec, '.') != NULL) {  // je to PKID
                            BTInsert(&Global_TS, instr->Act->value3.retezec, *item3);
                        } else {  // je to ID
                            BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value3.retezec, *item3);
                        }
                        if (!found3)
                        {
                            free(item3);
                            item3 = NULL;
                        }
                    }
                }
                break;
            }

            case I_FUNC:    // function
            {
                if (pruchod == 2)
                {                    
                    tRetValPtr retVal = malloc (sizeof(struct tRetVal));
                    if (retVal == NULL)
                        return ReturnError (99); // selhani alokace

                    retVal->retType = instr->Act->instType;
                    if (instr->Act->operandType3 == VAR)
                        retVal->retVar = instr->Act->value3.retezec;
                    else
                        retVal->retVar = NULL;

                    SRetPush(&retStack, retVal);
                    tBTNodePtr Local_TS;   // vytvoreni nove lokalni TS
                    BTInit(&Local_TS);
                    SPush(&TSstack, Local_TS);
                }
                break;
            }

            case I_PARAM:   // fucntion parameters
            {
                if (pruchod == 2)
                {
                    dataVariablePtr data = malloc (sizeof(struct dataVariable));
                    if (data == NULL)
                        return ReturnError (99); // selhani alokace
                    tSymTablePtr lokTab = malloc (sizeof(struct tSymTable));
                    if (lokTab == NULL)
                        return ReturnError (99); // selhani alokace
                    lokTab->type = TABLE_VARIABLE;
                    lokTab->function = NULL;
                    lokTab->variable = data;
                    lokTab->name = instr->Act->value1.retezec;

                    // Pokud je operand 2 konstanta, pristupuje se primo k hodnotam unionu
                    if (instr->Act->operandType2 == CONST)
                    {
                        if (instr->Act->instType == INST_INT)
                        {
                            data->data_type = DATA_TYPE_INT;
                            data->value.cele_cislo = instr->Act->value2.cele_cislo;
                        }
                        if (instr->Act->instType == INST_DOUBLE)
                        {
                            data->data_type = DATA_TYPE_DOUBLE;
                            data->value.desetinne_cislo = instr->Act->value2.desetinne_cislo;
                        }
                        if (instr->Act->instType == INST_STRING)
                        {
                            data->data_type = DATA_TYPE_STRING;
                            data->value.retezec = instr->Act->value2.retezec;
                        }
                        data->init = true;
                    }

                    // Pokud je operand 2 promenna, musi se vyhledat v TS
                    if (instr->Act->operandType2 == VAR)
                    {
                        if(strchr(instr->Act->value2.retezec, '.') != NULL) // PKID -> globalni TS
                        {
                            if (!BTSearch(Global_TS, instr->Act->value2.retezec, &item2))
                                return ReturnError (8); 
                        }
                        else // je to ID -> lokalni tabulka
                        {
                            // Pouzije se tabulka, ktera je na zasobniku o jedna niz, protoze na vrcholu zasobniku
                            // jiz je TS volane funkce.
                            if (!BTSearch(TSstack.a[TSstack.top-1], instr->Act->value2.retezec, &item2))
                                return ReturnError (8); 
                        }
                        if (item2->variable->data_type == DATA_TYPE_INT)
                        {
                            data->data_type = DATA_TYPE_INT;
                            data->value.cele_cislo = item2->variable->value.cele_cislo;
                        }
                        if (item2->variable->data_type == DATA_TYPE_DOUBLE)
                        {
                            data->data_type = DATA_TYPE_DOUBLE;
                            data->value.desetinne_cislo = item2->variable->value.desetinne_cislo;
                        }
                        if (item2->variable->data_type == DATA_TYPE_STRING)
                        {
                            data->data_type = DATA_TYPE_STRING;
                            data->value.retezec = item2->variable->value.retezec;
                        }
                        data->init = true;
                    }

                    if (!BTInsert(&(TSstack.a[TSstack.top]), instr->Act->value1.retezec, *lokTab))
                        return ReturnError (99);

                    free (lokTab);
                    lokTab = NULL;
                }
                break;
            }

            case I_FSTART:  // push return instruction, goto function
            {   
                if (pruchod == 2)
                {
                    retStack.retVal[retStack.top]->a = instr->Act;
                    if (!GotoInst(instr, instr->Act->value3.retezec)){
                        return ReturnError (99);
                    }
                }
                break;
            }

            case I_END:     // end of program
            {
                if (pruchod == 1)
                {
                    // Pruchod 1 dorazil na konec instrukcni pasky, zacina pruchod 2 na funci Main.run.
                    pruchod = 2;
                    if (!GotoInst (instr, "Main.run"))
                        return ReturnError (99);
                }
                break;
            }

            case I_FEND:    // end of function
            {
                if (pruchod == 2)
                {
                    tRetValPtr tmp = SRetTopPop (&retStack);

                    tBTNodePtr tmpTS = STopPop (&TSstack);
                    free (tmpTS);
                    tmpTS = NULL;

                    if(strcmp (instr->Act->value1.retezec, "Main.run") != 0) 
                    {
                        if (tmp->retType != INST_VOID) // u nonvoid funkce chybi return -> chyba 8
                            return ReturnError (8);

                        ActiveInst (instr, tmp->a); // nastaveni navratu na instrukci, odkud se volala funkce
                        free (tmp);
                        tmp = NULL;
                    }
                    else // ukonceni funkce Main.run -> konec programu, zasobnik je jiz prazdny
                    {
                        return true; // program se cely zpracoval
                    }
                }
                break;
            }

            case I_DECSTART: // Zacatek sekce deklaraci statickych promennych
            {
                declarationMode = true;
                break;
            }

            case I_DECEND: // Konec sekce deklaraci statickych promennych
            {
                declarationMode = false;
                break;
            }

            default:
                return ReturnError (99); // nedefinovana instrukce
        }
        // Nacteni dalsi instrukce
        NextInst (instr);
    }
    if (instr->Act == NULL)
        return ReturnError (99);
return true;
}

// funkce pracujici se seznamem instrukci
void InitInstList (tLInst *LInst)
{
    LInst->First = NULL;
    LInst->Act = NULL;
    LInst->Last = NULL;
    LInst->Return = NULL;
}
void DisposeInstList (tLInst *LInst)
{
    tInstPtr tmp;
    tInstPtr del;
    tmp = LInst->First;         // pomocny ukazatel na zacatek seznamu
    while (tmp != NULL)     // cyklus prochazi seznam a postupne rusi prvky
    {
        del = tmp;
        tmp = tmp->rptr;
        free(del);
        del = NULL;
    }
    LInst->First = NULL;        // nastaveni ukazatele na prvni, posledni a aktivni prvek na NULL
    LInst->Act = NULL;
    LInst->Last = NULL;
    LInst->Return = NULL;
}

bool InsertInst (tLInst *LInst, struct tInst instr)
{
    tInstPtr tmp = (tInstPtr) malloc(sizeof(struct tInst));  // alokace prvku
    if (tmp == NULL)
        return false;
    else
    {
        tmp->name = instr.name; // naplneni prvku hodnotou, nastaveni nasledovnika na NULL, predchudce na posledni prvek
        tmp->instType = instr.instType;
        tmp->operandType1 = instr.operandType1;
        tmp->operandType2 = instr.operandType2;
        tmp->operandType3 = instr.operandType3;
        if (instr.operandType1 != NDEF)
            tmp->value1 = instr.value1;
        if (instr.operandType2 != NDEF)
            tmp->value2 = instr.value2;
        if (instr.operandType3 != NDEF)
            tmp->value3 = instr.value3;
        tmp->rptr = NULL;
        tmp->lptr = LInst->Last;
        if (LInst->First == NULL)   // pokud seznam nema prvni prvek (je prazdny), stane se nove tvoreny prvek prvnim
            LInst->First = tmp;
        else
            LInst->Last->rptr = tmp; // puvodne posledni prvek se bude mit ukazatel nasledovnika na novy prvek
        LInst->Last = tmp;          // novy prvek se stane poslednim
        return true;
    }
}

void ActiveInst (tLInst *LInst, tInstPtr ptr)
{
    LInst->Act = ptr;
}

void NextInst (tLInst *LInst)
{
   if (LInst->Act != NULL)
   {
      if (LInst->Act == LInst->Last)
         LInst->Act = NULL;
      else
         LInst->Act = LInst->Act->rptr;
   }
}

// Nastavuje aktivitu v seznamu instrukci na navesti label
bool GotoInst (tLInst *LInst, char * label)
{
    tInstPtr tmp;
    tmp = LInst->First;

    while (tmp != NULL)
    {
        if (tmp->instType == INST_LABEL)
        {
            if (tmp->operandType1 != NDEF)
            {
                if(strcmp (label, tmp->value1.retezec) == 0)
                {
                    LInst->Act = tmp;
                    return true;
                }
            }
        }
        tmp = tmp->rptr;
    }
    errCode = 99;
    return false;
}

// ----- zasobnik ------
// Inicializuje zasobnik. Alokuje misto pro 10 prvku.
bool SRetInit (tRetStack *S) {
    if ((S->retVal = malloc (10 * sizeof (tRetValPtr))) == NULL)
        return false;
    S->top = -1;
    S->max = 10;
    return true;
}

// Zvetsuje zasobnik (na dvojnasobek sve hodnoty)
bool SRetAddSpace (tRetStack *S) {
    S->max *= 2;
    S->retVal = realloc (S->retVal, S->max * sizeof (tRetValPtr));
    if (S->retVal == NULL) {
        return false;
    }
    return true;
}

// Vklada hodnotu na vrchol zasobniku
bool SRetPush (tRetStack *S, tRetValPtr retVal) {
    if (S->top == S->max-1)
        if (!SRetAddSpace (S))
            return false;
    S->top++;
    S->retVal[S->top] = retVal;
    return true;
}

// Odstranuje prvek z vrcholu zasobniku a vraci jeho hodnotu
tRetValPtr SRetTopPop (tRetStack *S) {
    if (S->top == -1)
        return NULL;
    else
        return (S->retVal[S->top--]);
}

// Zjistuje prazdnotu zasobniku
bool SRetEmpty (tRetStack *S) {
    return (S->top == -1);
}

// Uvolnuje alokovanou pamet pro zasobnik
void SRetFree (tRetStack *S) {
    free (S->retVal);
    S->retVal = NULL;
}

// Nastavi errCode a vrati false
bool ReturnError (int errNo)
{
    errCode = errNo;
    return false;
}

int translateType(int type) {
    //Vystup:
    // DATA_TYPE_VOID,
    // DATA_TYPE_INT,
    // DATA_TYPE_DOUBLE,
    // DATA_TYPE_STRING,
    // DATA_TYPE_BOOL
    //Vstup:
    // INST_INT,
    // INST_DOUBLE,
    // INST_STRING,
    // INST_VOID,
    // INST_BOOL,
    // INST_FUNCTION,
    // INST_LABEL,
    // INST_JMP
    if(type == INST_INT)
        return DATA_TYPE_INT;
    if(type == INST_DOUBLE)
        return DATA_TYPE_DOUBLE;
    if(type == INST_STRING)
        return DATA_TYPE_STRING;
    if(type == INST_BOOL)
        return DATA_TYPE_BOOL;
    // v ostatnich pripadech se vraci VOID
    return DATA_TYPE_VOID;
}

bool AddBuiltIns (tLInst * LInst)
{
    struct tInst instrukce;

    // ---------- IFJ16.READINT ----------- //
    instrukce.name = I_LABEL;
    instrukce.instType = INST_LABEL;            
    instrukce.operandType1 = CONST;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    instrukce.value1.retezec = "ifj16.readInt";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_READINT;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = VAR;

    instrukce.value3.retezec = "#tmp";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_RET;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    if (!InsertInst (LInst, instrukce))
        return false;

    // ---------- IFJ16.READDOUBLE ----------- //
    instrukce.name = I_LABEL;
    instrukce.instType = INST_LABEL;            
    instrukce.operandType1 = CONST;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    instrukce.value1.retezec = "ifj16.readDouble";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_READDBL;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = VAR;

    instrukce.value3.retezec = "#tmp";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_RET;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    if (!InsertInst (LInst, instrukce))
        return false;

    // ---------- IFJ16.READSTRING ----------- //
    instrukce.name = I_LABEL;
    instrukce.instType = INST_LABEL;            
    instrukce.operandType1 = CONST;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    instrukce.value1.retezec = "ifj16.readString";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_READSTR;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = VAR;

    instrukce.value3.retezec = "#tmp";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_RET;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    if (!InsertInst (LInst, instrukce))
        return false;

    // ---------- IFJ16.LENGTH ----------- //
    instrukce.name = I_LABEL;
    instrukce.instType = INST_LABEL;            
    instrukce.operandType1 = CONST;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    instrukce.value1.retezec = "ifj16.length";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_LEN;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = VAR;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = VAR;

    instrukce.value1.retezec = "s";
    instrukce.value3.retezec = "#tmp";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_RET;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    if (!InsertInst (LInst, instrukce))
        return false;

    // ---------- IFJ16.SUBSTR ----------- //
    instrukce.name = I_LABEL;
    instrukce.instType = INST_LABEL;            
    instrukce.operandType1 = CONST;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    instrukce.value1.retezec = "ifj16.substr";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_SUBSTR;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = VAR;
    instrukce.operandType2 = VAR;
    instrukce.operandType3 = VAR;

    instrukce.value1.retezec = "s";
    instrukce.value2.retezec = "i";
    instrukce.value3.retezec = "n";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_RET;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    if (!InsertInst (LInst, instrukce))
        return false;

    // ---------- IFJ16.COMPARE ----------- //
    instrukce.name = I_LABEL;
    instrukce.instType = INST_LABEL;            
    instrukce.operandType1 = CONST;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    instrukce.value1.retezec = "ifj16.compare";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_CMP;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = VAR;
    instrukce.operandType2 = VAR;
    instrukce.operandType3 = VAR;

    instrukce.value1.retezec = "s1";
    instrukce.value2.retezec = "s2";
    instrukce.value3.retezec = "#tmp";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_RET;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    if (!InsertInst (LInst, instrukce))
        return false;

    // ---------- IFJ16.FIND ----------- //
    instrukce.name = I_LABEL;
    instrukce.instType = INST_LABEL;            
    instrukce.operandType1 = CONST;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    instrukce.value1.retezec = "ifj16.find";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_FIND;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = VAR;
    instrukce.operandType2 = VAR;
    instrukce.operandType3 = VAR;

    instrukce.value1.retezec = "s";
    instrukce.value2.retezec = "search";
    instrukce.value3.retezec = "#tmp";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_RET;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    if (!InsertInst (LInst, instrukce))
        return false;

    // ---------- IFJ16.SORT ----------- //
    instrukce.name = I_LABEL;
    instrukce.instType = INST_LABEL;            
    instrukce.operandType1 = CONST;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    instrukce.value1.retezec = "ifj16.sort";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_SORT;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = VAR;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = VAR;

    instrukce.value1.retezec = "s";
    instrukce.value3.retezec = "#tmp";

    if (!InsertInst (LInst, instrukce))
        return false;

    instrukce.name = I_RET;
    instrukce.instType = INST_FUNCTION;            
    instrukce.operandType1 = NDEF;
    instrukce.operandType2 = NDEF;
    instrukce.operandType3 = NDEF;

    if (!InsertInst (LInst, instrukce))
        return false;

    return true;
}

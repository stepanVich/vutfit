/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * ial.c
 *
 * Antonin Mazanek,     xmazan07
 */

#include "ial.h"

int errCode;

// ----- binarni strom -----
// Inicializuje binarniho stromu
void BTInit (tBTNodePtr *rootPtr) {

    *rootPtr = NULL;
}

// Vytvori novy list v binarnim strome
tSymTablePtr BTInsert (tBTNodePtr *rootPtr, char *key, struct tSymTable data) {

    tBTNodePtr pomRootPtr = *rootPtr;

    // prazdny strom
    if (*rootPtr == NULL) {
        if ((*rootPtr = malloc (sizeof (struct tBTNode))) == NULL) {
            errCode = 99;
            fprintf (stderr, "E: malloc\n");
            return NULL;
        }
        if (((*rootPtr)->data = malloc (sizeof (struct tSymTable))) == NULL) {
            errCode = 99;
            fprintf (stderr, "E: malloc\n");
            free (*rootPtr);
            *rootPtr = NULL;
            return NULL;
        }
        (*rootPtr)->key = key;
        (*rootPtr)->data->type = data.type;
        (*rootPtr)->data->name = data.name;
        (*rootPtr)->data->function = data.function;
        (*rootPtr)->data->variable = data.variable;
        (*rootPtr)->LPtr = NULL;
        (*rootPtr)->RPtr = NULL;
        return (*rootPtr)->data;
    }

    while (1) {
        // existujici polozka s klicem - nahrazeni obsahu
        if (strcmp ((pomRootPtr)->key, key) == 0) {
            (pomRootPtr)->data->type = data.type;
            (pomRootPtr)->data->name = data.name;
            (pomRootPtr)->data->function = data.function;
            (pomRootPtr)->data->variable = data.variable;
            return (pomRootPtr)->data;
        }
        // klic je vetsi - pokracuj pravym podstromem
        else if (strcmp ((pomRootPtr)->key, key) < 0) {
            // pokud pravy podstrom neexistuje, vytvor jej
            if ((pomRootPtr)->RPtr == NULL) {
                tBTNodePtr pom;
                if ((pom = malloc (sizeof (struct tBTNode))) == NULL) {
                    errCode = 99;
                    fprintf (stderr, "E: malloc\n");
                    return NULL;
                }
                if ((pom->data = malloc (sizeof (struct tSymTable))) == NULL) {
                    errCode = 99;
                    fprintf (stderr, "E: malloc\n");
                    return NULL;
                }
                pom->key = key;
                pom->data->type = data.type;
                pom->data->name = data.name;
                pom->data->function = data.function;
                pom->data->variable = data.variable;
                pom->LPtr = NULL;
                pom->RPtr = NULL;
                (pomRootPtr)->RPtr = pom;
                return pom->data;
            }
            // pokud pravy podstrom existuje, pokracuj timto uzlem
            else {
                pomRootPtr = (pomRootPtr)->RPtr;
            }
        }
        // klic je mensi - pokracuj levym podstromem
        else {
            // pokud levy podstrom neexistuje, vytvor jej
            if ((pomRootPtr)->LPtr == NULL) {
                tBTNodePtr pom;
                if ((pom = malloc (sizeof (struct tBTNode))) == NULL) {
                    errCode = 99;
                    fprintf (stderr, "E: malloc\n");
                    return NULL;
                }
                if ((pom->data = malloc (sizeof (struct tSymTable))) == NULL) {
                    errCode = 99;
                    fprintf (stderr, "E: malloc\n");
                    return NULL;
                }
                pom->key = key;
                pom->data->type = data.type;
                pom->data->name = data.name;
                pom->data->function = data.function;
                pom->data->variable = data.variable;
                pom->LPtr = NULL;
                pom->RPtr = NULL;
                (pomRootPtr)->LPtr = pom;
                return pom->data;
            }
            // pokud levy podstrom existuje, pokracuj timto uzlem
            else {
                pomRootPtr = (pomRootPtr)->LPtr;
            }
        }
    }
}

// Vyhleda polozku v binarnim strome podle klice
bool BTSearch (tBTNodePtr root, char *key, tSymTablePtr *data) {

    // pokud strom nema zadnou polozku
    if (root == NULL) {
        *data = NULL;
        return false;
    }

    bool konec = false;
    while (!konec) {
        // polozka nalezena
        if (strcmp (root->key, key) == 0) {
            *data = root->data;
            return true;
        }
        // klic je vetsi - pokracuj pravym podstromem
        else if (strcmp (root->key, key) < 0) {
            root = root->RPtr;
        }
        // klic je mensi - pokracuj levym podstromem
        else {
            root = root->LPtr;
        }
        // pokud dosel na konec stromu
        if (root == NULL) {
            konec = true;
        }
    }

    *data = NULL;
    return false;
}

// Rusi strom a nastavuje jej do stavu po inicializaci
void BTDispose (tBTNodePtr *rootPtr) {

    // prazdny strom - nedelej nic
    if (*rootPtr == NULL)
        return;

    // pripraveni zasobniku
    tStack S;
    if (!SInit (&S))
        return;

    do {
        // pokud je (pod)strom prazdny, ale zasobnik ne, precti vrchol
        if (*rootPtr == NULL && !SEmpty (&S)) {
            *rootPtr = STopPop (&S);
        }
        else {
            // pokud pravy podstrom existuje, uloz jej na zasobnik
            if ((*rootPtr)->RPtr != NULL)
                SPush (&S, (*rootPtr)->RPtr);
            // smazani aktualniho uzlu a pokracovani levym podstromem
            tBTNodePtr pom = *rootPtr;
            *rootPtr = (*rootPtr)->LPtr;

            if (pom->data->function != NULL)
            {
                DisposeParamList(pom->data->function->params);
            }
            free (pom->data->function);
            pom->data->function = NULL;
            free (pom->data->variable);
            pom->data->variable = NULL;
            free (pom->data);
            pom->data = NULL;
            //free (pom->key);
            //pom->key = NULL;
            //free (pom->data->name);
            //pom->data->name = NULL;

            free (pom);
            pom = NULL;
        }
    } while (*rootPtr != NULL || !SEmpty (&S));

    SFree (&S);
}

// ----- zasobnik ------
// Inicializuje zasobnik. Alokuje misto pro 10 prvku.
bool SInit (tStack *S) {

    if ((S->a = malloc (10 * sizeof (tBTNodePtr))) == NULL) {
        errCode = 99;
        fprintf (stderr, "E: malloc\n");
        return false;
    }
    S->top = -1;
    S->max = 10;
    return true;
}

// Zvetsuje zasobnik (na dvojnasobek sve hodnoty)
bool SAddSpace (tStack *S) {

    S->max *= 2;
    S->a = realloc (S->a, S->max * sizeof (tBTNodePtr));
    if (S->a == NULL) {
        errCode = 99;
        fprintf (stderr, "E: realloc\n");
        return false;
    }
    return true;
}

// Vklada hodnotu na vrchol zasobniku
bool SPush (tStack *S, tBTNodePtr data) {

    if (S->top == S->max-1)
        if (!SAddSpace (S))
            return false;

    S->top++;
    S->a[S->top] = data;
    return true;
}

// Odstranuje prvek z vrcholu zasobniku a vraceni jeho hodnotu
tBTNodePtr STopPop (tStack *S) {

    if (S->top == -1)
        return NULL;
    else
        return (S->a[S->top--]);
}

// Zjistuje prazdnotu zasobniku
bool SEmpty (tStack *S) {

    return (S->top == -1);
}

// Uvolnuje alokovanou pamet pro zasobnik
void SFree (tStack *S) {

    free (S->a);
    S->a = NULL;
}

// ----- seznam -----
// Inicializuje seznam parametru
void InitParamList (tParamListPtr L) {

    L->First = NULL;
    L->Last = NULL;
}

// Zusi seznam parametru a nastavuje jej do stavu po inicalizaci
void DisposeParamList (tParamListPtr L) {

    if (L == NULL)
        return;

    for (tParamElemPtr Act = L->First; Act != NULL;) {
        tParamElemPtr pom = Act;
        Act = Act->next;
        free (pom);
        pom = NULL;
    }

    L->First = NULL;
    L->Last = NULL;
}

// Vklada parametr na konec seznamu parametru
bool InsertLastParam (tParamListPtr L, int type, char* name, int index) {

    if (L == NULL)
        return false;

    tParamElemPtr newOne;
    if ((newOne = malloc (sizeof (struct tParamElem))) == NULL) {
        errCode = 99;
        fprintf (stderr, "E: malloc\n");
        return false;
    }

    newOne->type = type;
    newOne->name = name;
    newOne->index = index;
    newOne->next = NULL;
    if (L->First == NULL)
        L->First = newOne;
    else
        L->Last->next = newOne;
    L->Last = newOne;

    return true;
}

// Hleda v seznamu parametru polozku s danym indexem
tParamElemPtr SearchParamList (tParamListPtr L, int index) {

    if (L == NULL)
        return NULL;

    for (tParamElemPtr Act = L->First; Act != NULL; Act = Act->next) {
        if (Act->index == index)
            return Act;
    }

    return NULL;
}

// Inicializuje seznam nazvu trid
void InitClassNameList (tClassNameListPtr L) {

    L->First = NULL;
    L->Last = NULL;
}

// Zusi seznam nazvu trid a nastavuje jej do stavu po inicalizaci
void DisposeClassNameList (tClassNameListPtr L) {

    if (L == NULL)
        return;

    for (tClassNameElemPtr Act = L->First; Act != NULL;) {
        tClassNameElemPtr pom = Act;
        Act = Act->next;
        free (pom->name);
        pom->name = NULL;
        free (pom);
        pom = NULL;
    }

    L->First = NULL;
    L->Last = NULL;
}

// Vklada dalsi nazev na konec seznamu nazvu trid
bool InsertLastClassName (tClassNameListPtr L, char *name) {

    if (L == NULL)
        return false;

    tClassNameElemPtr newOne;
    if ((newOne = malloc (sizeof (struct tClassNameElem))) == NULL) {
        errCode = 99;
        fprintf (stderr, "E: malloc\n");
        return false;
    }

    newOne->name = name;
    newOne->next = NULL;
    if (L->First == NULL)
        L->First = newOne;
    else
        L->Last->next = newOne;
    L->Last = newOne;

    return true;
}

// Hleda zadany nazev v zadanem liste nazvu trid
bool isInClassNameList (tClassNameListPtr L, char *name) {

    if (L == NULL)
        return false;

    for (tClassNameElemPtr Act = L->First; Act != NULL; Act = Act->next) {
        if (strcmp (name, Act->name) == 0)
            return true;
    }

    return false;
}

// ----- vestavene funkce v ramci IAL casti -----
// Seradi znaky v retezci vzestupne dle ordinalni hodnoty
char* sort (char* s) {

    if (s == NULL)
        return NULL;

    char *my_s = malloc (strlen (s));
    if (my_s == NULL) {
        fprintf (stderr, "E: malloc\n");
        errCode = 99;
        return NULL;
    }
    strcpy (my_s, s);
    my_s = convert_str (my_s);

    int length = strlen (my_s);
    int step = length / 2; // krok nastaven na polovinu delky

    while (step > 0) {
        for (int i = step; i < length; i++) {
            int j = i - step;
            while (j >= 0 && my_s[j] > my_s[j+step]) {
                char pom = my_s[j];
                my_s[j] = my_s[j+step];
                my_s[j+step] = pom;
                j -= step;
            }
        }
        step /= 2;
    }
    return my_s;
}

// Najde podretezec v retezci (Boyer-Mooreuv algoritmus)
int find(char *s, char *search) {

    if (s == NULL || search == NULL)
        return -1;

    // vytvoreni kopii zadanych retezcu
    char *my_s = malloc (strlen (s));
    if (my_s == NULL) {
        fprintf (stderr, "E: malloc\n");
        errCode = 99;
        return -1;
    }
    char *my_search = malloc (strlen (search));
    if (my_search == NULL) {
        fprintf (stderr, "E: malloc\n");
        errCode = 99;
        return -1;
    }

    strcpy (my_s, s);
    strcpy (my_search, search);

    // prevod escape sekvenci zadanych jako jednotlive znaky na jeden znak
    my_s = convert_str (my_s);
    my_search = convert_str (my_search);

    // zacatek Boyer-Mooreuv algoritmus
    int charJump[256];
    int delka_s = strlen (my_s);
    int delka_search = strlen (my_search);

    // nastaveni skoku na maximum (delka hledaneho retezce) pro vsechny znaky
    for (int i = 0; i < 256; i++)
        charJump[i] = delka_search;

    // prepocet skoku pro vsechny znaky na zaklade vstupnich retezcu
    for (int i = 0; i < delka_search; i++)
        charJump[(int)my_search[i]] = delka_search - 1 - i;

    // indexy do zadanych retezcu
    int j = delka_search - 1;
    int k = delka_search - 1;

    while (j < delka_s && k >= 0) {
        if (my_s[j] == my_search[k]) {
            j--;
            k--;
        } else {
            j += charJump[(int)my_s[j]];
            k = delka_search -1;
        }
    }

    // je-li nalezen podretezec (k == -1), je vracen index kde byl nalezen
    if (k == -1)
        return j + 1;
    else
        return -1;
}

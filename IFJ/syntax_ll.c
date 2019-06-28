/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * syntax_ll.c
 *
 * Monika Muzikovska,	xmuzik05
 * Dominik Turecek,		xturec06
 * Stepan Vich,			xvichs00
 */


/**
 * @author			xmuzik05
 * @author			xturec06
 * @author			xvichs00
 * @brief			Syntakticka analyza shora dolu a syntaxi rizeny preklad.
 *	Obsahuje pomocne funkce pro reprezentaci pravidel LL gramatiky a syntakticky analyzuje
 *  vstupni zdrojovy soubor, ze ktereho lexikalni analyzator nacita tokeny. Pote soubor analyzuje semanticky
 *  a vyvola interpret pro zpracovani instrukcni pasky, kterou generuje.
 * @return Vraci true, pokud vsechny analyzy a interpretace probehly v poradu, jinak cislo chyby podle zadani.
 */
#include "syntax_ll.h"

tBTNodePtr Globalni_TS;
tBTNodePtr Lokalni_TS;

tLInst Instrukce;
FILE * fp;
aktualniData dataSemantika;
int errCode;
int param_index;

int main(int argc, char*argv[])
{
	errCode = 0;
	if (argc != 2)
        return 99;

	fp = fopen(argv[1], "r");
	if(fp) {
		// Syntakticka analyza
		if (START(1))
		{
			rewind(fp);
			// Semanticka analyza
			if (START(2))
			{
				// Kontrola ze existuje funkce Main.run, ktera je typu void a nema parametry
				tSymTablePtr data = NULL;
				if(BTSearch(Globalni_TS, "Main.run", &data))
				{
					if(data->type == TABLE_FUNCTION && data->function != NULL)
					{
						if(data->function->return_type != DATA_TYPE_VOID)
						{
							fprintf(stderr, "Main.run nevraci void.\n");
							errCode = 3;
							BTDispose(&Globalni_TS);
							BTDispose(&Lokalni_TS);
							return errCode;
						}
						if(data->function->no_params != 0)
						{
							fprintf(stderr, "Main.run ma parametry.\n");
							errCode = 3;
							BTDispose(&Globalni_TS);
							BTDispose(&Lokalni_TS);
							return errCode;
						}
					} else {
						fprintf(stderr, "Main.run neni funkce.\n");
						errCode = 3;
						BTDispose(&Globalni_TS);
						BTDispose(&Lokalni_TS);
						return errCode;
					}
				} else {
					fprintf(stderr, "Funkce Main.run nenalezena.\n");
					errCode = 3;
					BTDispose(&Globalni_TS);
					BTDispose(&Lokalni_TS);
					return errCode;
				}
				struct tInst instrukce;

				// Instrukce pro oznaceni konce pasky
				instrukce.name = I_END;
				instrukce.instType = INST_LABEL;
				instrukce.operandType1 = NDEF;
				instrukce.operandType2 = NDEF;
				instrukce.operandType3 = NDEF;

				// Ulozeni instrukce do seznamu
				if (!InsertInst(&Instrukce, instrukce))
				{
					fprintf(stderr, "Chyba pri vkladani instrukce.\n");
					errCode = 99;
					BTDispose(&Globalni_TS);
					BTDispose(&Lokalni_TS);
					return errCode;
				}
				fclose(fp);

				// Volani interpretu
				if (!inter(&Instrukce))
				{
					fprintf(stderr, "Chyba interpretu. errCode: %d\n", errCode);
					BTDispose(&Globalni_TS);
					BTDispose(&Lokalni_TS);
					return errCode;
				}

				BTDispose(&Globalni_TS);
				BTDispose(&Lokalni_TS);
				return errCode;
			}
			else
			{
				if (errCode == 0)
				{
					fprintf(stderr, "Syntakticka chyba.\n");
					errCode = 2;
				}
				BTDispose(&Globalni_TS);
				BTDispose(&Lokalni_TS);
				return errCode;
			}
		}
		else
		{
			if (errCode == 0)
			{
				fprintf(stderr, "Syntakticka chyba.\n");
				errCode = 2;
			}
			BTDispose(&Globalni_TS);
			BTDispose(&Lokalni_TS);
			return errCode;
		}
	} else {
		fprintf(stderr, "Chyba pri otevireni souboru.\n");
		errCode = 99;
		BTDispose(&Globalni_TS);
		BTDispose(&Lokalni_TS);
		return errCode;
	}
}

// Funkce pro vytvoreni polozky v TS
int CREATE(tBTNodePtr *root, struct tSymTable item, tSymTablePtr* data)
{
	if (BTSearch(*root, item.name, data))
		return ERR_MATCHING; // Existuje polozka se stejnym jmenem
    else
    {
        (*data) = BTInsert(root, item.name, item);

        if ((*data) == NULL)
            return PROGRAM_ERROR;
        else
            return CREATED;
    }

}

// Funkce pro nalezeni polozky v TS
int FIND(Token *tk, char *className, tBTNodePtr lok, tBTNodePtr glob, tSymTablePtr *data)
{
	// PKID se vyhledava rovnou v globalni TS, jednoduchy ID nejdrive v lokalni TS a pokud neni nalezen
	// rozsiri se jeho nazev na PKID se jmenem aktualni tridy a vyhledava se v globalni TS
    if (tk->type != TOKEN_TYPE_IDENTIFIKATOR_SLOZENY) {

        if (BTSearch (lok, tk->value, data))
            return FOUND_LOCAL;
        else {
            char *pkid;
            int length = strlen(className) + 1 + strlen(tk->value) + 1;
            pkid = malloc ((size_t)length);
            if (pkid == NULL) {
                fprintf (stderr, "Selhal malloc\n");
                errCode = 99;
                return PROGRAM_ERROR;
            }
            pkid[0] = '\0';
            strcpy (pkid, className);
            strcat (pkid, ".");
            strcat (pkid, tk->value);
            pkid[length-1] = '\0';

            if(BTSearch (glob, pkid, data))
                return FOUND_GLOBAL;
            else
                return ERR_NOT_FOUND;
        }
    }
    else {
        if(BTSearch (glob, tk->value, data))
            return FOUND_GLOBAL;
        else
            return ERR_NOT_FOUND;
    }
}

// Funkce pro vlozeni vestavenych funkci do globalni TS
bool insertFunctions()
{
	return (insertReadInt() && insertReadDouble() && insertReadString() && insertPrint()
		      && insertLength() && insertSubstr() && insertCompare() && insertFind() && insertSort());
}

// Vestavene funkce s parametry a navratovymi hodnotami podle zadani
bool insertReadInt()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.readInt";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_INT;
	item.function->no_params = 0;
	item.function->params = NULL;
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

bool insertReadDouble()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.readDouble";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_DOUBLE;
	item.function->no_params = 0;
	item.function->params = NULL;
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

bool insertReadString()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.readString";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_STRING;
	item.function->no_params = 0;
	item.function->params = NULL;
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

bool insertPrint()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.print";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_VOID;
	item.function->no_params = 0;
	item.function->params = NULL;
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

bool insertLength()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.length";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_INT;
	item.function->no_params = 1;
	item.function->params = malloc(sizeof(struct tParamList));
    if (item.function->params == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	InitParamList(item.function->params);
	InsertLastParam(item.function->params, DATA_TYPE_STRING, "s", 0);
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

bool insertSubstr()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.substr";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_STRING;
	item.function->no_params = 3;
	item.function->params = malloc(sizeof(struct tParamList));
    if (item.function->params == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	InitParamList(item.function->params);
	InsertLastParam(item.function->params, DATA_TYPE_STRING, "s", 0);
	InsertLastParam(item.function->params, DATA_TYPE_INT, "i", 1);
	InsertLastParam(item.function->params, DATA_TYPE_INT, "n", 2);
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

bool insertCompare()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.compare";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_INT;
	item.function->no_params = 2;
	item.function->params = malloc(sizeof(struct tParamList));
    if (item.function->params == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	InitParamList(item.function->params);
	InsertLastParam(item.function->params, DATA_TYPE_STRING, "s1", 0);
	InsertLastParam(item.function->params, DATA_TYPE_STRING, "s2", 1);
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

bool insertFind()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.find";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_INT;
	item.function->no_params = 2;
	item.function->params = malloc(sizeof(struct tParamList));
    if (item.function->params == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	InitParamList(item.function->params);
	InsertLastParam(item.function->params, DATA_TYPE_STRING, "s", 0);
	InsertLastParam(item.function->params, DATA_TYPE_STRING, "search", 1);
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

bool insertSort()
{
	struct tSymTable item;
	item.type = TABLE_FUNCTION;
	item.name = "ifj16.sort";
	item.function = malloc(sizeof(struct dataFunction));
    if (item.function == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	item.function->return_type = DATA_TYPE_STRING;
	item.function->no_params = 1;
	item.function->params = malloc(sizeof(struct tParamList));
    if (item.function->params == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return false;
    }
	InitParamList(item.function->params);
	InsertLastParam(item.function->params, DATA_TYPE_STRING, "s", 0);
	item.variable = NULL;
	tSymTablePtr data = NULL;

	if (CREATE(&Globalni_TS, item, &data) != CREATED)
		return false;
	return true;
}

/************************** Syntaxi rizeny preklad *********************************************/
bool START (int pruchod)
{
	if (pruchod == 1 || pruchod == 2)
	{
		Lokalni_TS = NULL;
		dataSemantika.f_assign_to = NULL;
		tClassNameListPtr classes = NULL;
		if (pruchod == 1)
		{
			// inicializace globalni TS
			BTInit (&Globalni_TS);

			// inicializace seznamu s nazvy trid a vlozeni nazvu ifj16
			if (!insertFunctions())
			{
				fprintf(stderr, "Nepovedlo se vlozeni vestavenych fci.\n");
				errCode = 99;
				return false;
			}

			classes = (tClassNameListPtr)malloc(sizeof(struct tClassNameList));
			if(classes == NULL)
			{
				fprintf(stderr, "Nepovedl se malloc.\n");
				errCode = 99;
				return false;
			}
			InitClassNameList(classes);

			if (!InsertLastClassName(classes, "ifj16"))
			{
				fprintf(stderr, "Nepovedlo se vlozeni jmena fce ifj16.\n");
				errCode = 99;
				return false;
			}
		}
		if (pruchod == 2)
			// inicializace listu instrukci
			InitInstList(&Instrukce);

		TokenPtr tk = get_token();

		// Pravidlo c. 1 			S => <classes>
		if ((tk->type == TOKEN_TYPE_KEYWORD && strcmp("class", tk->value) == 0)
		 	|| tk->type == TOKEN_TYPE_EOF)
		{
			return CLASSES(&tk, pruchod, classes);
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool CLASSES (TokenPtr* tk, int pruchod, tClassNameListPtr classes)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 2 			<classes>  => <class> <classes>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("class", (*tk)->value) == 0)
			return (CLASS(tk, pruchod, classes) && CLASSES(tk, pruchod, classes));
		// pravidlo c. 3 			<classes> =>  epsilon
		else if ((*tk)->type == TOKEN_TYPE_EOF)
			return true;

		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool CLASS (TokenPtr* tk, int pruchod, tClassNameListPtr classes)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 4 			<class> => class identifikator {<definice>}
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("class", (*tk)->value) == 0)
		{
			*tk = get_token();
			if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
			{
				char *tmp_class_name = (*tk)->value;
				// Vlozeni nazvu tridy a kontrola ze jiz neni pouzit
				if (isInClassNameList(classes, (*tk)->value))
				{
					fprintf(stderr, "Semanticka chyba - Nazev tridy %s je jiz pouzit.\n", (*tk)->value);
					errCode = 3;
					return false;
				}
				else
				{
					if (!InsertLastClassName(classes, (*tk)->value))
					{
						fprintf(stderr, "Chyba pri vkladani nazvu tridy.\n");
						errCode = 99;
						return false;
					}
				}

				*tk = get_token();
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
				{
					*tk = get_token();
					if (DEFINICE(tk, pruchod, tmp_class_name) == true)
					{
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
						{
							*tk = get_token();
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 4 			<class> => class identifikator {<definice>}
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("class", (*tk)->value) == 0)
		{
			*tk = get_token();
			if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
			{
				dataSemantika.class_name = (*tk)->value; // prideleni do globalni promenne

				*tk = get_token();
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
				{
					*tk = get_token();
					if (DEFINICE(tk, pruchod, dataSemantika.class_name) == true)
					{
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
						{
							*tk = get_token();
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool DEFINICE (TokenPtr* tk, int pruchod, char* class_name)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 5 			<definice> => <jedna_definice> <definice>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("static", (*tk)->value) == 0)
			return (JEDNA_DEFINICE(tk, pruchod, class_name) && DEFINICE(tk, pruchod, class_name));
		// Pravidlo c. 6			<definice> => epsilon
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
			return true;

		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool JEDNA_DEFINICE (TokenPtr* tk, int pruchod, char* class_name)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 7			<jedna_definice> => static <jedna_definice_2>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("static", (*tk)->value) == 0)
		{
			*tk = get_token();
			return JEDNA_DEFINICE_2(tk, pruchod, class_name);
		}
		return false;

	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool JEDNA_DEFINICE_2 (TokenPtr* tk, int pruchod, char* class_name)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 8			<jedna_definice_2> => <typ> identifikator <jedna_definice_3>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			int data_type;
			if (TYP(tk, pruchod, &data_type) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
				{
					// Vytvoreni PKID pro parametr dalsi funkce
					char *pkid;
					int length = strlen(class_name) + 1 + strlen((*tk)->value) + 1;

		            if ((pkid = malloc ((size_t)length)) != NULL)
		            {
	       			    pkid[0] = '\0';
	       			    strcpy (pkid, class_name);
	            		strcat (pkid, ".");
	            		strcat (pkid, (*tk)->value);
						*tk = get_token();
						pkid[length-1] = '\0';
						return JEDNA_DEFINICE_3(tk, pruchod, pkid, data_type);
					}
					else
					{
						fprintf(stderr, "Selhal malloc.\n");
						errCode = 99;
						return false;
					}
				}
			}
			return false;
		}
		// Pravidlo c. 9			<jedna_definice_2> => void identifikator <jedna_definice_3>
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("void", (*tk)->value) == 0)
		{
			int return_type = DATA_TYPE_VOID;
			*tk = get_token();
			if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
			{
				// Vytvoreni PKID pro parametr dalsi funkce
				char *pkid;
				int length = strlen(class_name) + 1 + strlen((*tk)->value) + 1;

            	if ((pkid = malloc((size_t)length)) != NULL)
            	{
	            	pkid[0] = '\0';
	            	strcpy (pkid, class_name);
	            	strcat (pkid, ".");
	            	strcat (pkid, (*tk)->value);
					*tk = get_token();
					pkid[length-1] = '\0';
					return JEDNA_DEFINICE_4(tk, pruchod, pkid, return_type);
				}
				else
				{
					fprintf(stderr, "Selhal malloc.\n");
					errCode = 99;
					return false;
				}
			}
		}
		return false;

	} else if(pruchod == 2) {
		// Pravidlo c. 8			<jedna_definice_2> => <typ> identifikator <jedna_definice_3>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			if (TYP(tk, pruchod, &(dataSemantika.data_type)) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
				{
					// Vytvoreni PKID a ulozeni do globalni promenne
					char *pkid;
					int length = strlen(dataSemantika.class_name) + 1 + strlen((*tk)->value) + 1;

		            if ((pkid = malloc((size_t)length)) != NULL )
		            {
	       			    pkid[0] = '\0';
	       			    strcpy (pkid, dataSemantika.class_name);
	            		strcat (pkid, ".");
	            		strcat (pkid, (*tk)->value);
	            		strcat (pkid, "\0");
	            		dataSemantika.pkid = pkid;
						*tk = get_token();
						pkid[length-1] = '\0';
						return JEDNA_DEFINICE_3(tk, pruchod, pkid, dataSemantika.data_type);
					}
					else
					{
						fprintf(stderr, "Selhal malloc.\n");
						errCode = 99;
						return false;
					}
				}
			}
			return false;
		}
		// Pravidlo c. 9			<jedna_definice_2> => void identifikator <jedna_definice_3>
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("void", (*tk)->value) == 0)
		{
			dataSemantika.return_type = DATA_TYPE_VOID;
			*tk = get_token();
			if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
			{
				// Vytvoteni PKID a ulozeni do globalni promenne
				char *pkid;
				int length = strlen(dataSemantika.class_name) + 1 + strlen((*tk)->value) + 1;

            	if ((pkid = malloc((size_t)length)) != NULL)
            	{
	            	pkid[0] = '\0';
	            	strcpy (pkid, dataSemantika.class_name);
	            	strcat (pkid, ".");
	            	strcat (pkid, (*tk)->value);
	            	dataSemantika.pkid = pkid;
					*tk = get_token();
					pkid[length-1] = '\0';
					return JEDNA_DEFINICE_4(tk, pruchod, pkid, dataSemantika.return_type);
				}
				else
				{
					fprintf(stderr, "Selhal malloc.\n");
					errCode = 99;
					return false;
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool JEDNA_DEFINICE_4 (TokenPtr* tk, int pruchod, char* pkid, int data_type)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 11			<jedna_definice_4> => (<seznam_parametru>){<prikazy>}
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
		{
			// Vytvoreni polozky pro funkci
			struct tSymTable item;
			item.type = TABLE_FUNCTION;
			item.name = pkid;

			if ((item.function = malloc(sizeof(struct dataFunction))) == NULL)
			{
				fprintf(stderr, "Selhal malloc.\n");
				errCode = 99;
				return false;
			}

			item.function->return_type = data_type;
			item.function->no_params = 0;

			if ((item.function->params = malloc(sizeof(struct tParamList))) == NULL)
			{
				fprintf(stderr, "Selhal malloc.\n");
				errCode = 99;
				return false;
			}

			item.variable = NULL;
			tSymTablePtr data = NULL;

			// Vlozeni polozky do TS
			int create_return = CREATE(&Globalni_TS, item, &data);
			if (create_return == CREATED)
			{
				*tk = get_token();
				if (SEZNAM_PARAMETRU(tk, pruchod, data) == true)
				{
					if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
					{
						*tk = get_token();
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
						{
							*tk = get_token();
							if (PRIKAZY(tk, pruchod) == true)
							{
								if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
								{
									*tk = get_token();
									return true;
								}
							}
						}
					}
				}
			}
			else if (create_return == ERR_MATCHING)
			{
				fprintf(stderr, "Semanticka chyba - Pokus o vytvoreni existujici polozky.\n");
				errCode = 3;
				return false;
			}
			else
			{
				fprintf(stderr, "Selhalo vytvareni polozky v tabulce.\n");
				errCode = 99;
				return false;
			}
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 11			<jedna_definice_4> => (<seznam_parametru>){<prikazy>}
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
		{
			dataSemantika.return_type = dataSemantika.data_type;
			// Vytvoreni navesti s nazvem pkid
			// (I_LABEL, pkid, , )
			// Popsani instrukce
			struct tInst instrukce;
			instrukce.name = I_LABEL;
			instrukce.instType = INST_LABEL;

			instrukce.operandType1 = CONST;
			instrukce.operandType2 = NDEF;
			instrukce.operandType3 = NDEF;

			instrukce.value1.retezec = dataSemantika.pkid;

			// Ulozeni instrukce do seznamu
			if (!InsertInst(&Instrukce, instrukce))
			{
				fprintf(stderr, "Selhalo vkladani instrukce.\n");
				errCode = 99;
				return false;
			}

			// Vytvoreni nove lokalni TS pro aktualni funkci
			BTInit(&Lokalni_TS);

			*tk = get_token();
			if (SEZNAM_PARAMETRU(tk, pruchod, NULL) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
				{
					*tk = get_token();
					if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
					{
						*tk = get_token();
						if (PRIKAZY(tk, pruchod) == true)
						{
							if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
							{
								// Instrukce pro oznaceni konce tela funkce
								struct tInst instrukce2;
								instrukce2.name = I_FEND;
								instrukce2.instType = INST_FUNCTION;

								instrukce2.operandType1 = CONST;
								instrukce2.operandType2 = NDEF;
								instrukce2.operandType3 = NDEF;
								instrukce2.value1.retezec = instrukce.value1.retezec;

								// Ulozeni instrukce do seznamu
								if (!InsertInst(&Instrukce, instrukce2))
								{
									fprintf(stderr, "Selhalo vkladani instrukce.\n");
									errCode = 99;
									return false;
								}
								// Uvolneni lokalni TS aktualni funkce
								BTDispose(&Lokalni_TS);
								Lokalni_TS = NULL;
								*tk = get_token();
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool JEDNA_DEFINICE_3 (TokenPtr* tk, int pruchod, char* pkid, int data_type)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 10			<jedna_definice_3> => <definice_promenne>
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp(";", (*tk)->value) == 0
			|| strcmp("=", (*tk)->value) == 0))
		{
			// Vytvoreni polozky pro globalni promennou
			struct tSymTable item;
			item.type = TABLE_VARIABLE;
			item.name = pkid;

			if ((item.variable = malloc(sizeof(struct dataVariable))) == NULL)
			{
				fprintf(stderr, "Selhal malloc.\n");
				errCode = 99;
				return false;
			}

			item.variable->data_type = data_type;
			item.function = NULL;
			tSymTablePtr data = NULL;

			// Ulozeni polozky do TS
			int create_return = CREATE(&Globalni_TS, item, &data);
			if (create_return == CREATED)
			{
				return DEFINICE_PROMENNE(tk, pruchod);
			}
			else if (create_return == ERR_MATCHING)
			{
				fprintf(stderr, "Semanticka chyba - Pokus o vytvoreni existujici polozky.\n");
				errCode = 3;
				return false;
			}
			else
			{
				fprintf(stderr, "Selhalo vytvareni polozky v tabulce.\n");
				errCode = 99;
				return false;
			}
		}
		// Pravidlo c. 55			<jedna_definice_3> => <jedna_definice_4>
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
		{
			return JEDNA_DEFINICE_4(tk, pruchod, pkid, data_type);
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 10			<jedna_definice_3> => <definice_promenne>
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp(";", (*tk)->value) == 0
			|| strcmp("=", (*tk)->value) == 0))
		{
			// Vytvoreni instrukce pro oznaceni casti deklarace statickych promennych
			struct tInst instrukce;
			instrukce.name = I_DECSTART;
			instrukce.instType = INST_FUNCTION;

			instrukce.operandType1 = NDEF;
			instrukce.operandType2 = NDEF;
			instrukce.operandType3 = NDEF;

			// Ulozeni instrukce do seznamu
			if (!InsertInst(&Instrukce, instrukce))
			{
				fprintf(stderr, "Selhalo vlozeni instrukce.\n");
				errCode = 99;
				return false;
			}
			bool found = DEFINICE_PROMENNE(tk, pruchod);
			if (found)
			{
				// Vytvoreni instrukce pro oznaceni konce casto deklarace statickych promennych
				instrukce.name = I_DECEND;
				instrukce.instType = INST_FUNCTION;

				instrukce.operandType1 = NDEF;
				instrukce.operandType2 = NDEF;
				instrukce.operandType3 = NDEF;

				// Ulozeni instrukce do seznamu
				if (!InsertInst(&Instrukce, instrukce))
				{
					fprintf(stderr, "Selhalo vlozeni instrukce.\n");
					errCode = 99;
					return false;
				}
			}
			return found;
		}
		// Pravidlo c. 55			<jedna_definice_3> => <jedna_definice_4>
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
		{
			// V data_type je ulozen navratovy typ aktualni funkce
			dataSemantika.return_type = dataSemantika.data_type;
			return JEDNA_DEFINICE_4(tk, pruchod, pkid, data_type);
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool ID (TokenPtr* tk, int pruchod, tSymTablePtr* data)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 12			<id> => identifikator
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
		{
			*tk = get_token();
			return true;
		}
		// Pravidlo c. 13			<id> => plne_kvalifikovany_identifikator
		else if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY)
		{
			*tk = get_token();
			return true;
		}
		return false;
	} else if (pruchod == 2)
	{
		// Pravidlo c. 12			<id> => identifikator
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
		{
			switch(FIND(*tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, data)) {
				// Identifikator je globalni - pokud se jedna o funkci, ulozi se jeji nazev do function_name
				case FOUND_GLOBAL:
					if ((*data)->type == TABLE_FUNCTION)
						dataSemantika.function_name = (*data)->name;
					dataSemantika.pkid = (*data)->name;
					break;
				// Identifikator je lokalni - v pkid je nazec promenne
				case FOUND_LOCAL:
					dataSemantika.pkid = (*data)->name;
					break;
				case ERR_NOT_FOUND:
					// promena nebo funkce neni definovana, semanticka chyba
					fprintf(stderr, "Semanticka chyba - nedefinovana promenna, nebo funkce.\n");
					errCode = 3;
					return false;
					break;
                case PROGRAM_ERROR:
                    return false;
                    break;
			}

			// Ulozeni prislusnych datovych typu do globalni promenne
			if((*data)->variable != NULL) {
				dataSemantika.data_type = (*data)->variable->data_type;
			} else if((*data)->function != NULL) {
				dataSemantika.return_type = (*data)->function->return_type;
			}
			else
			{
				fprintf(stderr, "Spatne vlozena polozka do tabulky - neni fce ani promenna.\n");
				errCode = 99;
				return false;
			}

			*tk = get_token();
			return true;
		}
		// Pravidlo c. 13			<id> => plne_kvalifikovany_identifikator
		else if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY)
		{
			switch(FIND(*tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, data)) {
				case ERR_NOT_FOUND:
					// promena nebo funkce neni definovana, semanticka chyba
					fprintf(stderr, "Semanticka chyba - nedefinovana promenna, nebo funkce.\n");
					errCode = 3;
					return false;
					break;
                case PROGRAM_ERROR:
                    return false;
                    break;
			}
			if ((*data)->type == TABLE_FUNCTION)
				dataSemantika.function_name = (*data)->name;
			dataSemantika.pkid = (*data)->name;

			// Ulozeni prislusnych datovych typu do globalni promenne
			if((*data)->variable != NULL) {
				dataSemantika.data_type = (*data)->variable->data_type;
			} else if((*data)->function != NULL) {
				dataSemantika.return_type = (*data)->function->return_type;
			}
			else
			{
				fprintf(stderr, "Spatne vlozena polozka do tabulky - neni fce ani promenna.\n");
				errCode = 99;
				return false;
			}

			*tk = get_token();
			return true;
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool DEFINICE_PROMENNE (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 14			<definice_promenne> => ;
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp(";", (*tk)->value) == 0))
		{
			*tk = get_token();
			return true;
		}
		// Pravidlo c. 15			<definice_promenne> => = <vyraz>;
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp("=", (*tk)->value) == 0))
		{
			*tk = get_token();
			if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp(";", (*tk)->value) == 0))
				{
					*tk = get_token();
					return true;
				}
			}
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 14			<definice_promenne> => ;
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp(";", (*tk)->value) == 0))
		{
			*tk = get_token();
			return true;
		}
		// Pravidlo c. 15			<definice_promenne> => = <vyraz>;
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp("=", (*tk)->value) == 0))
		{
			*tk = get_token();
			if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp(";", (*tk)->value) == 0))
				{
					// Vytvoreni instrukce
					struct tInst instrukce;
					// Popsani instrukce I_ASSSIGN
					instrukce.name = I_ASSIGN;
					switch(dataSemantika.expr_type) {
						case DATA_TYPE_STRING:
							instrukce.instType = INST_STRING;
							break;
						case DATA_TYPE_INT:
							instrukce.instType = INST_INT;
							break;
						case DATA_TYPE_DOUBLE:
							instrukce.instType = INST_DOUBLE;
							break;
						case DATA_TYPE_BOOL:
							fprintf(stderr, "Semanticka chyba - Prirazovani datoveho typu bool.\n");
							errCode = 4;
							return false;
						case DATA_TYPE_VOID:
							fprintf(stderr, "Spatny datovy typ vyrazu.\n");
							errCode = 2;
							return false;
						default:
							fprintf(stderr, "Nedefinovany datovy typ.\n");
							errCode = 99;
							return false;
					}

					// Kontrola typu
					if (dataSemantika.data_type == DATA_TYPE_INT && dataSemantika.expr_type != DATA_TYPE_INT)
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
					else if (dataSemantika.data_type == DATA_TYPE_STRING && dataSemantika.expr_type != DATA_TYPE_STRING)
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
					else if (dataSemantika.data_type == DATA_TYPE_DOUBLE &&
							(dataSemantika.expr_type != DATA_TYPE_INT && dataSemantika.expr_type != DATA_TYPE_DOUBLE))
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}

					instrukce.operandType1 = VAR;
					instrukce.operandType2 = CONST;
					instrukce.operandType3 = VAR;
					instrukce.value1.retezec = "#tmp";
					instrukce.value2.cele_cislo = dataSemantika.data_type;
					instrukce.value3.retezec = dataSemantika.pkid;

					// Ulozeni instrukce do seznamu
					if (!InsertInst(&Instrukce, instrukce))
					{
						fprintf(stderr, "Selhalo vkladani instrukce.\n");
						errCode = 99;
						return false;
					}

					*tk = get_token();
					return true;
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool TYP (TokenPtr* tk, int pruchod, int* data_type)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 16			<typ> => String
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("String", (*tk)->value) == 0)
		{
			*data_type = DATA_TYPE_STRING;
			dataSemantika.data_type = DATA_TYPE_STRING;
			*tk = get_token();
			return true;
		}
		// Pravidlo c. 17			<typ> => int
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("int", (*tk)->value) == 0)
		{
			*data_type = DATA_TYPE_INT;
			dataSemantika.data_type = DATA_TYPE_INT;
			*tk = get_token();
			return true;
		}
		// Pravidlo c. 18			<typ> => double
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("double", (*tk)->value) == 0)
		{
			*data_type = DATA_TYPE_DOUBLE;
			dataSemantika.data_type = DATA_TYPE_DOUBLE;
			*tk = get_token();
			return true;
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool SEZNAM_PARAMETRU (TokenPtr* tk, int pruchod, tSymTablePtr data)
{

	if (pruchod == 1)
	{
		// Pravidlo c. 19			<seznam_parametru> => <def_parametru>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			return DEF_PARAMETRU(tk, pruchod, data);
		}
		// Pravidlo c. 20			<seznam_parametru> => epsilon
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp(")", (*tk)->value) == 0))
		{
			// Funkce je bezparametricka
			data->function->no_params = 0;
			free(data->function->params);
			data->function->params = NULL;
			return true;
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 19			<seznam_parametru> => <def_parametru>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			return DEF_PARAMETRU(tk, pruchod, NULL);
		}
		// Pravidlo c. 20			<seznam_parametru> => epsilon
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && (strcmp(")", (*tk)->value) == 0))
		{
			return true;
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool DEF_PARAMETRU (TokenPtr* tk, int pruchod, tSymTablePtr data)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 21			<def_parametru> => <jedna_def_parametru> <def_parametru_2>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			return (JEDNA_DEF_PARAMETRU(tk, pruchod, data) && DEF_PARAMETRU_2(tk, pruchod, data));
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool DEF_PARAMETRU_2 (TokenPtr* tk, int pruchod, tSymTablePtr data)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 22			<def_parametru_2> => epsilon
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
		{
			return true;
		}
		// Pravidlo c. 23			<def_parametru_2> => , <def_parametru>
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(",", (*tk)->value) == 0)
		{
			*tk = get_token();
			return DEF_PARAMETRU(tk, pruchod, data);
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool JEDNA_DEF_PARAMETRU (TokenPtr* tk, int pruchod, tSymTablePtr data)
{
	if (pruchod == 1)
	{
		// Pravidla c. 24			<jedna_def_parametru> => <typ> identifikator
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			int data_type;

			if (TYP(tk, pruchod, &data_type) == true)
			{

				if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
				{
					// Ulozeni parametru do seznamu parametru prislusne funkce
					if (!InsertLastParam (data->function->params, data_type, (*tk)->value, data->function->no_params))
					{
						fprintf(stderr, "Selhalo vkladani parametru.\n");
						errCode = 99;
						return false;
					}
					data->function->no_params += 1;
					*tk = get_token();
					return true;
				}
			}
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidla c. 24			<jedna_def_parametru> => <typ> identifikator
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			if (TYP(tk, pruchod, &(dataSemantika.data_type)) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
				{
					// Vytvoreni polozky pro parametr
					tSymTablePtr data = NULL;

					struct tSymTable item;
					item.type = TABLE_VARIABLE;
					item.name = (*tk)->value;

					if ((item.variable = malloc(sizeof(struct dataVariable))) == NULL)
					{
						fprintf(stderr, "Selhal malloc.\n");
						errCode = 99;
						return false;
					}

					item.variable->data_type = dataSemantika.data_type;
					item.function = NULL;

					// Vlozeni parametru do lokalni TS dane funkce
					int create_return = CREATE(&Lokalni_TS, item, &data);
					if (create_return == PROGRAM_ERROR)
					{
						fprintf(stderr, "Chyba pri vkladani polozky do tabulky.\n" );
						errCode =99;
						return false;
					}
					else if (create_return == ERR_MATCHING)
					{
						fprintf(stderr, "Parametry maji stejny identifikator.\n" );
						errCode =3;
						return false;
					}

					*tk = get_token();
					return true;
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool PRIKAZY (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 25			<prikazy> => <prikaz> <prikazy>
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY
				|| ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
			|| ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("if", (*tk)->value) == 0
				|| strcmp("while", (*tk)->value) == 0 || strcmp("return", (*tk)->value) == 0))
				|| (strcmp("String", (*tk)->value) == 0 || strcmp("int", (*tk)->value) == 0
				|| strcmp("double", (*tk)->value) == 0))
		{
			return (PRIKAZ(tk, pruchod) && PRIKAZY(tk, pruchod));
		}
		// Pravidlo c. 26			<prikazy> => epsilon
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
		{
			return true;
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool PRIKAZ (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 27			<prikaz> => <lokalni_promenna>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
		|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			return LOKALNI_PROMENNA(tk, pruchod);
		}
		// Pravidlo c. 28			<prikaz> => <dilci_prikaz>
		else if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY
			 	|| ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
			 	|| ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("if", (*tk)->value) == 0
					|| strcmp("while", (*tk)->value) == 0 || strcmp("return", (*tk)->value) == 0)))
		{
			return DILCI_PRIKAZ(tk, pruchod);
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool PRIRAZENI (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1)
	{
		while ((*tk)->type != TOKEN_TYPE_SYSTEM_ERROR && (*tk)->type != TOKEN_TYPE_ERROR
				&& (*tk)->type != TOKEN_TYPE_EOF && strcmp(";", (*tk)->value) != 0)
			*tk = get_token();

		if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
		{
			*tk = get_token();
			return true;
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 30			<prirazeni> => <vyvolani_funkce>
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY)
		{
			// Kontrola existence funkce v TS
			tSymTablePtr data = NULL;
			int find_return = FIND(*tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &data);
            if (find_return == PROGRAM_ERROR) {
                return false;
            }
            else if (find_return == ERR_NOT_FOUND)
            {
            	fprintf(stderr, "Pouzita nedeklarovana promenna.\n");
            	errCode = 3;
            	return false;
            }
			if ((find_return == FOUND_LOCAL || find_return == FOUND_GLOBAL ) && data->function != NULL)
			{
				// Ulozeni navratoveho typu funkce a nazvu promenne, do ktere se ma ulozit vysledek volani
				dataSemantika.f_assign_to = dataSemantika.pkid;
				dataSemantika.return_type = data->function->return_type;
				return VYVOLANI_FUNKCE(tk, pruchod);
			}
			else if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
			{
				// Instrukce pro prirazeni
				// Vytvoreni instrukce
				struct tInst instrukce;
				// Popsani instrukce
				instrukce.name = I_ASSIGN;
				switch(dataSemantika.expr_type) {
					case DATA_TYPE_STRING:
						instrukce.instType = INST_STRING;
						break;
					case DATA_TYPE_INT:
						instrukce.instType = INST_INT;
						break;
					case DATA_TYPE_DOUBLE:
						instrukce.instType = INST_DOUBLE;
						break;
					case DATA_TYPE_BOOL:
						fprintf(stderr, "Semanticka chyba - Prirazovani datoveho typu bool.\n");
						errCode = 4;
						return false;
					case DATA_TYPE_VOID:
						fprintf(stderr, "Chybi vyraz u prirazeni.\n");
						errCode = 2;
						return false;
					default:
						fprintf(stderr, "Nedefinovany datovy typ.\n");
						errCode = 99;
						return false;
				}

				// Kontrola typu
				if (dataSemantika.data_type == DATA_TYPE_INT && dataSemantika.expr_type != DATA_TYPE_INT)
				{
					fprintf(stderr, "Semanticka chyba.\n");
					errCode = 4;
					return false;
				}
				else if (dataSemantika.data_type == DATA_TYPE_STRING && dataSemantika.expr_type != DATA_TYPE_STRING)
				{
					fprintf(stderr, "Semanticka chyba.\n");
					errCode = 4;
					return false;
				}
				else if (dataSemantika.data_type == DATA_TYPE_DOUBLE &&
						(dataSemantika.expr_type != DATA_TYPE_INT && dataSemantika.expr_type != DATA_TYPE_DOUBLE))
				{
					fprintf(stderr, "Semanticka chyba.\n");
					errCode = 4;
					return false;
				}

				instrukce.operandType1 = VAR;
				instrukce.operandType2 = CONST;
				instrukce.operandType3 = VAR;
				instrukce.value1.retezec = "#tmp";
				instrukce.value2.cele_cislo = dataSemantika.data_type;
				instrukce.value3.retezec = dataSemantika.pkid;

				// Ulozeni instrukce do seznamu
				if (!InsertInst(&Instrukce, instrukce))
				{
					fprintf(stderr, "Selhalo vlozeni instrukce.\n");
					errCode = 99;
					return false;
				}


				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
				{
					*tk = get_token();
					return true;
				}
			}
			return false;
		}
		// Pravidlo c. 29			<prirazeni> => <vyraz>;
		else if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
		{
			// Instrukce pro prirazeni
			// Vytvoreni instrukce
			struct tInst instrukce;
			// Popsani instrukce
			instrukce.name = I_ASSIGN;
			switch(dataSemantika.expr_type) {
				case DATA_TYPE_STRING:
					instrukce.instType = INST_STRING;
					break;
				case DATA_TYPE_INT:
					instrukce.instType = INST_INT;
					break;
				case DATA_TYPE_DOUBLE:
					instrukce.instType = INST_DOUBLE;
					break;
				case DATA_TYPE_BOOL:
					fprintf(stderr, "Semanticka chyba - Prirazovani datoveho typu bool.\n");
					errCode = 4;
					return false;
				case DATA_TYPE_VOID:
					fprintf(stderr, "Chybi vyraz u prirazeni.\n");
					errCode = 2;
					return false;
				default:
					fprintf(stderr, "Nedefinovany datovy typ.\n");
					errCode = 99;
					return false;
			}

			// Kontrola typu
			if (dataSemantika.data_type == DATA_TYPE_INT && dataSemantika.expr_type != DATA_TYPE_INT)
			{
				fprintf(stderr, "Semanticka chyba.\n");
				errCode = 4;
				return false;
			}
			else if (dataSemantika.data_type == DATA_TYPE_STRING && dataSemantika.expr_type != DATA_TYPE_STRING)
			{
				fprintf(stderr, "Semanticka chyba.\n");
				errCode = 4;
				return false;
			}
			else if (dataSemantika.data_type == DATA_TYPE_DOUBLE &&
					(dataSemantika.expr_type != DATA_TYPE_INT && dataSemantika.expr_type != DATA_TYPE_DOUBLE))
			{
				fprintf(stderr, "Semanticka chyba.\n");
				errCode = 4;
				return false;
			}

			instrukce.operandType1 = VAR;
			instrukce.operandType2 = CONST;
			instrukce.operandType3 = VAR;
			instrukce.value1.retezec = "#tmp";
			instrukce.value2.cele_cislo = dataSemantika.data_type;
			instrukce.value3.retezec = dataSemantika.pkid;

			// Ulozeni instrukce do seznamu
			if (!InsertInst(&Instrukce, instrukce))
			{
				fprintf(stderr, "Selhalo vlozeni instrukce.\n");
				errCode = 99;
				return false;
			}


			if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
			{
				*tk = get_token();
				return true;
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool DILCI_PRIKAZ (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1)
	{
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY)
		{
			// Pravidlo c. 31			<dilci_prikaz> => <id> <dilci_prikaz2>
			if (strcmp("ifj16.print", (*tk)->value) != 0) {
				if(ID(tk, pruchod, NULL)) {
					return DILCI_PRIKAZ_2(tk, pruchod, NULL);
				} else {
					return false;
				}
			}
			// Pravidlo c. 53			<dilci_prikaz> => ifj16.print (<term_konkatenace>);
			else
			{
				*tk = get_token();
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
				{
					*tk = get_token();
					if (TERM_KONKATENACE(tk, pruchod) == true)
					{
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
						{
							*tk = get_token();
							if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
							{
								*tk = get_token();
								return true;
							}
						}
					}
				}
			}
			return false;
		}
		// Pravidlo c. 32			<dilci_prikaz> => {<dilci_prikazy>}
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
		{
			*tk = get_token();
			if (DILCI_PRIKAZY(tk, pruchod) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
				{
					*tk = get_token();
					return true;
				}
			}
			return false;
		}
		// Pravidlo c. 33			<dilci_prikaz> => if (<vyraz>) {<dilci_prikazy>} else {<dilci_prikazy>}
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("if", (*tk)->value) == 0)
		{
			*tk = get_token();
			if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
			{
				*tk = get_token();
				if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
				{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
					{
						*tk = get_token();
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
						{
							*tk = get_token();
							if (DILCI_PRIKAZY(tk, pruchod) == true)
							{
								if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
								{
									*tk = get_token();
									if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("else", (*tk)->value) == 0)
									{
										*tk = get_token();
										if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
										{
											*tk = get_token();
											if (DILCI_PRIKAZY(tk, pruchod) == true)
											{
												if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
												{
													*tk = get_token();
													return true;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			return false;
		}
		// Pravidlo c. 34			<dilci_prikaz> => while (<vyraz>) {<dilci_prikazy>}
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("while", (*tk)->value) == 0)
		{

			*tk = get_token();
			if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
			{
				*tk = get_token();
				if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
				{
					if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
					{
						*tk = get_token();
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
						{
							*tk = get_token();
							if (DILCI_PRIKAZY(tk, pruchod) == true)
							{
								if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
								{
									*tk = get_token();
									return true;
								}
							}
						}
					}
				}
			}
			return false;
		}
		// Pravidlo c. 35			<dilci_prikaz> => return <vyraz_navratu>;
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("return", (*tk)->value) == 0)
		{
			*tk = get_token();
			if (VYRAZ_NAVRATU(tk, pruchod) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
				{
					*tk = get_token();
					return true;
				}
			}
		}
		return false;
	}
	else if (pruchod == 2)
	{
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY)
		{
			// Pravidlo c. 31			<dilci_prikaz> => <id> <dilci_prikaz2>
			if (strcmp("ifj16.print", (*tk)->value) != 0)
			{
				tSymTablePtr data = NULL;
				if(ID(tk, pruchod, &data)) {
					return DILCI_PRIKAZ_2(tk, pruchod, data);
				} else {
					return false;
				}
			}
			else			// Pravidlo c. 53			<dilci_prikaz> => ifj16.print (<term_konkatenace>);
			{
				dataSemantika.pkid = (*tk)->value;
				*tk = get_token();
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
				{
					*tk = get_token();
					if (TERM_KONKATENACE(tk, pruchod) == true)
					{
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
						{
							*tk = get_token();
							if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
							{
								// Vytvoreni instrukce pro vestavenou funkci ifj16.print
								struct tInst instrukce1;
								instrukce1.name = I_PRINT;
								instrukce1.instType = INST_FUNCTION;

								instrukce1.operandType1 = NDEF;
								instrukce1.operandType2 = NDEF;
								instrukce1.operandType3 = VAR;

								instrukce1.value3.retezec = "#tmp";

								// Ulozeni instrukce do seznamu
								if (!InsertInst(&Instrukce, instrukce1))
								{
									fprintf(stderr, "Selhalo vlozeni instrukce.\n");
									errCode = 99;
									return false;
								}

								*tk = get_token();
								return true;
							}
						}
					}
				}
			}
			return false;
		}
		// Pravidlo c. 32			<dilci_prikaz> => {<dilci_prikazy>}
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
		{
			*tk = get_token();
			if (DILCI_PRIKAZY(tk, pruchod) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
				{
					*tk = get_token();
					return true;
				}
			}
			return false;
		}
		// Pravidlo c. 33			<dilci_prikaz> => if (<vyraz>) {<dilci_prikazy>} else {<dilci_prikazy>}
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("if", (*tk)->value) == 0)
		{
			*tk = get_token();
			if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
			{
				*tk = get_token();
				if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
				{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
					{
						// Kontrola datoveho typu
						if (dataSemantika.expr_type != DATA_TYPE_BOOL)
						{
							if (dataSemantika.expr_type == DATA_TYPE_VOID)
							{
								fprintf(stderr, "Chybejici vyraz v podmince.\n");
								errCode = 2;
								return false;
							}
							else
							{
								fprintf(stderr, "Semanticka chyba - spatny datovy typ v podmince.\n");
								errCode = 4;
								return false;
							}
						}

						// Generovani if..else ridici struktury
						// Generovani if vetve
						// Vytvoreni instrukce
						// (I_JMPN, hodnota_vyrazu, , L1)
						struct tInst instrukce1;
						// Popsani instrukce
						instrukce1.name = I_JMPN;
						instrukce1.instType = INST_JMP;

						instrukce1.operandType1 = VAR;
						instrukce1.operandType2 = NDEF;
						instrukce1.operandType3 = CONST;
						// tvorba unikatniho labelu
						char * label1 = uniqueLabel();
                        if (label1 == NULL) {
                            return false;
                        }

						instrukce1.value1.retezec = "#tmp";
						instrukce1.value3.retezec = label1;

						// Ulozeni instrukce do seznamu
						if (!InsertInst(&Instrukce, instrukce1))
						{
							fprintf(stderr, "Selhalo vlozeni instrukce.\n");
							errCode = 99;
							return false;
						}

						*tk = get_token();
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
						{
							*tk = get_token();
							if (DILCI_PRIKAZY(tk, pruchod) == true)
							{
								if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
								{
									// generovani else vetve
									// (I_GOTO, , , L2)
									struct tInst instrukce2;
									// Popsani instrukce
									instrukce2.name = I_GOTO;
									instrukce2.instType = INST_JMP;

									instrukce2.operandType1 = NDEF;
									instrukce2.operandType2 = NDEF;
									instrukce2.operandType3 = CONST;

									char * label2 = uniqueLabel();
                                    if (label2 == NULL) {
                                        return false;
                                    }
									instrukce2.value3.retezec = label2;

									// Ulozeni instrukce do seznamu
									InsertInst(&Instrukce, instrukce2);


									// (I_LABEL, L1, , )
									// Popsani instrukce
									struct tInst instrukce3;
									instrukce3.name = I_LABEL;
									instrukce3.instType = INST_LABEL;

									instrukce3.operandType1 = CONST;
									instrukce3.operandType2 = NDEF;
									instrukce3.operandType3 = NDEF;

									instrukce3.value1.retezec = label1;

									// Ulozeni instrukce do seznamu
									if (!InsertInst(&Instrukce, instrukce3))
									{
										fprintf(stderr, "Selhalo vlozeni instrukce.\n");
										errCode = 99;
										return false;
									}

									*tk = get_token();
									if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("else", (*tk)->value) == 0)
									{
										*tk = get_token();
										if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
										{
											*tk = get_token();
											if (DILCI_PRIKAZY(tk, pruchod) == true)
											{
												// (I_LABEL, L2, , )
												struct tInst instrukce4;
												// Popsani instrukce
												instrukce4.name = I_LABEL;
												instrukce4.instType = INST_LABEL;

												instrukce4.operandType1 = CONST;
												instrukce4.operandType2 = NDEF;
												instrukce4.operandType3 = NDEF;

												instrukce4.value1.retezec = label2;

												// Ulozeni instrukce do seznamu
												if (!InsertInst(&Instrukce, instrukce4))
												{
													fprintf(stderr, "Selhalo vlozeni instrukce.\n");
													errCode = 99;
													return false;
												}

												if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
												{
													*tk = get_token();
													return true;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			return false;
		}
		// Pravidlo c. 34			<dilci_prikaz> => while (<vyraz>) {<dilci_prikazy>}
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("while", (*tk)->value) == 0)
		{
			*tk = get_token();
			struct tInst instrukce1;
			// (I_LABEL, L3, , )
			// Popsani instrukce
			instrukce1.name = I_LABEL;
			instrukce1.instType = INST_LABEL;

			instrukce1.operandType1 = CONST;
			instrukce1.operandType2 = NDEF;
			instrukce1.operandType3 = NDEF;

			char * label3 = uniqueLabel();
            if (label3 == NULL) {
                return false;
            }
			instrukce1.value1.retezec = label3;

			// Ulozeni instrukce do seznamu
			if (!InsertInst(&Instrukce, instrukce1))
			{
				fprintf(stderr, "Selhalo vlozeni instrukce.\n");
				errCode = 99;
				return false;
			}

			if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
			{
				*tk = get_token();
				if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
				{
					if (dataSemantika.expr_type != DATA_TYPE_BOOL)
					{
						if (dataSemantika.expr_type == DATA_TYPE_VOID)
						{
							fprintf(stderr, "Chybejici vyraz v podmince.\n");
							errCode = 2;
							return false;
						}
						else
						{
							fprintf(stderr, "Spatny datovy typ podminky.\n");
							errCode = 4;
							return false;
						}
					}

					// (I_JMPN, L4, , )
					// Popsani instrukce
					instrukce1.name = I_JMPN;
					instrukce1.instType = INST_JMP;

					instrukce1.operandType1 = VAR;
					instrukce1.operandType2 = NDEF;
					instrukce1.operandType3 = CONST;

					instrukce1.value1.retezec = "#tmp";

					char * label4 = uniqueLabel();
		            if (label3 == NULL) {
		                return false;
		            }
					instrukce1.value3.retezec = label4;

					// Ulozeni instrukce do seznamu
					if (!InsertInst(&Instrukce, instrukce1))
					{
						fprintf(stderr, "Selhalo vlozeni instrukce.\n");
						errCode = 99;
						return false;
					}

					if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
					{
						*tk = get_token();
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
						{
							*tk = get_token();
							if (DILCI_PRIKAZY(tk, pruchod) == true)
							{
								if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
								{
									// (I_GOTO, L3, , )
									// Popsani instrukce
									instrukce1.name = I_GOTO;
									instrukce1.instType = INST_JMP;

									instrukce1.operandType1 = NDEF;
									instrukce1.operandType2 = NDEF;
									instrukce1.operandType3 = CONST;

									instrukce1.value3.retezec = label3;
									// Ulozeni instrukce do seznamu
									if (!InsertInst(&Instrukce, instrukce1))
									{
										fprintf(stderr, "Selhalo vlozeni instrukce.\n");
										errCode = 99;
										return false;
									}

									// (I_LABEL, L4, , )
									// Popsani instrukce
									instrukce1.name = I_LABEL;
									instrukce1.instType = INST_LABEL;

									instrukce1.operandType1 = CONST;
									instrukce1.operandType2 = NDEF;
									instrukce1.operandType3 = NDEF;

									instrukce1.value1.retezec = label4;
									// Ulozeni instrukce do seznamu
									if (!InsertInst(&Instrukce, instrukce1))
									{
										fprintf(stderr, "Selhalo vlozeni instrukce.\n");
										errCode = 99;
										return false;
									}

									*tk = get_token();
									return true;
								}
							}
						}
					}
				}
			}
			return false;
		}
		// Pravidlo c. 35			<dilci_prikaz> => return <vyraz_navratu>;
		else if ((*tk)->type == TOKEN_TYPE_KEYWORD && strcmp("return", (*tk)->value) == 0)
		{
			*tk = get_token();
			if (VYRAZ_NAVRATU(tk, pruchod) == true)
			{
				// Vygenerovani return instrukci
				// (I_RETURN, , , navratova_hodnota);
				struct tInst instrukce1;
				// Popsani instrukce
				instrukce1.name = I_RET;
				instrukce1.instType = INST_FUNCTION;

				instrukce1.operandType1 = NDEF;
				instrukce1.operandType2 = NDEF;
				instrukce1.operandType3 = NDEF;

				// Ulozeni instrukce do seznamu
				if (!InsertInst(&Instrukce, instrukce1))
				{
					fprintf(stderr, "Selhalo vlozeni instrukce.\n");
					errCode = 99;
					return false;
				}

				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
				{
					*tk = get_token();
					return true;
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool DILCI_PRIKAZ_2 (TokenPtr* tk, int pruchod, tSymTablePtr data)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 36			<dilci_prikaz2> => = <prirazeni>
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("=", (*tk)->value) == 0)
		{
			*tk = get_token();
			return PRIRAZENI(tk, pruchod);
		}
		// Pravidlo c. 37			<dilci_prikaz2> => (<seznam_vstupnich_parametru>);
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
		{
			*tk = get_token();
			if (SEZNAM_VSTUPNICH_PARAMETRU(tk, pruchod, NULL) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
				{
					*tk = get_token();
					if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
					{
						*tk = get_token();
						return true;
					}
				}
			}
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 36			<dilci_prikaz2> => = <prirazeni>
		// Vyvolani funkce s prirazenim navratove hodnoty
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("=", (*tk)->value) == 0)
		{
			*tk = get_token();
			return PRIRAZENI(tk, pruchod);
		}
		// Pravidlo c. 37			<dilci_prikaz2> => (<seznam_vstupnich_parametru>);
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
		{
			// (I_FUNC, , , pkid)
			struct tInst instrukce1;
			// Popsani instrukce
			instrukce1.name = I_FUNC;
			instrukce1.instType = transType(dataSemantika.return_type);

			instrukce1.operandType1 = CONST;
			instrukce1.operandType2 = NDEF;
			instrukce1.operandType3 = NDEF;

			instrukce1.value1.retezec = dataSemantika.pkid;

			// Ulozeni instrukce do seznamu
			if (!InsertInst(&Instrukce, instrukce1))
			{
				fprintf(stderr, "Selhalo vlozeni instrukce.\n");
				errCode = 99;
				return false;
			}

			*tk = get_token();
			if (SEZNAM_VSTUPNICH_PARAMETRU(tk, pruchod, data) == true)
			{

				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
				{
					// (I_FSTART, , , pkid)
					struct tInst instrukce2;
					// Popsani instrukce
					instrukce2.name = I_FSTART;
					instrukce2.instType = INST_FUNCTION;

					instrukce2.operandType1 = NDEF;
					instrukce2.operandType2 = NDEF;
					instrukce2.operandType3 = CONST;

					instrukce2.value3.retezec = dataSemantika.function_name;

					// Ulozeni instrukce do seznamu
					if (!InsertInst(&Instrukce, instrukce2))
					{
						fprintf(stderr, "Selhalo vlozeni instrukce.\n");
						errCode = 99;
						return false;
					}

					*tk = get_token();
					if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
					{
						*tk = get_token();
						return true;
					}
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool DILCI_PRIKAZY (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 38			<dilci_prikazy> => <dilci_prikaz> <dilci_prikazy>
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY
			|| ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("{", (*tk)->value) == 0)
			|| ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("if", (*tk)->value) == 0
				|| strcmp("while", (*tk)->value) == 0
				|| strcmp("return", (*tk)->value) == 0)))
			return (DILCI_PRIKAZ(tk, pruchod) && DILCI_PRIKAZY(tk, pruchod));
		// Pravidlo c. 39			<dilci_prikazy> => epsilon
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("}", (*tk)->value) == 0)
			return true;

		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool VYVOLANI_FUNKCE (TokenPtr* tk, int pruchod)
{
	if (pruchod == 2)
	{
		// Pravidlo c. 40			<vyvolani_funkce> => <id> ( <seznam_vstupnich_parametru> );
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY)
		{
			tSymTablePtr data = NULL;
			if (ID(tk, pruchod, &data) == true)
			{
				// (I_FUNC, , , pkid)
				struct tInst instrukce1;
				// Popsani instrukce
				instrukce1.name = I_FUNC;
				instrukce1.instType = transType(dataSemantika.return_type);

				instrukce1.operandType1 = CONST;
				instrukce1.operandType2 = NDEF;

				instrukce1.value1.retezec = dataSemantika.pkid;

				if (dataSemantika.f_assign_to == NULL)
				{
					instrukce1.operandType3 = NDEF;
				}
				else
				{
					instrukce1.operandType3 = VAR;
					instrukce1.value3.retezec = dataSemantika.f_assign_to;

					// Kontrola typu
					if (dataSemantika.return_type == DATA_TYPE_VOID)
					{
						fprintf(stderr, "Prirazeni navratove hodnoty void funkce.\n");
						errCode = 8;
						return false;
					}
					else if (dataSemantika.data_type == DATA_TYPE_INT && dataSemantika.return_type != DATA_TYPE_INT)
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
					else if (dataSemantika.data_type == DATA_TYPE_STRING && dataSemantika.return_type != DATA_TYPE_STRING)
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
					else if (dataSemantika.data_type == DATA_TYPE_DOUBLE &&
							(dataSemantika.return_type != DATA_TYPE_INT && dataSemantika.return_type != DATA_TYPE_DOUBLE))
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
					dataSemantika.f_assign_to = NULL;
				}

				// Ulozeni instrukce do seznamu
				if (!InsertInst(&Instrukce, instrukce1))
				{
					fprintf(stderr, "Selhalo vlozeni instrukce.\n");
					errCode = 99;
					return false;
				}

				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
				{
					*tk = get_token();
					if (SEZNAM_VSTUPNICH_PARAMETRU(tk, pruchod, data) == true)
					{
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
						{
							// (I_FSTART, , , pkid)
							struct tInst instrukce2;
							// Popsani instrukce
							instrukce2.name = I_FSTART;
							instrukce2.instType = INST_FUNCTION;

							instrukce2.operandType1 = NDEF;
							instrukce2.operandType2 = NDEF;
							instrukce2.operandType3 = CONST;
							instrukce2.value3.retezec = dataSemantika.function_name;

							// Ulozeni instrukce do seznamu
							InsertInst(&Instrukce, instrukce2);

							*tk = get_token();
							if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
							{
								*tk = get_token();
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	else if (pruchod == 1)
	{
		// Pravidlo c. 40			<vyvolani_funkce> => <id> ( <seznam_vstupnich_parametru> );
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY)
		{
			if (ID(tk, pruchod, NULL) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp("(", (*tk)->value) == 0)
				{
					*tk = get_token();
					if (SEZNAM_VSTUPNICH_PARAMETRU(tk, pruchod, NULL) == true)
					{
						if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
						{
							*tk = get_token();
							if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
							{
								*tk = get_token();
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool SEZNAM_VSTUPNICH_PARAMETRU (TokenPtr* tk, int pruchod, tSymTablePtr data)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 41			<seznam_vstupnich_parametru> => <vstupni_parametr>
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY
			|| (*tk)->type == TOKEN_TYPE_DOUBLE || (*tk)->type == TOKEN_TYPE_INT || (*tk)->type == TOKEN_TYPE_STRING)
			return VSTUPNI_PARAMETR(tk, pruchod, data);
		// Pravidlo c. 42			<seznam_vstupnich_parametru> => epsilon
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
			return true;

		return false;
	}
	else if (pruchod == 2)
	{
		param_index = 0;
		// Pravidlo c. 41			<seznam_vstupnich_parametru> => <vstupni_parametr>
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY
			|| (*tk)->type == TOKEN_TYPE_DOUBLE || (*tk)->type == TOKEN_TYPE_INT || (*tk)->type == TOKEN_TYPE_STRING)
			return VSTUPNI_PARAMETR(tk, pruchod, data);
		// Pravidlo c. 42			<seznam_vstupnich_parametru> => epsilon
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
		{
			if (data->function->no_params == 0)
				return true;
			else
			{
				fprintf(stderr, "Semanticka chyba - chybi parametry funkce.\n");
				errCode = 4;
				return false;
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool VSTUPNI_PARAMETR (TokenPtr* tk, int pruchod, tSymTablePtr data)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 43			<vstupni_parametr> => <term> <dalsi_vstupni_parametr>
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY
			|| (*tk)->type == TOKEN_TYPE_DOUBLE || (*tk)->type == TOKEN_TYPE_INT || (*tk)->type == TOKEN_TYPE_STRING)
			return (TERM(tk, pruchod) && DALSI_VSTUPNI_PARAMETR(tk, pruchod, data));

		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 43			<vstupni_parametr> => <term> <dalsi_vstupni_parametr>
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY
			|| (*tk)->type == TOKEN_TYPE_DOUBLE || (*tk)->type == TOKEN_TYPE_INT || (*tk)->type == TOKEN_TYPE_STRING)
		{
			if (param_index < data->function->no_params)
			{
				// Vyvolani funkce s argumenty
				// (I_PARAM, , , )
				struct tInst instrukce;
				// Popsani instrukce
				instrukce.name = I_PARAM;

				// Vyhledani prislusneho parametru funkce pro kontrolu datovych typu
				tParamElemPtr current_param = SearchParamList (data->function->params, param_index);
				if (current_param == NULL)
				{
					fprintf(stderr, "Selhalo hledani parametru.\n");
					errCode = 99;
					return false;
				}

				tSymTablePtr par = NULL;
				int found;
				switch((*tk)->type) {
					// Argumentem funkce je identifikator - hledani v TS a kontrola typu
					case TOKEN_TYPE_IDENTIFIKATOR:
					case TOKEN_TYPE_IDENTIFIKATOR_SLOZENY:
						found = FIND(*tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &par);
                        if (found == PROGRAM_ERROR) {
                            return false;
                        }
                        if ((found == FOUND_GLOBAL || found == FOUND_LOCAL) && (par->type == TABLE_VARIABLE))
						{
							if ((current_param->type == DATA_TYPE_STRING && par->variable->data_type == DATA_TYPE_STRING) ||
								  ((current_param->type == DATA_TYPE_DOUBLE) && (par->variable->data_type == DATA_TYPE_DOUBLE ||
								  	par->variable->data_type == DATA_TYPE_INT)) || (current_param->type == DATA_TYPE_INT &&
								      par->variable->data_type == DATA_TYPE_INT))
							{
								instrukce.instType = INST_STRING;
								instrukce.operandType2 = VAR;
								instrukce.value2.retezec = par->name;
							}
							else
							{
								fprintf(stderr, "Nespravny parametr.\n");
								errCode = 4;
								return false;
							}
						}
						else
						{
							fprintf(stderr, "Nespravny parametr.\n");
							errCode = 3;
							return false;
						}
						break;

					// Argumentem je term - kontrola typu
					case TOKEN_TYPE_DOUBLE:
						instrukce.instType = INST_DOUBLE;
						instrukce.operandType2 = CONST;
						instrukce.value2.desetinne_cislo = atof((*tk)->value);
						if (current_param->type != DATA_TYPE_DOUBLE)
						{
							fprintf(stderr, "Nespravny parametr.\n");
							errCode = 4;
							return false;
						}
						break;
					case TOKEN_TYPE_INT:
						instrukce.instType = INST_INT;
						instrukce.operandType2 = CONST;
						instrukce.value2.cele_cislo = atoi((*tk)->value);
						if (current_param->type != DATA_TYPE_INT)
						{
							if (current_param->type == DATA_TYPE_DOUBLE)
							{
								double pom = (double) instrukce.value2.cele_cislo;
								instrukce.instType = INST_DOUBLE;
								instrukce.value2.desetinne_cislo = pom;
							}
							else
							{
								fprintf(stderr, "Nespravny parametr.\n");
								errCode = 4;
								return false;
							}
						}
						break;
					case TOKEN_TYPE_STRING:
						instrukce.instType = INST_STRING;
						instrukce.operandType2 = CONST;
						instrukce.value2.retezec = (*tk)->value;
						if (current_param->type != DATA_TYPE_STRING)
						{
							fprintf(stderr, "Nespravny parametr.\n");
							errCode = 4;
							return false;
						}
						break;
				}

				instrukce.operandType1 = VAR;
				instrukce.value1.retezec = current_param->name;
				instrukce.operandType3 = NDEF;

				// Ulozeni instrukce do seznamu
				if (!InsertInst(&Instrukce, instrukce))
				{
					fprintf(stderr, "Selhalo vlozeni instrukce.\n");
					errCode = 99;
					return false;
				}

				// Zvyseni pocitadla parametru
				param_index += 1;
				return (TERM(tk, pruchod) && DALSI_VSTUPNI_PARAMETR(tk, pruchod, data));
			}
			else
			{
				fprintf(stderr, "Spatny pocet parametru.\n");
				errCode = 4;
				return false;
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool DALSI_VSTUPNI_PARAMETR (TokenPtr* tk, int pruchod, tSymTablePtr data)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 44			<dalsi_vstupni_parametr> => , <vstupni_parametr>
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(",", (*tk)->value) == 0)
		{
			*tk = get_token();
			return VSTUPNI_PARAMETR(tk, pruchod, data);
		}
		// Pravidlo c. 45			<dalsi_vstupni_parametr> => epsilon
		else if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(")", (*tk)->value) == 0)
		{
			if (pruchod == 2)
			{
				// Kontrola poctu argumentu funkce
				if (param_index != data->function->no_params)
				{
					fprintf(stderr, "Nespravny pocet parametru.\n");
					errCode = 4;
					return false;
				}
			}
			return true;
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool TERM (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 46			<term> => <id>
		if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR || (*tk)->type == TOKEN_TYPE_IDENTIFIKATOR_SLOZENY)
		{
			if (pruchod == 1)
				return ID(tk, pruchod, NULL);
			else
			{
				tSymTablePtr data = NULL;
				if (ID(tk, pruchod, &data) == true)
				{
					if (data != NULL && data->type == TABLE_VARIABLE)
						return true;
					else
					{
						fprintf(stderr, "Nespravny parametr.\n");
						errCode = 4;
						return false;
					}
				}
			}
		}
		// Pravidlo c. 47			<term> => desetinny_literal
		else if ((*tk)->type == TOKEN_TYPE_DOUBLE)
		{
			*tk = get_token();
			return true;
		}
		// Pravidlo c. 48			<term> => celociselny_literal
		else if ((*tk)->type == TOKEN_TYPE_INT)
		{
			*tk = get_token();
			return true;
		}
		// Pravidlo c. 49			<term> => retezcovy_literal
		else if ((*tk)->type == TOKEN_TYPE_STRING)
		{
			*tk = get_token();
			return true;
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool VYRAZ_NAVRATU (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1 || pruchod == 2)
	{
		// Pravidlo c. 51			<vyraz_navratu> => epsilon
		if ((*tk)->type == TOKEN_TYPE_OPERATOR && strcmp(";", (*tk)->value) == 0)
			return true;
		// Pravidlo c. 50			<vyraz_navratu> => <vyraz>
		else
		{
			if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce))
			{
				if (pruchod == 2)
				{
					// Kontrola typu
					if (dataSemantika.return_type == DATA_TYPE_INT && dataSemantika.expr_type != DATA_TYPE_INT)
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
					else if (dataSemantika.return_type == DATA_TYPE_STRING && dataSemantika.expr_type != DATA_TYPE_STRING)
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
					else if (dataSemantika.return_type == DATA_TYPE_DOUBLE &&
							(dataSemantika.expr_type != DATA_TYPE_INT && dataSemantika.expr_type != DATA_TYPE_DOUBLE))
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
					else if (dataSemantika.return_type == DATA_TYPE_VOID && dataSemantika.expr_type != DATA_TYPE_VOID)
					{
						fprintf(stderr, "Semanticka chyba.\n");
						errCode = 4;
						return false;
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool LOKALNI_PROMENNA (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1)
	{
		// Pravidlo c. 52			<lokalni_promenna> => <typ> identifikator <definice_promenne>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			int data_type;
			if (TYP(tk, pruchod, &data_type) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
				{
					*tk = get_token();
					return DEFINICE_PROMENNE(tk, pruchod);
				}
			}
		}
		return false;
	}
	else if (pruchod == 2)
	{
		// Pravidlo c. 52			<lokalni_promenna> => <typ> identifikator <definice_promenne>
		if ((*tk)->type == TOKEN_TYPE_KEYWORD && (strcmp("String", (*tk)->value) == 0
			|| strcmp("int", (*tk)->value) == 0 || strcmp("double", (*tk)->value) == 0))
		{
			if (TYP(tk, pruchod, &dataSemantika.data_type) == true)
			{
				if ((*tk)->type == TOKEN_TYPE_IDENTIFIKATOR)
				{
					// Ulozeni jmena promenne
					dataSemantika.pkid = (*tk)->value;

					tSymTablePtr data = NULL;

					struct tSymTable item;
					item.type = TABLE_VARIABLE;
					item.name = dataSemantika.pkid;

					if ((item.variable = malloc(sizeof(struct dataVariable))) == NULL)
					{
						fprintf(stderr, "Selhal malloc.\n");
						errCode = 99;
						return false;
					}

					item.variable->data_type = dataSemantika.data_type;
					item.function = NULL;

					// Semanticka kontrola - neexistuje funkce stejneho jmena
					tSymTablePtr control_data = NULL;
					if (FIND(*tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &control_data) == FOUND_GLOBAL)
					{
						if (control_data != NULL && control_data->type == TABLE_FUNCTION)
						{
							fprintf(stderr, "Pokus o definici promenne stejneho jmena jako ma funkce.\n");
							errCode = 3;
							return false;
						}
					}

					int create_return = CREATE(&Lokalni_TS, item, &data);

					if (create_return == ERR_MATCHING)
					{
						fprintf(stderr, "Pokus o definici promenne jiz pouziteho jmena.\n" );
						errCode = 3;
						return false;
					}
					else if (create_return == PROGRAM_ERROR)
					{
						fprintf(stderr, "Programova chyba pri vytvareni polozky v tabulce. \n" );
						errCode = 99;
						return false;
					}

					*tk = get_token();
					return DEFINICE_PROMENNE(tk, pruchod);
				}
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

bool TERM_KONKATENACE (TokenPtr* tk, int pruchod)
{
	if (pruchod == 1)
	{
		//Pravidlo c. 54			<term_konkatenace> => <vyraz>
		if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
		{
			return true;
		}

		return false;
	}
	else if (pruchod == 2)
	{
		//Pravidlo c. 54			<term_konkatenace> => <vyraz>
		if (syntax_expr(tk, dataSemantika.class_name, Lokalni_TS, Globalni_TS, &dataSemantika.expr_type, pruchod, &Instrukce) == true)
		{

			if (dataSemantika.expr_type == DATA_TYPE_STRING || dataSemantika.expr_type == DATA_TYPE_INT
				 || dataSemantika.expr_type == DATA_TYPE_DOUBLE)
			{
				return true;
			}
			else
			{
				fprintf(stderr, "Spatne datove typy.\n");
				errCode = 4;
				return false;
			}
		}
		return false;
	}
	else
	{
		fprintf(stderr, "Spatne cislo pruchodu.\n");
		errCode = 99;
		return false;
	}
}

// Funkce pro generovani unikatniho jmena navesti
char * uniqueLabel()
{
	static unsigned long counter = 0;
	char *label = malloc(20*sizeof(char));
    if (label == NULL) {
        fprintf (stderr, "Selhal malloc\n");
        errCode = 99;
        return NULL;
    }
	sprintf(label, "%lu", counter);
	counter++;
	return label;
}
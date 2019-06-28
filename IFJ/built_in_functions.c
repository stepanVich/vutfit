/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * built_in_functions.c
 * 
 * Petr Mohelnik,       xmohel02
 * Monika Muzikovska,   xmuzik05
 * Dominik Turecek,     xturec06
 */


#include "built_in_functions.h"

/**
 * @brief  Funkce vraci delku retezce.
 * @param  str  Retezec, ktereho pocitame delku
 * @return  int  Delka retezce
 */
int length(const char *str) {
    char *mystr = convert_str(str);
    int len = strlen(mystr);
    free(mystr);
    mystr = NULL;
    return len;
}

/**
 * @brief  Porovnava dva retezce
 * @param  str1  Prvni retezec
 * @param  str2  Druhy retezec
 * @return  -1  Prvni retezec je mensi
 * @return  0  Oba retezce jsou stejne
 * @return  1  Prvni retezec je vetsi
 */
int compare(const char *str1, const char *str2) {
    char *first = convert_str(str1);
    char *second = convert_str(str2);
    int i = strcmp(first, second);
    free(first);
    first = NULL;
    free(second);
    second = NULL;
    if(i < 0) {  // prvni mensi
        return -1;
    }
    if(i > 0) {  // prvni vetsi
        return 1;
    }
    // jinak se rovnaji
    return 0;
}

/**
 * @brief  Funkce vybrani podretezce
 * @param  str  Ze ktereho retezece.
 * @param  i  Od ktereho znaku zacit
 * @param  n  Kolik znaku vybrat
 * @return  char*  Pozadovana cast retezce.
 *          NULL  Pri chybe.
 */
char *substr(const char *str, int i, int n) {
    if(str == NULL) {  // nebyl zadany retezec
        return NULL;
    }
    if(i < 0 || n < 0) { // nevhodne hodnoty i nebo n
        return NULL;
    }
    char *mystr = convert_str(str);
    if(mystr == NULL) {  // chyba pri alokaci;
        return NULL;
    }
    if(i+n > (int)strlen(mystr)) {  // aby se necetlo mimo pole
    	return NULL;
    }

    char *sub = malloc(n+1);
    if(sub == NULL) {  // chyba pri alokaci
        free(mystr);
        mystr = NULL;
        return NULL;
    } else {  // kopirovani potrebne casti retezce
        memcpy(sub, mystr+i, n);
        sub[n] = 0;
        free(mystr);
        mystr = NULL;
        return sub;
    }
}

/**
 * @brief  Funkce pro zvetseni mista pro retezec
 * @param  s  Retezec
 * @param  cap  Pozadovana velikost
 * @return  char*  Novy retezec
 *          NULL  Pri chybe.
 */
char* tryRealloc(char* s, int cap)
{
	char* new = realloc(s, cap*sizeof(char));
	if (new == NULL)
	{
		free(s);
		s = NULL;
		return NULL;
	}
	else
	{
		return new;
	}
}

/**
 * @brief  Funkce pro prevedeni retezce na cislo
 * @param  input  Retezec
 * @return  int  Cislo
 *          <0  Pri chybe.
 */
int convertToNumber(char* input)
{
	char* rest = "";
	long int result = strtol(input, &rest, 10);

	if (rest[0] == '\0')
	{
		if (result <= INT_MAX)
		{
			return (int) result;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

/**
 * @brief  Funkce pro prevedeni retezce na desetinne cislo
 * @param  input  Retezec
 * @return  double  Cislo
 *          <0  Pri chybe.
 */
double convertToDouble(char* input)
{
	char* rest;
	double result = strtod(input, &rest);

	if (rest[0] == '\0')
	{
		int i = 0;
		bool isDot = false;

		// Test na to, jestli nahodou neni po znaku "." hned "E" nebo "e"
		while (input[i] != '\0')
		{
			if (isDot && (input[i] == 'e' || input[i] == 'E'))
			{
				return -3;
			}
			else if (input[i] == '.')
			{
				isDot = true;
			}
			else
			{
				isDot = false;
			}
			i++;
		}
		if (isDot)
		{
			return -3;
		}

		return result;
	}
	else
	{
		return -1;
	}
}

/**
 * @brief  Funkce pro nacteni retezce
 * @return  char*  Nacteny retezec
 * @return  NULL  Pri chybe
 */
char* readString()
{
	int cap = 50;
	int size = 0;

    char c;
    char* s = (char *) malloc(cap*sizeof(char));
    if (s == NULL)
    	return NULL;

	while (s != NULL && (c = getchar()) != EOF && c != '\n')
	{
    	if (size == (cap-1))
    	{
    		cap = 2*cap;
    		if ((s = tryRealloc(s, cap)) == NULL)
    			return NULL;
    	}

    	s[size] = c;
    	size++;
	}
	
	s[size] = '\0';
	return s;
}

/**
 * @brief  Funkce pro nacteni celeho cisla
 * @return  int  Nactene cislo
 * @return  <0  Pri chybe
 */
int readInt()
{
	char *s = readString();
	if (s == NULL)		// nastala chyba pri (re)alokaci
	{
		return -2;
	}
	else if (!isdigit(s[0]))	// zaporna cisla se neuvazuji
	{
		free(s);
		s = NULL;
		return -1;
	}
	else
	{
		int i = convertToNumber(s);
		free(s);
		s = NULL;
		return i;
	}
}

/**
 * @brief  Funkce pro nacteni desetinneho cisla
 * @return  double  Nactene cislo
 * @return  <0  Pri chybe
 */
double readDouble()
{
	char *s = readString();
	if (s == NULL)			// chyba pri realokaci
	{
		return -2;
	}
	else if (!isdigit(s[0]))	// zaporne cislo nebo zacina teckou
	{
		free(s);
		s = NULL;
		return -1;
	}
	else
	{
		double d = convertToDouble(s);
		free(s);
		s = NULL;
		return d;
	}
}

/**
 * @brief  Pomocna funkce pro prevedeni retezce na tvar, se kterym se lepe pracuje.
 * @param  str  Retezec pro prevedeni
 * @return  char*  Prevedeny retezec
 *          NULL  Pri chybe
 */
char *convert_str(const char *str) {
    int len = strlen(str);
    char *new = malloc(len + 1);

    int pos = 0;  // pozice, na kterou ukladam v novem retezci
    for(int i = 0; i < len; i++) {
        if(str[i] == '\\') {
            if(isdigit(str[i+1]) && isdigit(str[i+2]) && isdigit(str[i+3])) {  // za backspace jsou tri cisla
                if((str[i+1] >= '0' && str[i+1] <= '3') && (str[i+2] >= '0' && str[i+2] <= '7') && (str[i+3] >= '0' && str[i+3] <= '7')) {  // je tam oktalove cislo
                    new[pos] = str[i+3]- '0' + 8*(str[i+2] - '0' + 8*(str[i+1] - '0'));
                    i+=3;
                } else {
                    return NULL;
                }
            } else {  // neni to oktalove cislo
                if(str[i+1] == 't') {  // mam tabulator
                    new[pos] = '\t';
                    i++;
                } else {
                    if(str[i+1] == 'n') {  // mam novy radek
                        new[pos] = '\n';
                        i++;
                    } else {
                        if(str[i+1] == '\\') {  // mam backslash
                            new[pos] = '\\';
                            i++;
                        } else {
                            if(str[i+1] == '\"') {  // mam uvozovku "
                                new[pos] = '\"';
                                i++;
                            } else {
                                return NULL;
                            }
                        }
                    }
                }
            }
        } else {
            new[pos] = str[i];
        }
        pos++;
    }
    new[pos]=0;  // konec retezce, usekne uvozovku na konci
    
    return new;
}

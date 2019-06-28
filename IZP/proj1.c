/**
 * Soubor:  proj1.c
 * Datum:   21.10.2015 19:49
 * Autor:   Štěpán Vích, xvichs00@vutbr.cz
 * Projekt: Práce s textem, projekt č. 1 pro předmět IZP
 * Popis:   Program zpracovává slova ze strandartního vstupu a obsahuje funkce pro detekci čísla, kalendářního data, prvočísla a palindromu.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

/**
 * Vlastní metoda pro výpočet délky řetězce.
 * Nahrazuje funkci strlen z hnihovny string.h, kterou nemůžeme v projektu použít.
 */
int strlenCustom(char str[])
{
	int length = 0;
	while (str[length] != '\0')
	{
		length++;
	}
	return length;
}

/**
 * Vrací true, pokud je každy znak v řetězci číslo 0-9.
 */
bool isNumber(char word[])
{
	int i = 0;
	while ( word[i] )
	{
		if ( !isdigit(word[i]) )
		{
			return false;
		}
		i++;
	}
	return true;
}

/**
 * Vrací true, pokud je řetězec převedený na číslo, pomocí funkce strtol, v rozsahu INT_MIN - INT_MAX.
 */
bool isInteger(char word[])
{
	char *endptr;
	long number;
	errno = 0;
	number = strtol(word, &endptr, 10);
	if ( (errno == ERANGE && (number < INT_MIN || number > INT_MAX) ) || (errno != 0 && number == 0) || (endptr == word) )
	{
		return false;
	}
	return true;
}

/**
 * Vrací true pokud je číslo prvočíslo.
 */
bool isPrime( int number )
{
	if( number <= 1 )
	{
		return false;
	}
	for (int i = 2; i < number; i++) {
		if (number % i == 0 && i != number) return false;
	}
	return true;
}

/**
 * Vrací true pokud je řetězec palindromem.
 */
bool isPalindrom( char str[] )
{
	int i = 0;
	int strLength = strlenCustom(str);
	while( str[i] )
	{
		if(str[i] != str[strLength - i - 1])
		{
			return false;
		}
		i++;
	}
	return true;
}

/**
 * Vrací true, pokud řetězec odpovídá formátu XXXX-XX-XX, kde X představuje číslovku 0-9.
 */
bool isDateFormat( char word[] )
{
	int i = 0;
	// Délka 9 znaků + nultý znak.
	if( strlenCustom(word) != 10 )
	{
		return false;
	}
	while(word[i])
	{
		if( i == 4 || i == 7 )
		{
			// 45 = '-' v ASCII tabulce.
			if( word[i] != 45 )
			{
				return false;
			}
		}
		else
		{
			if( !isdigit(word[i]) )
			{
				return false;
			}
		}

		i++;
	}

	return true;
}

int getWeekday(char date[])
{

	char yearStr[5] = {date[0], date[1], date[2],date[3], '\0'};
	char monthStr[3] = {date[5], date[6], '\0'};
	char dayStr[3] = {date[8], date[9], '\0'};

	int year = atoi(yearStr) - 1990;
	int month = atoi(monthStr) - 1;
	int day = atoi(dayStr);
	int returnValue;

	struct tm time;

	time.tm_year = year;
	time.tm_mon = month;
	time.tm_mday = day;
	time.tm_hour = 0;
	time.tm_min = 0;
	time.tm_sec = 1;
	time.tm_isdst = -1;

	returnValue = mktime(&time);

	if( day <= 0 || day > 31 || month < 0 || month > 11 )
	{
		return -1;
	}

	if( returnValue == -1 )
	{
		return -1;
	}
	else
	{
		return time.tm_wday;
	}

}

/**
 * Na stdin vytiskne nápovědu.
 */
int printHelp()
{
	printf( "Práce s textem - 1. projekt\n"
		"Autor: Štěpán Vích (c) 2015\n"
		"Program provádí jednoduché zpracování textu ze vstupu.\n"
		"Provádí detekci čísel, prvočísel, kalendářního datumu a detekci slov a palindromu.\n"
		"Data jsou načítána ze standardního vstupu.\n"
		"Formát datumu je YYYY-MM-DD\n"
		"Povolený rozsah hodnot pro měsíc je 01-12 a pro den v měsíci 01-31. \n"
		"Každé slovo má maximální délku 100 znaků, při překročení je rozděleno na více slov. \n"
		"Maximální hodnota čísla pro detekci prvočísla je %i \n"
		"Popis parametrů:\n"
		" Při zadání libovolných argumentů, během volání programu se vypíše tato nápověda.\n", INT_MAX);
	return 0;
}

/**
 * Na stdin vytiskne detekci slova.
 */
int printWord( char word[], bool isPalindrom )
{
	printf("word: %s", word);
	if(isPalindrom)
	{
		printf(" (palindrom)");
	}

	return 0;
}

/**
 * Na stdin vytiskne detekci čísla.
 */
int printNumber( char number[], bool isPrime )
{

	printf("number: %s", number);
	if( isPrime )
	{
		printf(" (prime)");
	}
	return 0;
}

/**
 * Na stdin vytiskne detekci datumu.
 */
int printDate( char date[], int weekday )
{
	char  *weekdayStr[] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	printf("date: %s %s", weekdayStr[weekday], date);
	return 0;
}

/**
 * Vrací true, pokud se v řetězci nevyskytují neočekávaná data, jinak vytiskne chybové hlášení.
 */
bool checkString( char input[] )
{

	bool isValid = true;

	for( int i = 0; input[i]; i++ )
	{
		int asciiValue = (int)input[i];
		// 0-32 = Bílé znaky v ASCII tabulce.
		// 127 = Znak DEL.
		if( asciiValue < 33  || asciiValue > 255 || asciiValue == 127 )
		{
			isValid = false;
		}

	}

	if( !isValid )
	{
		printf("Neocekavana vstupni data!");
	}

	return isValid;
}

/**
 * Nad řetězcem volá detekční funkce.
 * Po detekci volá print funkce.
 */
int parseString(char str[])
{
	// Zkontroluje neočekávaná vstupní data.
	if( !checkString(str) )
	{
		return 0;
	}

	bool    isWordFlag = false,
		isNumberFlag = false,
		isDateFlag = false,
		isPalindromFlag = false,
		isPrimeFlag = false;
	int weekday = 0;

	if( isNumber( str ) )
	{
		isNumberFlag = true;

		if( isPrime( atoi(str) ) && isInteger(str)  )
		{
			isPrimeFlag = true;
		}
	}
	else if( isDateFormat( str ) )
	{
		weekday = getWeekday(str);

        	if(weekday >=0 && weekday <= 6)
		{
			isDateFlag = true;
		}
		else
		{
			isWordFlag = true;
		}
	}
	else
	{
		isWordFlag = true;
		if( isPalindrom( str ) )
		{
			isPalindromFlag = true;
		}
	}

	if( isNumberFlag )
	{
		printNumber( str, isPrimeFlag );
	}

	if( isDateFlag )
	{
		printDate(str, weekday);
	}

	if( isWordFlag )
	{
		printWord( str, isPalindromFlag );
	}

	return 0;
}

/**
 * Hlavní program
 */
int main(int argc, char *argv[])
{
	// Pouze pro zrušení warningu kompilátoru.
	(void)argv;

	if(argc > 1)
	{
		printHelp();
		return 0;
	}
	else if(argc == 1)
	{
		char buffer[101];
		while( scanf("%100s", buffer) != EOF  )
		{
			parseString(buffer);
			printf("\n");
		}
	}

	return 0;
}


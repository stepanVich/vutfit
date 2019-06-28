#include "lexikalni-analyzator.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int errCode;
FILE * fp;

/**
 * @author			xvichs00
 * @brief			Hlavni funkce lexikalniho analyzatoru. Cte soubor na ktery,
 * 					ukazuje file descriptor, a vraci jeden token. Nezajistuje
 *					cteni souboru od pocatku.
 * @param 	create	Ciselna hodnota vyjadrujici, jestli se ma v tabulce symbolu hledat
 *					nebo vytvaret.
 * @param 	S 	 	Zasobnik ve kterem jsou ulozeny odkazy na koreny binarnich stromu.
 * @param 	data	Ukazatel do tabulky symbolu, ktery se naplni pozadovanou hodnotou.
 * @return	Token	Struktura tokenu, obsahujici typ, hodnotu tokenu a
 * 					radek na kterem se token nachazi.
 */
Token *get_token() {

  // Alokace mista pro token - tento se i bude vracet
  Token *tk = (Token *)malloc(sizeof(Token));
  if (tk == NULL) {
    errCode = 99; 
    return tk;
  };
  // Kontrola souboru
  if (fp == NULL) {
    tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
    return tk;
  }
  // Alokace mista pro token_value - nejprve 10B, pri prekroceni realloc
  tk->value = (char *)malloc(10 * sizeof(char));
  if (tk->value == NULL) {
    tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
  };
  // Nastaveni pocatecnich hodnot - v tomto stavu je token nastaven na konec
  // souboru
  tk->type = TOKEN_TYPE_EOF;
  errCode = 0;
  tk->value_length = 1; // 1 kvuli nulovemu znaku
  tk->value_capacity = 10;
  tk->value[0] = '\0';

  // Konecny automat
  // stav automatu - ulozen v celem cislu - jednotlive stavy vzestupne
  // ocislovany - viz hlavicka - moznost pouzit typ enum pro vetsi prehlednost
  int state = 0;
  // deklarace symbolu
  char symbol;
  // hlavni smycka
  while (!feof(fp)) {
    // Cteni znaku
    symbol = fgetc(fp);
    // Jednotlive stavy automatu
    // Nejvyssi uroven
    switch (state) {
    case 0:
      // Vyprazdnime hodnotu tokenu - je to nutne napr. kvuli komentarum, ktere
      // uchovavaji prvotni operator deleno v hodnote tokenu a s touto hodnotou
      // se vraci do pocatecniho stavu
      if (tk->value_length > 1) {
        tk->value_length = 1;
        tk->value[0] = '\0';
      }
      // Vetev cislice
      if (isdigit(symbol)) {
        // Nastavime novy stav, do ktereho jsme se dostali
        state = 1;
        // Jsme v koncovem stavu (int) -> nastavime typ tokenu na hodnotu
        // int
        tk->type = TOKEN_TYPE_INT;
	errCode = 0;
        // Pridame symbol do hodnoty tokenu
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

        // Vetev identifikatoru
      } else if (isalpha(symbol) || symbol == '_' || symbol == '$') {
        state = 7;
        tk->type = TOKEN_TYPE_IDENTIFIKATOR;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

        // Vetev retezce
      } else if (symbol == '"') {
        state = 10;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        //if (!add_symbol(tk, symbol)) {
        //  tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
        //  return tk;
        //}

        // Vetev komentare
      } else if (symbol == '/') {
        state = 15;
        tk->type = TOKEN_TYPE_OPERATOR;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

        // Vetev operatoru
      } else if (symbol == '<') {
        state = 19;
        tk->type = TOKEN_TYPE_OPERATOR;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else if (symbol == '=') {
        state = 19;
        tk->type = TOKEN_TYPE_OPERATOR;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else if (symbol == '>') {
        state = 19;
        tk->type = TOKEN_TYPE_OPERATOR;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else if (symbol == '!') {
        state = 19;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

        // Stavy operatoru, ktere nemaji moznost se dal vetvit
      } else if (symbol == ';' || symbol == ',' || symbol == '(' ||
                 symbol == ')' || symbol == '{' || symbol == '}' ||
                 symbol == '+' || symbol == '-' || symbol == '*') {
        // Nastav token a primo vrat pomoci return
        state = 0;
        tk->type = TOKEN_TYPE_OPERATOR; // typ operator
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

        // Natvrdo vrat
	errCode = 0;
        return tk;
        // Bile znaky - bez komentaru
      } else if (symbol == 32 || symbol == '\n' || symbol == '\t') {
        // Iterace nad bilymi znaky - pokud narazis na bily znak, vrat se na
        // zacatek
        // Bile znaky nemeni typ operatoru
        state = 0;
        // Znak EOF
      } else if (symbol == EOF) {
        tk->type = TOKEN_TYPE_EOF;
        // natvrdo vrat
	errCode = 0;
        return tk;
        // Pokud jakykoliv jiny neznamy znak -> chyba v lexikalni analyze
      } else {
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        return tk;
      }
      break;
    // Blok identifikace cisel
    case 1:
      if (isdigit(symbol)) {
        state = 1;
        tk->type = TOKEN_TYPE_INT;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else if (symbol == '.') {
        state = 2;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else if (symbol == 'e' || symbol == 'E') {
        state = 4;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 2:
      if (isdigit(symbol)) {
        state = 3;
        tk->type = TOKEN_TYPE_DOUBLE;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 3:
      if (isdigit(symbol)) {
        state = 3;
        tk->type = TOKEN_TYPE_DOUBLE;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else if (symbol == 'e' || symbol == 'E') {
        state = 4;
        tk->type = TOKEN_TYPE_DOUBLE;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 4:
      if (isdigit(symbol)) {
        state = 6;
        tk->type = TOKEN_TYPE_DOUBLE;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else if (symbol == '+' || symbol == '-') {
        state = 5;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 5:
      if (isdigit(symbol)) {
        state = 6;
        tk->type = TOKEN_TYPE_DOUBLE;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 6:
      if (isdigit(symbol)) {
        state = 6;
        tk->type = TOKEN_TYPE_DOUBLE;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    // Konec bloku identifikace cisel

    // Blok identifikace identifikatoru
    case 7:
      if (isalpha(symbol) || isdigit(symbol) || symbol == '_' ||
          symbol == '$') {
        state = 7;
        tk->type = TOKEN_TYPE_IDENTIFIKATOR;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }
      } else if (symbol == '.') {
        state = 8;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        // Kontrola zda hodnota predtim nebyla klicove slove, pokud ano chyba v
        // lexikalni analyze
        if (is_keyword(tk->value)) {
          ungetc(symbol, fp);
          return tk;
        }
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        // Kontrola zda je identifikator, klicovym slovem
        if (is_keyword(tk->value)) {
          tk->type = TOKEN_TYPE_KEYWORD;
	  errCode = 0;
        }
        return tk;
      }
      break;
    case 8:
      if (isalpha(symbol) || symbol == '_' || symbol == '$') {
        state = 9;
        tk->type = TOKEN_TYPE_IDENTIFIKATOR_SLOZENY;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 9:
      if (isalpha(symbol) || isdigit(symbol) || symbol == '_' ||
          symbol == '$') {
        state = 9;
        tk->type = TOKEN_TYPE_IDENTIFIKATOR_SLOZENY;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        if (contain_keyword(tk->value)) {
          tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        }
        ungetc(symbol, fp);
        return tk;
      }
      break;
    // Konec bloku identifikace identifikatoru

    // Blok identifikace retezce
    case 10:
      if (symbol != '"' && symbol != '\\' && symbol != '\n') {
        state = 10;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else if (symbol == '"') {
        state = 11;
        tk->type = TOKEN_TYPE_STRING;
	errCode = 0;
        //if (!add_symbol(tk, symbol)) {
        //  tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
        //  return tk;
        //}

      } else if (symbol == '\\') {
        state = 12;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 11:
      // Neni zde zadna vetev -> vynechame podminky
      ungetc(symbol, fp);
      return tk;
      break;
    case 12:
      // 1..3 -> 49..51 v ASCII tabulce
      if (symbol >= 49 && symbol <= 51) {
        state = 13;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }
      // 0 -> 48 v ASCII tabulce
      } else if (symbol == 48) {
        state = 21;
	tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }
      } else if (symbol == '\\' || symbol == '"' || symbol == 'n' ||
                 symbol == 't') {
        state = 10;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 13:
      // 0..7 -> 48..55 v ASCII tabulce
      if (symbol >= 48 && symbol <= 55) {
        state = 14;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 14:
      // 0..7 -> 48..55 v ACSII tabulce
      if (symbol >= 48 && symbol <= 55) {
        state = 10;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 21:
      // 1..7 -> 49..55 v ASCII tabulce
      if (symbol >= 49 && symbol <= 55) {
        state = 22;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }
      } else if (symbol == 48) {
        state = 23;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }
      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 22:
      // 0..7 -> 48..55 v ASCII tabulce
      if (symbol >= 48 && symbol <= 55) {
        state = 10;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }
      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 23:
      // 1..7 -> 49..55 v ASCII tabulce
      if (symbol >= 49 && symbol <= 55) {
        state = 10;
        tk->type = TOKEN_TYPE_ERROR; errCode = 1;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }
      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    // Konec identifikace retezce

    // Zacatek identifikace komentare
    // Komentare nikdy neukladame, ani nevracime jako token, jen preskakujeme ->
    // add_symbol zde nepotrebujeme
    // Jejich hodnota je EOF - pokud nejsou u konce souboru, jejich typ se
    // prepise, pokud jsou u konce souboru, vrati se token typu EOF
    case 15:
      if (symbol == '/') {
        state = 16;
        tk->type = TOKEN_TYPE_EOF;
	errCode = 0;
      } else if (symbol == '*') {
        state = 17;
        tk->type = TOKEN_TYPE_EOF;
	errCode = 0;
      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
    case 16:
      if (symbol != '\n') {
        state = 16;
      } else {
        state = 0;
      }
      break;
    case 17:
      if (symbol != '*') {
        state = 17;
      } else {
        state = 18;
      }
      break;
    case 18:
      if (symbol == '/') {
        state = 0;
      } else {
        state = 17;
      }
      break;
    // Konec identifikace komentaru

    // Zacatek identifikace operatoru, skladajicich se ze dvou casti
    case 19:
      if (symbol == '=') {
        state = 20;
        tk->type = TOKEN_TYPE_OPERATOR;
	errCode = 0;
        if (!add_symbol(tk, symbol)) {
          tk->type = TOKEN_TYPE_SYSTEM_ERROR; errCode = 99;
          return tk;
        }

        return tk;
      } else {
        ungetc(symbol, fp);
        return tk;
      }
      break;
      // Konec identifikace operatoru, skladajiciho se ze dvou casti
    } // Konec vyctu stavu
  }   // Konec smycky
  
  if (tk->type != TOKEN_TYPE_ERROR && tk->type != TOKEN_TYPE_SYSTEM_ERROR) 
    errCode = 0;

  // Kdyz je token EOF, vymaz hodnotu value
  if (tk->type == TOKEN_TYPE_EOF) {
    tk->value_length = 1;
    tk->value[0] = '\0';
    errCode = 0;
  }

  // Finalni navraceni tokenu
  return tk;
}

/**
 * @author	xvichs00
 * @brief	Pridava jeden znak na konec hodnoty value, struktury Token.
 * Podradnou cinnosti je realokace pameti, pokud je retezec prilis velky.
 * @param 	tk	Ukazatel na strukturu Tokenu, ve ktere upravujeme
 * vlastnost value.
 * @param 	symbol	Znak, ktery pridavame na konec vlastnosti value.
 * @return	int	1 pokud realokace probehne dobre, 0  pokud probehne
 * spatne.
 */
int add_symbol(Token *tk, char symbol) {
  // Realokace pokud je potreba - nezapominat na nulovy znak
  if (tk->value_length >= tk->value_capacity) {
    tk->value_capacity *= 2;
    tk->value = (char *)realloc(tk->value, (tk->value_capacity) * sizeof(char));
    if (tk->value == NULL) {
      // Chyba v realokaci pameti
      return 0;
    }
  }

  // Nakonec znak pridame
  tk->value[tk->value_length - 1] = symbol;
  tk->value[tk->value_length] = '\0';
  // Inkrementace velikosti retezce
  tk->value_length++;
  // Vse probehlo v poradku
  return 1;
}

/**
 * @author	xvichs00
 * @brief	Kontroluje zda se retezec, nerovna nekteremu z klicovych slov.
 * @param	string	Retezec, se kterym se porovnavaji klicova slova.
 * @return	int	1 pokud se najde shoda s klicovym slovem, 0 pokud ne.
 */
int is_keyword(char *string) {
  // Vycet klicovych slov
  char *keywords[] = {"boolean", "break",  "class",  "continue", "do",
                      "double",  "else",   "false",  "for",      "if",
                      "int",     "return", "String", "static",   "true",
                      "void",    "while"};
  // Rozdeleni na vice slov podle .
  for (int i = 0; i < 17; i++) {
    if (!strcmp(keywords[i], string)) {
      // Nasla se shoda, vrat true
      return 1;
    }
  }

  // Nenasla se shoda, vrat false
  return 0;
}

/**
 * @author	xvichs00
 * @brief	Zjistuje zda druha cast plne kvalifikovaneho identifikatoru neni
 * klicovym slovem.
 * @param	string	Ukazatel na retezec. Nejcasteji tk->value.
 * @return	int	1 pokud se najde shoda s klicovym slovem v alespon jedne
 * casti identifikatoru, jinak 0.
 */
int contain_keyword(char *string) {
  // Pomocne promenne
  char *trim_string;
  // strchr ukazuje na pozici prvni tecky -> posuneme ukazatel o jeden znak
  // vpred
  trim_string = strchr(string, '.') + 1;
  if (is_keyword(trim_string)) {
    // Nalezli jsme shodu
    return 1;
  }
  // Zadna shoda se nenasla
  return 0;
}

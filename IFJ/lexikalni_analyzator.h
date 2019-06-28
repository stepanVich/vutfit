/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * lexikalni_analyzator.h
 * 
 * Stepan Vich,     xvichs00
 */


#ifndef LEXIKALNI_ANALYZATOR_H
#define LEXIKALNI_ANALYZATOR_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"

// Struktury

typedef struct {
  // Typ tokenu - napr. 1->int, 3->double, ...
  int type;
  // Dynamicky alokovany retezec, ukladajici hodnotu tokenu
  char *value;
  // Pomocna promenna urcujici delku retezce value
  int value_length;
  // Pomocna promenna udavajici velikost naalokovane pameti
  int value_capacity;
  // Zatim neimplementovano - bude urcovat na jakem radku se token vyskytuje
  int line;
} Token;
typedef Token *TokenPtr;

// Soubor s interpretovanym kodem
extern FILE * fp;

// Funkce

Token *get_token();
int add_symbol(Token *tk, char symbol);
int is_keyword(char * string);
int contain_keyword(char * string);

// Enum
enum {
  TOKEN_TYPE_ERROR,
  TOKEN_TYPE_EOF,
  TOKEN_TYPE_INT,
  TOKEN_TYPE_DOUBLE,
  TOKEN_TYPE_IDENTIFIKATOR,
  TOKEN_TYPE_IDENTIFIKATOR_SLOZENY,
  TOKEN_TYPE_STRING,
  TOKEN_TYPE_OPERATOR,
  TOKEN_TYPE_KEYWORD,
  TOKEN_TYPE_SYSTEM_ERROR
};

enum {
  STATE_BEGIN,
  STATE_OPERATORS
  // ...
};

#endif

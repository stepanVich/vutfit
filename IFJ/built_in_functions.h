/*
 * Implementace interpretu imperativniho jazyka IFJ16
 * built_in_functions.h
 * 
 * Petr Mohelnik,       xmohel02
 */


#ifndef BUILTIN_FUNCTIONS
#define BUILTIN_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h> // nutne kvuli funkci isdigit()


int length(const char *str);
int compare(const char *str1, const char *str2);
char *substr(const char *str, int i, int n);
char* tryRealloc(char* s, int cap);
int convertToNumber(char* input);
double convertToDouble(char* input);
char* readString();
int readInt();
double readDouble();
char *convert_str(const char *str);


#endif




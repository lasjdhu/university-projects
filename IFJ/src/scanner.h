/**
 * @file scanner.h
 * Project - IFJ Project 2024
 *
 * @author Dias Tursynbayev xtursyd00
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief Scanner implementation header
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "error.h"
#include "jm_string.h"
#include "memory.h"
#include <ctype.h>
#include <stdio.h>

typedef enum TokenType {
  T_EOF = 0,   // end of file
  T_DOT,       // .
  T_SEMICOLON, // ;
  T_COLON,     // :
  T_COMMA,     // ,
  T_OPAREN,    // (
  T_CPAREN,    // )
  T_OSQUARE,   // [
  T_CSQUARE,   // ]
  T_OCURLY,    // {
  T_CCURLY,    // }
  T_PIPE,      // |
  T_AT,        // @
  T_QUESTM,    // ?
  T_PLUS,      // +
  T_SUB,       // -
  T_MUL,       // *
  T_DIV,       // /
  T_STR,       // ""

  T_LT,     // <
  T_GT,     // >
  T_ASSIGN, // =
  T_NEQ,    // !=

  T_LEQ,   // <=
  T_GEQ,   // >=
  T_EQ,    // ==
  T_INT,   // integer
  T_FLOAT, // float
  T_ID,    // identifier
  T_UNDERSCORE // _
} TokenType;

typedef enum State {
  S_ERR = -1,
  S_START,
  S_LT,
  S_GT,
  S_EQ,
  S_NEQ,
  S_DIV,
  S_INT,
  S_FLOAT,
  S_EXPS,
  S_EXP,
  S_STR,
  S_MLSTR,
  S_ID,
  S_UNDERSCORE
} State;

typedef enum Keyword {
  K_UNKNOWN = -1,
  K_CONST = 0,
  K_ELSE,
  K_FN,
  K_IF,
  K_I32,
  K_F64,
  K_NULL,
  K_PUB,
  K_RETURN,
  K_U8,
  K_VAR,
  K_VOID,
  K_WHILE,
  K_IFJ,
} Keyword;

typedef union TokenValue {
  String *string;
  int integer;
  double real;
} TokenValue;

typedef struct Token {
  TokenType type;
  TokenValue value;
  Keyword keyword;
} Token;

/**
 * Scanner structure that will contain array of tokens
 * Which will be used in parser
 */
typedef struct Scanner {
  size_t size;
  size_t capacity;
  size_t current_token; //Current index in token array
  Token **tokens;
} Scanner;

extern Scanner *scanner;

void ScannerInit(FILE *f);

Keyword GetKeyword(String *str);
char *GetEscapeSequence();
int GetToken(Token *token);
void ScannerDest();
void GenerateTokens();

void PrintToken(const char *key, Token *token);
char *KeywordToString(Keyword keyword);

#endif // SCANNER_H

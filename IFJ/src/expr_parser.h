/**
 * @file expr_parser.h
 * Project - IFJ Project 2024
 *
 * @author Albert Tikaiev xtikaia00
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief Header for expression parser
 */

#ifndef EXPR_PARSER_H
#define EXPR_PARSER_H

#include "ASTnodes.h"
#include "memory.h"
#include "parser.h"
#include "scanner.h"
#include "symtable.h"
#include "stack.h"
#include <stdio.h>

//If expression has only literals or constants
extern bool isknown_return;

typedef enum {
  O_MUL = 0,
  O_PLUS,
  O_ID,
  O_RELATIONAL,
  O_OPENPAREN,
  O_CLOSEPAREN,
  O_DOLLAR
} precedenceType;

typedef struct expr_item {
  precedenceType type;
  bool isFunction; //For code generator, which will decide what it has to use from union
  bool i2f; //For code generator, which will cast i32 to f64
  union {
    Token *token;
    ASTFuncCall *funcCall;
  } data;
} expr_data;

typedef struct expr {
  bool isTerminal;
  bool isLess;
  bool isLiteral;
  bool isKnownConstant;
  DataType returnType; //Type of current expr item
  expr_data *item;
} expr;

expr* GetTopTerminal(stack* pushdown);
void MakeTopTerminalLess(stack *pushdown);
precedenceType GetType(bool additional_parenthesis, int *count_parentheses, bool additional_comma);
expr *CreateExprItem(bool additional_parenthesis, int *count_parentheses, bool additional_comma);
expr* shift(expr *right, stack* pushdown);
void Reduce(stack* pushdown, stack* infix);
DataType PrecedeneParseExpression(stack **output_stack, bool parenthesis, bool comma);

#endif

/**
 * @file symtable.h
 * Project - IFJ Project 2024
 *
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief symbolic table functions header,
 * variant of hashtable with open addressing
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "array.h"
#include "error.h"
#include "memory.h"
#include "stack.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 16383 // 2^14 - 1
#define C 1            // Linear probing step size

typedef enum {
  ST_FUNC,
  ST_VAR_VAR,
  ST_VAR_CONST,
} SymbolType;

typedef enum {
  ST_NOT_DEFINED = -1,
  ST_VOID,
  ST_I32,
  ST_F64,
  ST_U8,
  ST_NULL,
  ST_NULLI32, //?I32
  ST_NULLF64,
  ST_NULLU8,
  ST_STRING,
  ST_BOOL
} DataType;

typedef struct var_data {
  DataType type;
  int scope;
  bool isUsed;
  bool isModificationUsed;
  bool isKnown;
} var_data;

typedef struct Param {
  char *name;
  DataType type;
} Param;

typedef struct func_data {
  DataType returnType;
  List *params;
  List *variables;
  bool returnMade;
} func_data;

typedef struct {
  SymbolType symType;
  char *name;
  union {
    var_data var;
    func_data func;
  } details;
} symtable_item_data;

typedef struct {
  char *key;
  symtable_item_data *data;
  bool busy;
} symtable_item;

typedef symtable_item *symtable[MAX_SIZE];

unsigned int LinearProbe(int index, int step);
void SymtableInit();
symtable_item_data *SymtableAdd(char *key, SymbolType type);
void SymtableRemove(char *key);
void SymtableClear();
symtable_item_data *SymtableFind(char *key);
unsigned int SymtableHash(char *key);
void SymtableEnterScope();
void SymtableLeaveScope();
symtable_item_data *SymtableAssertFunction(char *key);
symtable_item_data *SymtableAssertVariable(char *key);
symtable_item_data *SymtableUpdate_isUsed(char *key);
symtable_item_data *SymtableUpdate_isModified(char *key);
void SymtableAssertReturns();

#endif // SYMTABLE_H

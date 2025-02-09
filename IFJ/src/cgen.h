/**
 * @file cgen.h
 * Project - IFJ Project 2024
 *
 * @author Danil Tasmassys xtasmad00
 *
 * @brief Header for assembly code generator
 */

#ifndef CGEN_H
#define CGEN_H

#include "ASTnodes.h"
#include "error.h"
#include "expr_parser.h"
#include "memory.h"
#include "scanner.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include <stdbool.h>


void CgenStart(ASTStart *code);

void CgenParam(ASTParam *param);

void CgenFuncDecl(ASTFuncDecl *func);

void CgenExpr(ASTExpression *expr, bool callfromfunc);

void CgenReturnStat(ASTReturn *ret);

void CgenBody(ASTBody *body);

void CgenFuncCall(ASTFuncCall *fcall, bool isdef, bool callfromfunc);

void CgenVarDeclDef(ASTVarDeclDef *decloh);

void CgenIf(ASTIfStatement *ifelse);

void CgenWhile(ASTWhileStatement *cyklus);

#endif

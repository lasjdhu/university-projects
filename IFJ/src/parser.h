/**
 * @file parser.h
 * Project - IFJ Project 2024
 *
 * @author Albert Tikaiev xtikaia00
 * @author Danil Tasmassys xtasmad00
 *
 * @brief Parser header file
 */

#ifndef PARSER_H
#define PARSER_H

#include "ASTnodes.h"
#include "cgen.h"
#include "expr_parser.h"
#include "jm_string.h"
#include "scanner.h"
#include "symtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void GetNextToken();
void FirstPassSymtable();

void Parse();
void ParseProlog();
void ParseFuncDecl(ASTFuncDecl **func_decl);
void ParseFuncDeclNext(ASTFuncDecl **func_decl);

void ParseFuncParams(ASTParam **params);
void ParseParam(ASTParam **params);
void ParseParamNext(ASTParam **params);

void ParseBody(ASTBody **body, bool already_entered);
void ParseStatement(ASTStatement **statement);
void ParseStatementNext(ASTStatement **statement);

void ParseIfStatement(ASTIfStatement **statement);
void ParseElseStatement(ASTIfStatement **statement);
void ParseWhileStatement(ASTWhileStatement **statement);
char* ParseNotNull(DataType nullable_type);

void ParseVarDecl(SymbolType const_type, ASTVarDeclDef *var_decl);
DataType ParseVarDef(ASTVarDeclDef *var_decl);
void ParseReturn(ASTReturn *statement_return);

DataType ParseExpr(ASTExpression *expression, bool additional_parenthesis, bool additional_comma);

DataType ParseFuncCall(ASTFuncCall *func_call);
DataType ParseCallParam(ASTParamCall **func_call_param, int* counter_args);

DataType ParseType();

bool CheckNullableTypes(DataType base, DataType from_expression);

#endif

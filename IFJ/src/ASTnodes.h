/**
 * @file ASTnodes.h
 * Project - IFJ Project 2024
 *
 * @author Danil Tasmassys xtasmad00
 * @author Albert Tikaiev xtikaia00
 *
 * @brief Header file for AST nodes definition
 */
#ifndef ASTNODES_H
#define ASTNODES_H

#include "memory.h"
#include "stack.h"
#include "symtable.h"

typedef enum StatementType {
  T_FCALL = 0,
  T_VARDECLDEF,
  T_IF,
  T_WHILE,
  T_RETURN
} StatementType;

typedef struct ASTStart ASTStart;
typedef struct ASTFuncDecl ASTFuncDecl;
typedef struct ASTParam ASTParam;
typedef struct ASTBody ASTBody;
typedef struct ASTStatement ASTStatement;
typedef struct ASTFuncCall ASTFuncCall;
typedef struct ASTParamCall ASTParamCall;
typedef struct ASTVarDeclDef ASTVarDeclDef;
typedef struct ASTExpression ASTExpression;
typedef struct ASTIfStatement ASTIfStatement;
typedef struct ASTWhileStatement ASTWhileStatement;
typedef struct ASTReturn ASTReturn;

/**
 * @brief Main AST node that will have first function declaration
 */
typedef struct ASTStart {
  ASTFuncDecl *funcdecls;
} ASTStart;

/**
 * @brief AST node with function declaration information
 */
typedef struct ASTFuncDecl {
  char *name;       /**< Name of function*/
  ASTParam *params; /**< Function parameters*/
  List *variables;  /**< Dynamic array of all variables in function*/
  ASTBody *body;    /**< All function statements are here*/
  ASTFuncDecl *next;/**< Next function declaration*/
} ASTFuncDecl;

/**
 * @brief AST node with function parameter information
 */
typedef struct ASTParam {
  char *name;     /**< Name of function parameter*/
  ASTParam *next; /**< Next function parameter*/
} ASTParam;

/**
 * @brief AST node for body of function or if/while/else statement
 */
typedef struct ASTBody {
  ASTStatement *statement; /**< First statement of body*/
} ASTBody;

/**
 * @brief AST node for general statement
 */
typedef struct ASTStatement {
  StatementType type; /**< Type of statement*/
  void *statement;    /**< Pointer to specific AST node statement*/
  ASTStatement *next; /**< Pointer to next AST node statement*/
} ASTStatement;

/**
 * @brief AST node for function call statement
 */
typedef struct ASTFuncCall {
  char *name;           /**< Name of function that is calling. Can be ifj.chr, etc..*/
  ASTParamCall *params; /**< First parameter of function call*/
} ASTFuncCall;

/**
 * @brief AST node for function call parameter
 */
typedef struct ASTParamCall {
  ASTExpression *expr;  /**< Expression of this parameter*/
  ASTParamCall *next;   /**< Next parameter or NULL*/
} ASTParamCall;

/**
 * @brief AST node for variable declaration/redefinition statement
 */
typedef struct ASTVarDeclDef {
  bool isIgnoring;    /**< if its _ = ... statement*/
  char *name;         /**< Name of variable*/
  ASTExpression *expr;/**< Expression for declaration/redefinition */
} ASTVarDeclDef;

/**
 * @brief AST node for expression
 * exprStack is in postfix notation for code generator
 */
typedef struct ASTExpression {
  stack *exprStack;     /**< Stack of expr struct items from expression parser */
  DataType returnType;  /**< Data type of expression */
} ASTExpression;

/**
 * @brief AST node for if statement
 */
typedef struct ASTIfStatement {
  ASTExpression *expr;  /**< boolean/nullable expression */
  char* notNullID;      /**< Name of variable from nullable part */
  ASTBody *ifBody;      /**< Body of if part */
  ASTBody *elseBody;    /**< Body of else part */
} ASTIfStatement;

/**
 * @brief AST node for while statement
 */
typedef struct ASTWhileStatement {
  ASTExpression *expr;  /**< boolean/nullable expression */
  char* notNullID;      /**< Name of variable from nullable part */
  ASTBody *whileBody;   /**< Body of while */
} ASTWhileStatement;

/**
 * @brief AST node for return statement
 */
typedef struct ASTReturn {
  ASTExpression *expr; /**< Expression or NULL */
} ASTReturn;

//Creating nodes
ASTStart *CreateASTStart();
ASTFuncDecl *CreateASTFuncDecl();
ASTParam *CreateASTParam();
ASTBody *CreateASTBody();
ASTStatement *CreateASTStatement();
ASTFuncCall *CreateASTFuncCall();
ASTParamCall *CreateASTParamCall();
ASTVarDeclDef *CreateASTVarDeclDef();
ASTExpression *CreateASTExpression();
ASTIfStatement *CreateASTIfStatement();
ASTWhileStatement *CreateASTWhileStatement();
ASTReturn *CreateASTReturn();

#endif

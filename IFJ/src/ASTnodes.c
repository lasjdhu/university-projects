/**
 * @file ASTnodes.c
 * Project - IFJ Project 2024
 *
 * @author Danil Tasmassys xtasmad00
 * @author Albert Tikaiev xtikaia00
 *
 * @brief Source file for AST nodes creating
 * Description of all nodes can be found in header file
 */

#include "ASTnodes.h"

ASTStart *CreateASTStart() {
  ASTStart *ast_start = InvokeAlloc(sizeof(ASTStart));
  ast_start->funcdecls = NULL;
  return ast_start;
}

ASTFuncDecl *CreateASTFuncDecl() {
  ASTFuncDecl *ast_func_decl = InvokeAlloc(sizeof(ASTFuncDecl));
  ast_func_decl->next = NULL;
  ast_func_decl->variables = NULL;
  ast_func_decl->params = NULL;
  ast_func_decl->body = NULL;
  return ast_func_decl;
}

ASTParam *CreateASTParam() {
  ASTParam *ast_param = InvokeAlloc(sizeof(ASTParam));
  ast_param->next = NULL;
  return ast_param;
}

ASTBody *CreateASTBody() {
  ASTBody *ast_body = InvokeAlloc(sizeof(ASTBody));
  ast_body->statement = NULL;
  return ast_body;
}

ASTStatement *CreateASTStatement() {
  ASTStatement *ast_statement = InvokeAlloc(sizeof(ASTStatement));
  ast_statement->statement = NULL;
  ast_statement->next = NULL;
  return ast_statement;
}

ASTFuncCall *CreateASTFuncCall() {
  ASTFuncCall *ast_func_call = InvokeAlloc(sizeof(ASTFuncCall));
  ast_func_call->params = NULL;
  return ast_func_call;
}

ASTParamCall *CreateASTParamCall() {
  ASTParamCall *ast_param_call = InvokeAlloc(sizeof(ASTParamCall));
  ast_param_call->next = NULL;
  ast_param_call->expr = NULL;
  return ast_param_call;
}

ASTVarDeclDef *CreateASTVarDeclDef() {
  ASTVarDeclDef *ast_var_decl_def = InvokeAlloc(sizeof(ASTVarDeclDef));
  ast_var_decl_def->isIgnoring = false;
  ast_var_decl_def->name = NULL;
  ast_var_decl_def->expr = NULL;
  return ast_var_decl_def;
}

ASTExpression *CreateASTExpression() {
  ASTExpression *ast_expression = InvokeAlloc(sizeof(ASTExpression));
  ast_expression->exprStack = NULL;
  ast_expression->returnType = ST_NOT_DEFINED;
  return ast_expression;
}

ASTIfStatement *CreateASTIfStatement() {
  ASTIfStatement *ast_if_statement = InvokeAlloc(sizeof(ASTIfStatement));
  ast_if_statement->elseBody = NULL;
  ast_if_statement->expr = NULL;
  ast_if_statement->ifBody = NULL;
  ast_if_statement->notNullID = NULL;
  return ast_if_statement;
}

ASTWhileStatement *CreateASTWhileStatement() {
  ASTWhileStatement *ast_while_statement = InvokeAlloc(sizeof(ASTWhileStatement));
  ast_while_statement->expr = NULL;
  ast_while_statement->notNullID = NULL;
  ast_while_statement->whileBody = NULL;
  return ast_while_statement;
}

ASTReturn *CreateASTReturn() {
  ASTReturn *ast_return = InvokeAlloc(sizeof(ASTReturn));
  ast_return->expr = NULL;
  return ast_return;
}
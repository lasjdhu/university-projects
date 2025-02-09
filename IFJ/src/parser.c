/**
 * @file parser.c
 * Project - IFJ Project 2024
 *
 * @author Albert Tikaiev xtikaia00
 * @author Danil Tasmassys xtasmad00
 *
 * @brief Parser source file
 */

#include "parser.h"
#include "error.h"
#include "memory.h"
#include "symtable.h"

// Current token
Token token;
// Root node of AST
ASTStart *ast_start;
// If it is first pass of parser
bool first_pass;
// Current symtable function item
symtable_item_data *current_func_sym;

/**
 * Get next token from token array
 */
void GetNextToken() {
  token = *scanner->tokens[scanner->current_token];
  scanner->current_token++;
}

/**
 * @brief Initialize symtable and start parsing
 *
 */
void Parse() {
  SymtableInit();
  // First pass parsing
  first_pass = true;
  FirstPassSymtable();
  scanner->current_token = 0;
  // Second pass parsing
  first_pass = false;
  ParseProlog();
  //Assert that all function had return(besides void function)
  SymtableAssertReturns();
  //AST is ready, calling code generator
  CgenStart(ast_start);
}

/**
 * @brief Fill symtable with function data
 * First pass of parser
 *
 * Tries to save all attributes of all functions to symtable
 */
void FirstPassSymtable() {
  scanner->current_token = 0;
  GetNextToken();
  // Skip prolog until first function
  while (token.keyword != K_PUB && token.type != T_EOF) {
    GetNextToken();
  }
  ParseFuncDecl(NULL);
  // Main function semantics
  symtable_item_data *main_func = SymtableAssertFunction("main");
  // Main function can only be return type of void
  if (main_func->details.func.returnType != ST_VOID)
    InvokeExit(PARAMETER_RETURN_ERROR);
  // Main function can't have any arguments
  if (main_func->details.func.params->size != 0)
    InvokeExit(PARAMETER_RETURN_ERROR);
}

/**
 * @brief Parsing START rule, parsing prolog
 *
 * <START> -> const ifj = @import("ifj24.zig"); <FIRST_FUNC_DECL>
 */
void ParseProlog() {
  // Get first token
  GetNextToken();
  // PROLOG
  // const ifj = @import("ifj24.zig");
  for (int i = 0; i < 9; i++) {
    switch (i) {
    case 0:
      if (token.type == T_ID && token.keyword == K_CONST)break;
      InvokeExit(SYNTAX_ERROR);break;
    case 1:
      if (token.type == T_ID && token.keyword == K_IFJ)break;
      InvokeExit(SYNTAX_ERROR);break;
    case 2:
      if (token.type == T_ASSIGN)break;
      InvokeExit(SYNTAX_ERROR);break;
    case 3:
      if (token.type== T_AT)break;
      InvokeExit(SYNTAX_ERROR);break;
    case 4:
      if (token.type == T_ID && StringEquals(token.value.string, "import")) break;
      InvokeExit(SYNTAX_ERROR);break;
    case 5:
      if (token.type == T_OPAREN) break;
      InvokeExit(SYNTAX_ERROR);break;
    case 6:
      if (token.type == T_STR && StringEquals(token.value.string, "ifj24.zig")) break;
      InvokeExit(SYNTAX_ERROR);break;
    case 7:
      if (token.type == T_CPAREN) break;
      InvokeExit(SYNTAX_ERROR);break;
    case 8:
      if (token.type == T_SEMICOLON) break;
      InvokeExit(SYNTAX_ERROR);break;
    default:
      break;
    }
    GetNextToken();
  }
  //<FUNC_DECL>
  ast_start = CreateASTStart();
  ParseFuncDecl(&ast_start->funcdecls);
}

/**
 * @brief Parsing function declaration
 *
 * <FUNC_DECL> -> pub fn <ID> (<FUNC_PARAMS>) <FUNC_TYPE> <BODY>
 * <FUNC_DECL_NEXT>
 */
void ParseFuncDecl(ASTFuncDecl **func_decl) {
  // pub fn
  if (token.type == T_ID && token.keyword == K_PUB)GetNextToken();
  else InvokeExit(SYNTAX_ERROR);
  if (token.type == T_ID && token.keyword == K_FN)GetNextToken();
  else InvokeExit(SYNTAX_ERROR);

  //<ID>
  if (token.type != T_ID || token.keyword != K_UNKNOWN) InvokeExit(SYNTAX_ERROR);

  /*
  FIRST PASS OF PARSER
  CREATE NEW FUNCTION ITEM FOR SYMTABLE
  */
  if (first_pass) {
    //Semantic check(no overloading in IFJ24 )
    if (SymtableFind(token.value.string->str) != NULL) InvokeExit(REDIFINE_ERROR);
    symtable_item_data *func_item = SymtableAdd(token.value.string->str, ST_FUNC);
    current_func_sym = func_item;
  }
  /*
  SECOND PASS OF PARSER
  CREATE NEW ITEM FOR AST
  */
  else {
    current_func_sym = SymtableFind(token.value.string->str);
    (*func_decl) = CreateASTFuncDecl();
    (*func_decl)->name = token.value.string->str;
  }
  GetNextToken();

  //(<FUNC_PARAMS>)
  if (token.type == T_OPAREN) {
    // consume '('
    GetNextToken();
    // FILL SYMTABLE
    if (first_pass)ParseFuncParams(NULL);
    // FILL AST
    else {
      //Entered scope with function parameters
      SymtableEnterScope();
      ParseFuncParams(&((*func_decl)->params));
    }
  }
  else InvokeExit(SYNTAX_ERROR);

  // consume ')'
  if (token.type == T_CPAREN)
    GetNextToken();
  else
    InvokeExit(SYNTAX_ERROR);

  //<FUNC_TYPE>
  if ((token.type == T_ID && token.keyword != K_UNKNOWN) || token.type == T_QUESTM || token.type == T_OSQUARE) {
    if (first_pass) {
      DataType return_function_type = ParseType();
      if(return_function_type == ST_NOT_DEFINED) InvokeExit(SYNTAX_ERROR);
      if(return_function_type == ST_VOID) current_func_sym->details.func.returnMade = true;
      current_func_sym->details.func.returnType = return_function_type;
    }
  }
  else InvokeExit(SYNTAX_ERROR);
  if(token.type == T_QUESTM) GetNextToken();
    if(token.type == T_OSQUARE) {
        GetNextToken();
        GetNextToken();
    }
  GetNextToken();

  // FIRST PASS OF PARSER
  // TRY TO FIND NEW FUNCTION ITEM FOR SYMTABLE
  if (first_pass) {
    while (true) {
      if (token.type == T_ID && token.keyword == K_PUB)
        break;
      if (token.type == T_EOF)
        break;
      GetNextToken();
    }
    ParseFuncDeclNext(NULL);
    return;
  }

  // SECOND PASS OF PARSER
  // CREATE NEW ITEM FOR AST
  // Current token must be '{'
  //<BODY>
  else ParseBody(&(*func_decl)->body, true);

  // All variables to be declared in function
  (*func_decl)->variables = current_func_sym->details.func.variables;

  //<FUNC_DECL>
  ParseFuncDeclNext(func_decl);
}

/**
 * @brief Try to parse next function declaration
 *
 *  <FUNC_DECL_NEXT> -> <FUNC_DECL> <FUNC_DECL_NEXT>    | if there is a function
 * after <FUNC_DECL_NEXT> -> EOL                             | if there is no
 * function
 */
void ParseFuncDeclNext(ASTFuncDecl **func_decl) {
  // NO NEXT FUNCTION -> stop parsing
  if (token.type == T_EOF)
    return;

  // FIRST PASS OF PARSER
  // CREATE NEW ITEM FOR SYMTABLE
  if (first_pass)
    ParseFuncDecl(NULL);

  // SECOND PASS OF PARSER
  // CREATE NEW ITEM FOR AST
  else
    ParseFuncDecl(&(*func_decl)->next);
}

/**
 * @brief Parse function declaration parameters
 *
 * <FUNC_PARAMS> -> <PARAM> <PARAMS>
 */
void ParseFuncParams(ASTParam **params) {
  //<FUNC_PARAMS> -> E    | Check if there is any parameters by ')'
  if (token.type != T_CPAREN)
    ParseParam(params);
  ParseParamNext(params);
}

/**
 * @brief Parse one function parameter
 *
 * <PARAM> -> <ID> : <TYPE>
 */
void ParseParam(ASTParam **params) {
  // SECOND PASS OF PARSER
  // CREATE NEW AST NODE
  if (!first_pass) (*params) = CreateASTParam();

  // FIRST PASS OF PARSER
  // FILL CURRENT FUNCTION PARAMS IN SYMTABLE
  Param *param_func = NULL;
  symtable_item_data *param_var = NULL;
  if (first_pass) {
    if (token.type == T_ID && token.keyword == K_UNKNOWN) {
      param_func = InvokeAlloc(sizeof(Param));
      param_func->name = token.value.string->str;
      InvokeAddVarsArray(current_func_sym->details.func.params, param_func);
    }
    else InvokeExit(SYNTAX_ERROR);
  }
  // SECOND PASS OF PARSER
  // FULFILL AST NODE INFO
  // ADD PARAM AS CONST VAR TO SYMTABLE
  else {
    if(SymtableFind(token.value.string->str) != NULL) InvokeExit(REDIFINE_ERROR);
    param_var = SymtableAdd(token.value.string->str, ST_VAR_CONST);
    (*params)->name = token.value.string->str;
  }

  //: <TYPE>
  GetNextToken();
  // FIRST PASS OF PARSER
  // FULFILL CURRENT FUNCTION PARAM TYPE IN SYMTABLE
  if(token.type != T_COLON) InvokeExit(SYNTAX_ERROR);
  GetNextToken();
  DataType type = ParseType();
  if (first_pass) {
    switch (type) {
    case ST_NOT_DEFINED:
    case ST_VOID:
      InvokeExit(SYNTAX_ERROR);
      break;
    default:
      param_func->type = type;
      break;
    }
  }
  // SECOND PASS OF PARSER
  else{
    param_var->details.var.type = type;
  }
  // consumes <TYPE>
  GetNextToken();

  if (first_pass)
    ParseParamNext(NULL);
  else
    ParseParamNext(params);
}

/**
 * @brief Try to parse next function parameter
 *
 *  <PARAM_NEXT> -> ,<PARAM> <PARAM_NEXT>
 *  <PARAM_NEXT> -> E
 */
void ParseParamNext(ASTParam **params) {
  if (token.type == T_CPAREN)
    return;
  if (token.type != T_COMMA)
    InvokeExit(SYNTAX_ERROR);
  // consumes ','
  GetNextToken();
  if (first_pass)
    ParseParam(NULL);
  else
    ParseParam(&(*params)->next);
}

/**
 * @brief Parse body of function, if, else etc.
 *
 *  <BODY> -> {<STL>}
 */
void ParseBody(ASTBody **body, bool already_entered) {
  // try to consume '{'
  if (token.type != T_OCURLY)
    InvokeExit(SYNTAX_ERROR);

  // Create new scope in second pass
  if (!first_pass && !already_entered) {
    SymtableEnterScope();
  }

  GetNextToken();
  (*body) = CreateASTBody();
  ParseStatementNext(&(*body)->statement);

  // try to consume '}'
  if (token.type != T_CCURLY)
    InvokeExit(SYNTAX_ERROR);

  if (!first_pass) {
    SymtableLeaveScope();
  }

  GetNextToken();
}

/**
 * @brief Parse statement in body
 *
 * A semicolon has to be consumed by certain statements
 */
void ParseStatement(ASTStatement **statement) {
  if (!first_pass)
    (*statement) = CreateASTStatement();

  //<STAT> -> <IF_STAT>
  if (token.keyword == K_IF) {
    (*statement)->type = T_IF;
    (*statement)->statement = CreateASTIfStatement();
    ParseIfStatement((ASTIfStatement **)&(*statement)->statement);
    //<STAT> -> <ELSE_STAT>
    if (token.keyword == K_ELSE)
      ParseElseStatement((ASTIfStatement **)&(*statement)->statement);
    else InvokeExit(SYNTAX_ERROR);
  }

  //<STAT> -> <WHILE_STAT>
  else if (token.keyword == K_WHILE) {
    (*statement)->type = T_WHILE;
    (*statement)->statement = CreateASTWhileStatement();
    ParseWhileStatement((ASTWhileStatement **)&(*statement)->statement);
  }

  //<STAT> -> <VAR_DECL>
  else if (token.type == T_ID && token.keyword == K_CONST) {
    (*statement)->type = T_VARDECLDEF;
    (*statement)->statement = CreateASTVarDeclDef();
    ParseVarDecl(ST_VAR_CONST, (*statement)->statement);
  } else if (token.type == T_ID && token.keyword == K_VAR) {
    (*statement)->type = T_VARDECLDEF;
    (*statement)->statement = CreateASTVarDeclDef();
    ParseVarDecl(ST_VAR_VAR, (*statement)->statement);
  }

  //<STAT> -> return <RETURN_EXPR>;
  else if (token.type == T_ID && token.keyword == K_RETURN) {
    (*statement)->type = T_RETURN;
    (*statement)->statement = CreateASTReturn();
    ParseReturn((*statement)->statement);
    GetNextToken();
  }

  //<STAT> -> <FUNC_CALL>; but for ifj
  else if (token.type == T_ID && token.keyword == K_IFJ) {
    // consume ifj
    GetNextToken();
    (*statement)->type = T_FCALL;
    (*statement)->statement = CreateASTFuncCall();
    if (ParseFuncCall((*statement)->statement) != ST_VOID)
      InvokeExit(RETURN_ERROR);
    // consume )
    GetNextToken();
    if (token.type != T_SEMICOLON)
      InvokeExit(SYNTAX_ERROR);
    GetNextToken();
  }

  //<STAT> -> <FUNC_CALL>;
  //<STAT> -> <VAR_CNG_DEF>
  else if (token.type == T_ID && token.keyword == K_UNKNOWN) {
    char *name = token.value.string->str;
    GetNextToken();

    //Variable redefinition
    if (token.type == T_ASSIGN) {

      // AST node
      (*statement)->type = T_VARDECLDEF;
      (*statement)->statement = CreateASTVarDeclDef();
      ASTVarDeclDef *varDecl = (*statement)->statement;
      SymtableAssertVariable(name);
      symtable_item_data *var = SymtableUpdate_isModified(name);
      SymtableUpdate_isUsed(name);
      varDecl->name = name;
      DataType redef_type = ParseVarDef(varDecl);
      if(CheckNullableTypes(var->details.var.type, redef_type) == true){}
      else if(redef_type != var->details.var.type) InvokeExit(TYPE_ERROR);
    }

    //Void function call
    else if (token.type == T_OPAREN) {
      // AST node
      (*statement)->type = T_FCALL;
      (*statement)->statement = CreateASTFuncCall();
      if (ParseFuncCall((*statement)->statement) != ST_VOID)
        InvokeExit(PARAMETER_RETURN_ERROR);

      // Consume )
      GetNextToken();

      // Try to consume ;
      if (token.type != T_SEMICOLON)
        InvokeExit(SYNTAX_ERROR);
      GetNextToken();
    }

    else InvokeExit(SYNTAX_ERROR);
  }

  //_ = ...
  else if (token.type == T_UNDERSCORE) {
    GetNextToken();
    if (token.type == T_ASSIGN) {
      (*statement)->type = T_VARDECLDEF;
      (*statement)->statement = CreateASTVarDeclDef();
      ASTVarDeclDef *varDecl = (*statement)->statement;
      ParseVarDef(varDecl);
      varDecl->isIgnoring = true;
    }
    else InvokeExit(SYNTAX_ERROR);
  }

  else {
    InvokeExit(SYNTAX_ERROR);
  }

  ParseStatementNext(&(*statement)->next);
}

/**
 * @brief Try to parse next statement of body
 *
 * <STAT_NEXT> -> <STAT> <STAT_NEXT>
 * <STAT_NEXT> -> E
 *
 *  Left '}' to be consumed by BODY
 */
void ParseStatementNext(ASTStatement **statement) {
  if (token.type == T_CCURLY)
    return;
  else
    ParseStatement(statement);
}

/**
 * @brief Parse if statement
 *
 * <IF_STAT> ->if (<TRUE_EXPR> <BODY>
 *
 */
void ParseIfStatement(ASTIfStatement **statement) {
  // consume IF
  GetNextToken();
  (*statement)->expr = CreateASTExpression();
  DataType if_expr = ParseExpr((*statement)->expr, true, false);
  GetNextToken();

  if(if_expr == ST_BOOL) {
    ParseBody(&(*statement)->ifBody, false);
  }

  //If it's nullable it must continue with new variable in |var|
  else if(if_expr == ST_NULLI32 || if_expr == ST_NULLU8 || if_expr == ST_NULLF64 || if_expr == ST_NULL) {
    SymtableEnterScope();

    (*statement)->notNullID = ParseNotNull(if_expr);

    ParseBody(&(*statement)->ifBody, true);
  }
  else InvokeExit(TYPE_ERROR);
}

/**
 * @brief Parse else statement
 *
 * <ELSE_STAT> -> E
 * <ELSE_STAT> -> else <BODY>
 *
 */
void ParseElseStatement(ASTIfStatement **statement) {
  GetNextToken();
  ParseBody(&(*statement)->elseBody, false);
}

/**
 * @brief Parse while statement
 *
 * <WHILE_STAT> -> while (<TRUE_EXPR> <BODY>
 *
 * *TODO*: Add parsing for true expression
 */
void ParseWhileStatement(ASTWhileStatement **statement) {
  // consume WHILE
  GetNextToken();
  (*statement)->expr = CreateASTExpression();
  DataType while_expr = ParseExpr((*statement)->expr, true, false);
  GetNextToken();

  if(while_expr == ST_BOOL) {
    ParseBody(&(*statement)->whileBody, false);
  }

  //If it's nullable it must continue with new variable in |var|
  else if(while_expr == ST_NULLI32 || while_expr == ST_NULLU8 || while_expr == ST_NULLF64 || while_expr == ST_NULL) {
    SymtableEnterScope();

    (*statement)->notNullID = ParseNotNull(while_expr);

    ParseBody(&(*statement)->whileBody, true);
  }
  else InvokeExit(TYPE_ERROR);
}

char* ParseNotNull(DataType nullable_type) {
  char* notnull_name = NULL;
  if(token.type != T_PIPE) InvokeExit(OTHER_SEMANTIC_ERROR);
  GetNextToken();

  if(token.type == T_ID && token.keyword == K_UNKNOWN) {
    notnull_name = token.value.string->str;

    if(SymtableFind(token.value.string->str) != NULL) InvokeExit(REDIFINE_ERROR);
    symtable_item_data *nullable_var = SymtableAdd(token.value.string->str, ST_VAR_CONST);
    InvokeAddVarsArray(current_func_sym->details.func.variables, token.value.string->str);

    if(nullable_type==ST_NULLI32) nullable_var->details.var.type = ST_I32;
    else if(nullable_type==ST_NULLF64) nullable_var->details.var.type = ST_F64;
    else if(nullable_type==ST_NULLU8) nullable_var->details.var.type = ST_U8;
    else if(nullable_type==ST_NULL) nullable_var->details.var.type = ST_NULL;
  }
  else if(token.type == T_UNDERSCORE) notnull_name = "_";
  else InvokeExit(SYNTAX_ERROR);
  GetNextToken();
  if(token.type != T_PIPE) InvokeExit(SYNTAX_ERROR);
  GetNextToken();

  return notnull_name;
}

/**
 * @brief Parse variable declaration
 *
 * <VAR_DECL> -> const <ID> <VAR_TYPE> <VAR_DEF>
 * <VAR_DECL> -> var <ID> <VAR_TYPE> <VAR_DEF>
 *
 * @param const_type if variable is const or var
 */
void ParseVarDecl(SymbolType const_type, ASTVarDeclDef *var_decl) {
  // consume const/var
  GetNextToken();
  symtable_item_data *var_item = NULL;

  //<ID>
  if (token.type == T_ID && token.keyword == K_UNKNOWN) {

    // Semantic check on redefining variable with the same name in certain scope
    if (SymtableFind(token.value.string->str) != NULL)
      InvokeExit(REDIFINE_ERROR);

    var_item = SymtableAdd(token.value.string->str, const_type);

    var_decl->name = token.value.string->str;
    InvokeAddVarsArray(current_func_sym->details.func.variables,
              token.value.string->str);
    GetNextToken();

    //<VAR_TYPE> -> E
    //<VAR_TYPE> -> : <TYPE>
    DataType var_type = ST_NOT_DEFINED;
    if (token.type == T_COLON) {
      GetNextToken();
      var_type = ParseType();
      GetNextToken();
    }
    var_item->details.var.type = var_type;

    //<VAR_DEF>
    if (token.type != T_ASSIGN)
      InvokeExit(SYNTAX_ERROR);
    DataType decl_type = ParseVarDef(var_decl);

    //All nullable types can be NULL or not nullable type of the same or nullable type of the same type
    if(CheckNullableTypes(var_type, decl_type) == true){}
    //Empty expression
    else if(decl_type == ST_NOT_DEFINED) InvokeExit(SYNTAX_ERROR);
    //Can't decide what type(null or string literal) when there wasn't variable type in declaration
    else if((decl_type == ST_NULL || decl_type == ST_STRING) && var_type == ST_NOT_DEFINED) InvokeExit(TYPE_DEDUCTION_ERROR);
    //Deciding what type from expression
    else if(var_type == ST_NOT_DEFINED) var_item->details.var.type = decl_type;
    //If types isn't the same, then type error
    else if(var_type != decl_type) InvokeExit(TYPE_ERROR);

    if(isknown_return && var_item->symType == ST_VAR_CONST) var_item->details.var.isKnown = true;
  }
  else if(token.keyword == K_IFJ) InvokeExit(REDIFINE_ERROR);
  else InvokeExit(SYNTAX_ERROR);
}

/**
 * @brief Parse variable definition
 *
 * <VAR_DEF> -> = <GEN_EXPR>;
 */
DataType ParseVarDef(ASTVarDeclDef *var_decl) {
  // connsume '='
  // get_next_token();
  var_decl->expr = CreateASTExpression();
  DataType var_def_type = ParseExpr(var_decl->expr, false, false);
  // consume ';'
  GetNextToken();
  return var_def_type;
}

/**
 * @brief Parse return statement
 *
 * <RET_STAT> -> return <RETURN_EXPR>;
 */
void ParseReturn(ASTReturn *statement_return) {
  //Void function return must not have an expression
  if(current_func_sym->details.func.returnType == ST_VOID) {
    GetNextToken();
    if(token.type != T_SEMICOLON) InvokeExit(RETURN_ERROR);
  }
  else {
    statement_return->expr = CreateASTExpression();
    DataType return_type = ParseExpr(statement_return->expr, false, false);
    if(return_type == ST_NOT_DEFINED) InvokeExit(RETURN_ERROR);
    else if(CheckNullableTypes(current_func_sym->details.func.returnType, return_type) == true){}
    else if(return_type != current_func_sym->details.func.returnType) InvokeExit(PARAMETER_RETURN_ERROR);
    current_func_sym->details.func.returnMade = true;
  }
}

/**
 * @brief Parse expression
 *
 */
DataType ParseExpr(ASTExpression *expression, bool additional_parenthesis, bool additional_comma) {
  expression->exprStack = InvokeAlloc(sizeof(stack));
  InitStack(expression->exprStack);
  DataType expr_type = PrecedeneParseExpression(&expression->exprStack, additional_parenthesis, additional_comma);
  expression->returnType = expr_type;
  return expr_type;
}

DataType ParseFuncCall(ASTFuncCall *func_call) {
  DataType return_type = ST_NOT_DEFINED;
  //Helps to decide if error is 4 or 2
  int expected_args = 0;
  int actual_args = 0;

  // GET <ID>
  scanner->current_token -= 2;
  GetNextToken();
  // IFJ functions
  if (token.keyword == K_IFJ) {

    GetNextToken();
    // IFJ <DOT>
    if (token.type != T_DOT)
      InvokeExit(SYNTAX_ERROR);
    GetNextToken();
    // IFJ <DOT> <ID>
    if (token.type != T_ID)
      InvokeExit(SYNTAX_ERROR);

    // ifj.write(term) void
    if (StringEquals(token.value.string, "write")) {
      GetNextToken();
      expected_args=1;
      func_call->name = "ifj.write";
      // consume (
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      if(ParseCallParam(&func_call->params, &actual_args) == ST_VOID) InvokeExit(TYPE_ERROR);
      // consume param
      return_type = ST_VOID;
    }

    // ifj.readstr() ?[]u8
    else if (StringEquals(token.value.string, "readstr")) {
      GetNextToken();
      expected_args=0;
      func_call->name = "ifj.readstr";
      GetNextToken();
      return_type = ST_NULLU8;
    }

    // ifj.readi32() ?i32
    else if (StringEquals(token.value.string, "readi32")) {
      GetNextToken();
      expected_args=0;
      func_call->name = "ifj.readi32";
      GetNextToken();
      return_type = ST_NULLI32;
    }

    // ifj.readf64() ?f64
    else if (StringEquals(token.value.string, "readf64")) {
      GetNextToken();
      expected_args=0;
      func_call->name = "ifj.readf64";
      GetNextToken();
      return_type = ST_NULLF64;
    }

    // ifj.i2f(term ∶ i32) f64
    else if (StringEquals(token.value.string, "i2f")) {
      GetNextToken();
      expected_args=1;
      func_call->name = "ifj.i2f";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      if (ParseCallParam(&func_call->params, &actual_args) != ST_I32)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_F64;
    }

    // ifj.f2i(term ∶ f64) i32
    else if (StringEquals(token.value.string, "f2i")) {
      GetNextToken();
      expected_args=1;
      func_call->name = "ifj.f2i";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      if (ParseCallParam(&func_call->params, &actual_args) != ST_F64)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_I32;
    }

    // ifj.string(term) []u8
    else if (StringEquals(token.value.string, "string")) {
      GetNextToken();
      expected_args=1;
      func_call->name = "ifj.string";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      DataType type = ParseCallParam(&func_call->params, &actual_args);
      if (type != ST_STRING && type != ST_U8)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_U8;
    }

    // ifj.length(s : []u8) i32
    else if (StringEquals(token.value.string, "length")) {
      GetNextToken();
      expected_args=1;
      func_call->name = "ifj.length";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      if (ParseCallParam(&func_call->params, &actual_args) != ST_U8)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_I32;
    }

    // ifj.concat(s1 : []u8, s2 : []u8) []u8
    else if (StringEquals(token.value.string, "concat")) {
      GetNextToken();
      expected_args=2;
      func_call->name = "ifj.concat";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      // s1
      if (ParseCallParam(&func_call->params, &actual_args) != ST_U8)
        InvokeExit(PARAMETER_RETURN_ERROR);

      if (token.type != T_COMMA)
        InvokeExit(SYNTAX_ERROR);
      GetNextToken();
      // s2
      if (ParseCallParam(&func_call->params->next, &actual_args) != ST_U8)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_U8;
    }

    // ifj.substring(s : []u8, i : i32, j : i32) ?[]u8
    else if (StringEquals(token.value.string, "substring")) {
      GetNextToken();
      expected_args=3;
      func_call->name = "ifj.substring";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      // s
      if (ParseCallParam(&func_call->params, &actual_args) != ST_U8)
        InvokeExit(PARAMETER_RETURN_ERROR);
      if (token.type != T_COMMA)
        InvokeExit(SYNTAX_ERROR);
      GetNextToken();
      // i
      if (ParseCallParam(&func_call->params->next, &actual_args) != ST_I32)
        InvokeExit(PARAMETER_RETURN_ERROR);
      if (token.type != T_COMMA)
        InvokeExit(SYNTAX_ERROR);
      GetNextToken();
      // j
      if (ParseCallParam(&func_call->params->next->next, &actual_args) != ST_I32)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_NULLU8;
    }

    // ifj.strcmp(s1 : []u8, s2 : []u8) i32
    else if (StringEquals(token.value.string, "strcmp")) {
      GetNextToken();
      expected_args=2;
      func_call->name = "ifj.strcmp";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      // s1
      if (ParseCallParam(&func_call->params, &actual_args) != ST_U8)
        InvokeExit(PARAMETER_RETURN_ERROR);

      if (token.type != T_COMMA)
        InvokeExit(SYNTAX_ERROR);
      GetNextToken();
      // s2
      if (ParseCallParam(&func_call->params->next, &actual_args) != ST_U8)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_I32;
    }

    // ifj.ord(s : []u8, i : i32) i32
    else if (StringEquals(token.value.string, "ord")) {
      GetNextToken();
      expected_args=2;
      func_call->name = "ifj.ord";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      // s1
      if (ParseCallParam(&func_call->params, &actual_args) != ST_U8)
        InvokeExit(PARAMETER_RETURN_ERROR);

      if (token.type != T_COMMA)
        InvokeExit(SYNTAX_ERROR);
      GetNextToken();
      // i
      if (ParseCallParam(&func_call->params->next, &actual_args) != ST_I32)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_I32;
    }

    // ifj.chr(i : i32) []u8
    else if (StringEquals(token.value.string, "chr")) {
      GetNextToken();
      expected_args=1;
      func_call->name = "ifj.chr";
      if(token.type == T_OPAREN)GetNextToken();
      else InvokeExit(SYNTAX_ERROR);
      if (ParseCallParam(&func_call->params, &actual_args) != ST_I32)
        InvokeExit(PARAMETER_RETURN_ERROR);
      return_type = ST_U8;
    }

    else
      InvokeExit(SYNTAX_ERROR);
  }

  // DEFAULT FUNCTION CALL
  else {
    func_call->name = token.value.string->str;

    // Find function in symtable with semantics check
    symtable_item_data *func = SymtableAssertFunction(func_call->name);
    expected_args=func->details.func.params->size;
    return_type = func->details.func.returnType;
    // consume <ID>
    GetNextToken();

    // Consume (
    GetNextToken();

    ASTParamCall **current = &func_call->params;
    if(expected_args == 0 && token.type != T_CPAREN){
        ParseCallParam(current, &actual_args);
        InvokeFree((*current));
    }

    // Cycle through params in symtable function item
    for (size_t i = 0; i < func->details.func.params->size; i++) {
      Param *symtable_param = func->details.func.params->array[i];

      if (ParseCallParam(current, &actual_args) != symtable_param->type)
        InvokeExit(PARAMETER_RETURN_ERROR);

      // try to consume comma if it isn't last param
      if (i != func->details.func.params->size - 1) {
        if(token.type != T_COMMA && token.type == T_CPAREN)
            InvokeExit(PARAMETER_RETURN_ERROR);
        else if (token.type != T_COMMA)
          InvokeExit(SYNTAX_ERROR);
        GetNextToken();
      }
      current = &(*current)->next;
    }

  }

  ASTParamCall *temp = NULL;
  while(token.type == T_COMMA){
      GetNextToken();
      ParseCallParam(&temp, &actual_args);
      temp = (temp)->next;
  }
  temp = NULL;
  if(token.type != T_CPAREN) InvokeExit(SYNTAX_ERROR);
  if(actual_args != expected_args) InvokeExit(PARAMETER_RETURN_ERROR);
  return return_type;
}

DataType ParseCallParam(ASTParamCall **func_call_param, int* counter_args) {
  *func_call_param = CreateASTParamCall();
  *counter_args = *counter_args + 1;
  (*func_call_param)->expr = CreateASTExpression();
  scanner->current_token--;
  DataType paramtype = ParseExpr((*func_call_param)->expr,true, true);
  scanner->current_token--;
  GetNextToken();
  return paramtype;
}

/**
 * @brief Parse a type
 *
 * <FUNC_TYPE> -> void
 * <FUNC_TYPE> -> <TYPE>
 * <TYPE> -> i32
 * <TYPE> -> f64
 * <TYPE> -> []u8
 *
 *
 * @return DataType type
 */
DataType ParseType() {
  //?...
  bool is_nullable = false;
  if(token.type == T_QUESTM) {
    is_nullable = true;
    GetNextToken();
  }

  // []u8 / ?[]u8
  if (token.type == T_OSQUARE) {
    GetNextToken();
    if (token.type == T_CSQUARE) {
      GetNextToken();
      if (token.type == T_ID && token.keyword == K_U8) {
        return is_nullable? ST_NULLU8 : ST_U8;
      }
      else InvokeExit(SYNTAX_ERROR);
    }
    else InvokeExit(SYNTAX_ERROR);
  }
  // i32 / ?i32
  else if (token.type == T_ID && token.keyword == K_I32) {
    return is_nullable ? ST_NULLI32 : ST_I32;
  }
  // f64 / ?f64
  else if (token.type == T_ID && token.keyword == K_F64) {
    return ST_F64;
  }
  // void
  else if (token.type == T_ID && token.keyword == K_VOID) {
    //?void is syntax error
    if(is_nullable) InvokeExit(SYNTAX_ERROR);
    return ST_VOID;
  }
  return ST_NOT_DEFINED;
}


bool CheckNullableTypes(DataType base, DataType from_expression){
    if(base == ST_NULLI32 && (from_expression == ST_NULL || from_expression == ST_NULLI32 || from_expression == ST_I32)) return true;
    else if(base == ST_NULLU8 && (from_expression == ST_NULL || from_expression == ST_NULLU8 || from_expression == ST_U8)) return true;
    else if(base == ST_NULLF64 && (from_expression == ST_NULL || from_expression == ST_NULLF64 || from_expression == ST_F64)) return true;
    return false;
}
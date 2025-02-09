/**
 * @file expr_parser.c
 * Project - IFJ Project 2024
 *
 * @author Albert Tikaiev xtikaia00
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief Source file for expression parser
 */

#include "expr_parser.h"

Token expr_token;
bool isknown_return;

const char precedence_lookup[7][7] = {
            /*m    a    id   rel  (    )    $ */
  /*mul */ {'>', '>', '<', '>', '<', '>', '>'},
  /*add */ {'<', '>', '<', '>', '<', '>', '>'},
  /* id */ {'>', '>', '-', '>', '-', '>', '>'},
  /*rel */ {'<', '<', '<', '-', '<', '>', '>'},
  /* (  */ {'<', '<', '<', '<', '<', '=', '-'},
  /* )  */ {'>', '>', '-', '>', '-', '>', '>'},
  /* $  */ {'<', '<', '<', '<', '<', '-', '-'},
};

/**
 * @brief Get next token from token array
 */
void GetNextTokenExpr() {
  expr_token = *scanner->tokens[scanner->current_token];
  scanner->current_token++;
}

/**
 * @brief Get most top terminal from pushdown stack
 *
 * @param pushdown pushdown stack
 * @return Top terminal from stack
 */
expr* GetTopTerminal(stack* pushdown) {
  stackItem* current = pushdown->top;
  while (((expr*)(current->data))->isTerminal == false) {
    current = current->next;
  }
  return current->data;
}

/**
 * @brief Makes most top terminal isLess value to true
 *
 * @param pushdown pushdown stack
 */
void MakeTopTerminalLess(stack *pushdown) {
  expr* top = GetTopTerminal(pushdown);
  top->isLess = true;
}

/**
 * @brief Copy token from expr_token to use it in expr_data structure
 *
 * @return copied token
 */
Token* CopyToken() {
  Token* copy_token = InvokeAlloc(sizeof(Token));
  copy_token->keyword = expr_token.keyword;
  copy_token->type = expr_token.type;
  copy_token->value = expr_token.value;
  return copy_token;
}

/**
 * @brief Get precedence enum value from current token
 *
 * @param additional_parenthesis if expression is parsing in if/while/function params
 * @param count_parentheses count of parentheses
 * @param additional_comma if expression is parsing in function params
 *
 * @return precedence enum value
 */
precedenceType GetType(bool additional_parenthesis, int *count_parentheses, bool additional_comma) {
  switch (expr_token.type) {
  case T_PLUS:
  case T_SUB:
    return O_PLUS;
  case T_MUL:
  case T_DIV:
    return O_MUL;
  case T_ID:
  case T_INT:
  case T_FLOAT:
  case T_STR:
    return O_ID;
  case T_GEQ:
  case T_GT:
  case T_LT:
  case T_LEQ:
  case T_EQ:
  case T_NEQ:
    return O_RELATIONAL;
  case T_OPAREN:
    *count_parentheses = *count_parentheses + 1;
    return O_OPENPAREN;
  case T_CPAREN:
    if (additional_parenthesis && ((*count_parentheses) == 0))
      return O_DOLLAR;
    else if((*count_parentheses)>=1) {
      *count_parentheses = *count_parentheses - 1;
      return O_CLOSEPAREN;
    }
    else InvokeExit(SYNTAX_ERROR);
    break;
  case T_COMMA:
    if(additional_comma) return O_DOLLAR;
    else InvokeExit(SYNTAX_ERROR);
    break;
  case T_SEMICOLON:
    return O_DOLLAR;
    break;
  case T_UNDERSCORE:
      InvokeExit(DEFINITION_ERROR);
      break;
  default:
    InvokeExit(SYNTAX_ERROR);
  }
  return O_DOLLAR;
}

/**
 * @brief Creates new expr item from current token
 *
 * @param additional_parenthesis if expression is parsing in if/while/function params
 * @param count_parentheses count of parentheses
 * @param additional_comma if expression is parsing in function params
 *
 * @return new expr item
 */
expr *CreateExprItem(bool additional_parenthesis, int *count_parentheses, bool additional_comma) {
  expr* expression = InvokeAlloc(sizeof(expr));
  expr_data *item = InvokeAlloc(sizeof(expr_data));

  expression->isLess = false;
  expression->isTerminal = true;
  expression->isLiteral = false;
  expression->isKnownConstant = false;
  item->type = GetType(additional_parenthesis, count_parentheses, additional_comma);
  expression->item = item;

  switch (expr_token.type) {
  case T_ID: {
    if(expr_token.keyword == K_NULL) {
      item->isFunction = false;
      item->data.token = CopyToken();
      expression->returnType = ST_NULL;
    }
    else if(expr_token.keyword == K_UNKNOWN || expr_token.keyword == K_IFJ) {
      GetNextTokenExpr();
      if (expr_token.type == T_OPAREN || expr_token.type == T_DOT) {
        item->isFunction = true;
        item->data.funcCall = InvokeAlloc(sizeof(ASTFuncCall));
        item->data.funcCall->params = NULL;
        expression->returnType = ParseFuncCall(item->data.funcCall);
      } else {
        scanner->current_token -= 2;
        GetNextTokenExpr();
        item->isFunction = false;
        if(expr_token.keyword != K_UNKNOWN) InvokeExit(SYNTAX_ERROR);
        item->data.token = CopyToken();
        symtable_item_data *var = SymtableUpdate_isUsed(expr_token.value.string->str);
        expression->isKnownConstant = var->details.var.isKnown;
        expression->returnType = var->details.var.type;
      }
    }
    else InvokeExit(SYNTAX_ERROR);
    break;
  }
  case T_INT:
    expression->returnType = ST_I32;
    expression->isLiteral = true;
    expression->isKnownConstant = true;
    item->isFunction = false;
    item->data.token = CopyToken();
    break;
  case T_FLOAT:
    expression->returnType = ST_F64;
    expression->isLiteral = true;
    expression->isKnownConstant = true;
    item->isFunction = false;
    item->data.token = CopyToken();
    break;
  case T_STR:
    expression->returnType = ST_STRING;
    expression->isLiteral = true;
    expression->isKnownConstant = true;
    item->isFunction = false;
    item->data.token = CopyToken();
    break;
  default:
    expression->returnType = ST_NOT_DEFINED;
    item->isFunction = false;
    item->data.token = CopyToken();
    break;
  }

  return expression;
}

/**
 * @brief Perform operation on 1 or 3 expr items
 * And return only one back on pushdown stack to continue on semantic checks
 *
 * @param pushdown pusdown stack
 * @param infix infix stack
 */
void Reduce(stack* pushdown, stack* infix) {
  //Get items for reduce
  expr* ontop = TopStack(pushdown);
  expr* lhs=NULL;
  expr* op =NULL;
  expr* rhs=NULL;
  int until_less;

  for (until_less = 0; until_less < 3; until_less++) {
    if(until_less==0) rhs = ontop;
    else if(until_less==1) op = ontop;
    else if(until_less==2) lhs = ontop;
    PopStack(pushdown);
    ontop = TopStack(pushdown);
    if(ontop->isLess) break;
  }


  if(until_less == 0) {
    //i -> E
    rhs->isTerminal = false;
    ontop->isLess = false;
    PushStack(infix, rhs);
    PushStack(pushdown, rhs);
  }
  else if(until_less == 1) {
    InvokeExit(SYNTAX_ERROR);
  }
  else if(until_less == 2) {
    op->isTerminal = false;
    //(E)->E
    if(lhs->item->type == O_OPENPAREN && rhs->item->type == O_CLOSEPAREN) {}

    // (i == i) == i->SYNTAX ERROR
    else if((lhs->returnType ==ST_BOOL || rhs->returnType ==ST_BOOL) && op->item->type == O_RELATIONAL) InvokeExit(SYNTAX_ERROR);

    // +-/*
    else if(op->item->type == O_PLUS || op->item->type == O_MUL) {
      //f64/f64 and i32/i32 can be multiplied or added always
      if(lhs->returnType==ST_F64 && rhs->returnType==ST_F64) op->returnType = ST_F64;
      else if(lhs->returnType==ST_I32 && rhs->returnType==ST_I32) op->returnType = ST_I32;
      //f64 and i32 can be multiplied or added only in case if i32 is literal
      else if(lhs->returnType==ST_F64 && rhs->returnType==ST_I32) {
        if(!rhs->isLiteral) InvokeExit(TYPE_ERROR);
        if(op->item->data.token->type == T_DIV) InvokeExit(TYPE_ERROR);
        op->returnType = ST_F64;
      }
      else if(lhs->returnType==ST_I32 && rhs->returnType==ST_F64) {
        if(!lhs->isLiteral) InvokeExit(TYPE_ERROR);
        if(op->item->data.token->type == T_DIV) InvokeExit(TYPE_ERROR);
        op->returnType = ST_F64;
      }
      else InvokeExit(TYPE_ERROR);

      op->isKnownConstant = lhs->isKnownConstant && rhs->isKnownConstant;
      op->isLiteral = lhs->isLiteral && rhs->isLiteral;
      PushStack(infix, op);
    }
    // == != >= <= ...
    else if(op->item->type == O_RELATIONAL) {
      //f64/f64 and i32/i32 can be compared always
      if((lhs->returnType==ST_F64 && rhs->returnType==ST_F64)
        || (lhs->returnType==ST_I32 && rhs->returnType==ST_I32)) {}
      //Also we can compare ?i32 with i32 and ?f64 with f64, but only with operators == !=
      else if((lhs->returnType==ST_NULLF64 && rhs->returnType==ST_F64)
        ||    (lhs->returnType==ST_F64 && rhs->returnType==ST_NULLF64)
        ||    (lhs->returnType==ST_NULLI32 && rhs->returnType==ST_I32)
        ||    (lhs->returnType==ST_I32 && rhs->returnType==ST_NULLI32)) {
        if(op->item->data.token->type != T_EQ && op->item->data.token->type != T_NEQ) InvokeExit(TYPE_ERROR);
      }
      //We can compare everything with null that can possibly obtain null, but only with operators == !=
      else if(lhs->returnType==ST_NULL) {
        if(op->item->data.token->type != T_EQ && op->item->data.token->type != T_NEQ) InvokeExit(TYPE_ERROR);
        if(rhs->returnType != ST_NULL && rhs->returnType != ST_NULLI32 &&
          rhs->returnType != ST_NULLF64 && rhs->returnType != ST_NULLU8) InvokeExit(TYPE_ERROR);
      }
      else if(rhs->returnType==ST_NULL) {
        if(op->item->data.token->type != T_EQ && op->item->data.token->type != T_NEQ) InvokeExit(TYPE_ERROR);
        if(lhs->returnType != ST_NULL && lhs->returnType != ST_NULLI32 &&
          lhs->returnType != ST_NULLF64 && lhs->returnType != ST_NULLU8) InvokeExit(TYPE_ERROR);
      }
      //There is explicit implicit cast i32->f64 if it's known at compile time
      else if((rhs->returnType==ST_F64 && lhs->returnType==ST_I32 && (lhs->isLiteral == true || lhs->isKnownConstant == true)) ||
          (lhs->returnType==ST_F64 && rhs->returnType==ST_I32 && (rhs->isLiteral == true || rhs->isKnownConstant == true))) {}
      else InvokeExit(TYPE_ERROR);

      op->returnType = ST_BOOL;
      PushStack(infix, op);
    }
    else InvokeExit(SYNTAX_ERROR);
    ontop->isLess = false;
    PushStack(pushdown, op);
  }
}

/**
 * @brief Main function of precedence analysis, which is used to parse expressions
 *
 * @param output_stack Stack that will have expression in postfix notation
 * @param parenthesis If there can be additional parenthesis in expression
 * @param comma If there can be comma in expression
 *
 * @return Type of expression
 */
DataType PrecedeneParseExpression(stack **output_stack, bool parenthesis, bool comma) {
  DataType return_type;
  int count_parentheses = 0;
  stack *pushdown = InvokeAlloc(sizeof(stack));
  stack *infix = InvokeAlloc(sizeof(stack));

  isknown_return = true;
  bool expr_has_f64 = false;

  InitStack(infix);
  InitStack(pushdown);

  //$ on bottom always
  expr *left = InvokeAlloc(sizeof(expr));
  left->item = InvokeAlloc(sizeof(expr_data));
  left->isLess = false;
  left->isTerminal = true;
  left->item->type = O_DOLLAR;
  left->returnType = ST_NOT_DEFINED;
  PushStack(pushdown, left);

  //get first token of expression
  GetNextTokenExpr();
  expr *right = CreateExprItem(parenthesis, &count_parentheses, comma);

  while (true) {
    left = GetTopTerminal(pushdown);
    char precedence = precedence_lookup[left->item->type][right->item->type];

    //Pushdown stack must be $E in the end
    if (left->item->type == O_DOLLAR && right->item->type == O_DOLLAR) {
      expr* final = TopStack(pushdown);
      PopStack(pushdown);
      return_type = final->returnType;
      if(final->item->type != O_DOLLAR){
        isknown_return = final->isKnownConstant;
      }
      break;
    }

    //Actions
    switch (precedence) {
      case '<': {
        MakeTopTerminalLess(pushdown);
        PushStack(pushdown, right);
        GetNextTokenExpr();
        right = CreateExprItem(parenthesis, &count_parentheses, comma);
        break;
      }
      case '>': {
        if(left->returnType == ST_F64) expr_has_f64 = true;
        Reduce(pushdown, infix);
        break;
      }
      case '=':
        GetNextTokenExpr();
        PushStack(pushdown, right);
        right = CreateExprItem(parenthesis, &count_parentheses, comma);
        break;
      case '-':
        InvokeExit(SYNTAX_ERROR);
      }
  }

  //Flip over infix stack so output_stack will have expression in postfix notation
  while (!IsEmptyStack(infix)) {
    expr *expr_item = TopStack(infix);
    if(expr_has_f64 && expr_item->returnType == ST_I32) expr_item->item->i2f = true;
    else expr_item->item->i2f = false;
    PopStack(infix);
    PushStack((*output_stack), expr_item->item);
    InvokeFree(expr_item);
  }

  FreeStack(infix);
  FreeStack(pushdown);

  return return_type;
}
/**
 * @file cgen.c
 * Project - IFJ Project 2024
 *
 * @author Danil Tasmassys xtasmad00
 *
 * @brief Assembly code generator
 */

#include "cgen.h"

int ifcnt = 1;//global variable that will be used for labels of whiles and ifs unique id 
stack *stackloh;//stack where will be stored value of if or while that is currently being processed 

/**
 * Creating header and all needed for further code creation and proceeding of all functions
 *
 * @param code pointer to the AST tree where all code information is located
 *
 * @return
 */
void CgenStart(ASTStart *code) {
  fprintf(stdout, ".IFJcode24\n");//header of a file that's needed for interpret 
  fprintf(stdout,
          "DEFVAR GF@lefttrue\nDEFVAR GF@righttrue\nDEFVAR GF@inputread\n");//auxiliary variables for logic expr. and for ifj.read 
  fprintf(stdout, "DEFVAR GF@str1\nDEFVAR GF@str2\n");//auxiliary variables for 2 strings in function 
  fprintf(stdout, "DEFVAR GF@cnt\nDEFVAR GF@length1\nDEFVAR GF@length2\n");//auxiliary variables for length and some counter 
  fprintf(stdout, "DEFVAR GF@notnullable\n");//auxiliary variable for | | parts of code
  fprintf(stdout, "DEFVAR GF@sym1\nDEFVAR GF@sym2\n");//auxiliary variables for symbols
  fprintf(stdout, "DEFVAR GF@$iftrue\nDEFVAR GF@%%retval\n");//auxiliary variables for logic part and return value 
  fprintf(stdout, "CALL $$main\n");//so we won't be dependent of order of functions 
  fprintf(stdout, "EXIT int@0 \n\n");//end of whole code will always be here(in right confitions) 
  stackloh = InvokeAlloc(sizeof(stack));
  InitStack(stackloh);
  ASTFuncDecl *func = code->funcdecls;
  while (func != NULL) {//functions well be in linked list, so we need to proceed them all 
    CgenFuncDecl(func);
    func = func->next;
    fprintf(stdout, "\n");//tabulation for better readability 
  }
  InvokeFree(stackloh);
}

/**
 * Proceeding of all params of a function
 *
 * @param param pointer to the AST node where all params is in a linked list
 *
 * @return
 */
void CgenParam(ASTParam *param) {
  int intik = 1;
  while (param != NULL) {//unlike all variables that will be used in function, here we know their value, and it will be const. 
    fprintf(stdout, "DEFVAR LF@%s\n", param->name);
    fprintf(stdout, "MOVE LF@%s LF@%%%i\n", param->name, intik);
    intik++;
    param = param->next;
  }
}

/**
 * Proceeding function declaration 
 *
 * @param func pointer to the AST node where is located information about function
 *
 * @return
 */
void CgenFuncDecl(ASTFuncDecl *func) {
  fprintf(stdout, "LABEL $$%s\n", func->name);
  if (!strcmp(func->name, "main")) {//only if it's main we need to create a frame
    fprintf(stdout, "CREATEFRAME\n");// otherwise it will be created when function is called 
  }
  fprintf(stdout, "PUSHFRAME\n");//so work will be in local frame 
  if (strcmp(func->name, "main")) {
    fprintf(stdout, "MOVE GF@%%retval nil@nil\n");//sth like inicialization, to prevent errors 
  }
  CgenParam(func->params);
  if (func->variables != NULL) {//proceeding of all variables that will be used in this function 
    for (size_t i = 0; i < func->variables->size; i++) {
      fprintf(stdout, "DEFVAR LF@%s\n", (char *)func->variables->array[i]);
    }
  }
  if(func->body != NULL){//if there is a body, we need to proceed it 
    CgenBody(func->body);
  }
  fprintf(stdout, "POPFRAME\n"); 
  fprintf(stdout, "RETURN\n"); 
}

/**
 * Proceeding of expr in postfix notation
 *
 * @param expr pointer to the stack where all elements of expt is contained
 * @param callfromfunc bool for indication is it called from function (part of funexp extension)
 *
 * @return
 */
void CgenExpr(ASTExpression *expr, bool callfromfunc) {//we use postfix notation because it's easier to make in assembly due properties of stack.
  expr_data *item = TopStack(expr->exprStack);
  while (!IsEmptyStack(expr->exprStack)) {//until this stack isn't empty expr continuos
    switch (item->type) {
    case (O_MUL)://both mul and div are in the same place 
      if (item->data.token->type == T_MUL) {
        fprintf(stdout, "MULS\n");
      } else if (item->data.token->type == T_DIV) {
        if(expr->returnType == ST_I32) fprintf(stdout, "IDIVS\n");//I because of int
        else fprintf(stdout, "DIVS\n");
      }
      break;
    case (O_PLUS):
      if (item->data.token->type == T_PLUS) {
        fprintf(stdout, "ADDS\n");
      } else if (item->data.token->type == T_SUB) {
        fprintf(stdout, "SUBS\n");
      }
      break;
    case (O_RELATIONAL):
      if (item->data.token->type == T_LT) {
        fprintf(stdout, "LTS\n");
        fprintf(stdout, "PUSHS bool@true\n");
        fprintf(stdout, "JUMPIFNEQS $else%d\n", *(int *)TopStack(stackloh));
      } else if (item->data.token->type == T_GT) {
        fprintf(stdout, "GTS\n");
        fprintf(stdout, "PUSHS bool@true\n");
        fprintf(stdout, "JUMPIFNEQS $else%d\n", *(int *)TopStack(stackloh));
      } else if (item->data.token->type == T_EQ) {
        fprintf(stdout, "EQS\n");
        fprintf(stdout, "PUSHS bool@true\n");
        fprintf(stdout, "JUMPIFNEQS $else%d\n", *(int *)TopStack(stackloh));
      } else if (item->data.token->type == T_NEQ) {
        fprintf(stdout, "EQS\n");
        fprintf(stdout, "PUSHS bool@false\n");
        fprintf(stdout, "JUMPIFNEQS $else%d\n", *(int *)TopStack(stackloh));
      } else if (item->data.token->type == T_LEQ) {//because there is no <= in assembly we do it in parts
        fprintf(stdout, "POPS GF@righttrue\n");//here we put both sides in auxiliary vars
        fprintf(stdout, "POPS GF@lefttrue\n");
        fprintf(stdout, "LT GF@$iftrue GF@lefttrue  GF@righttrue\n");//firstly we check <
        fprintf(stdout, "JUMPIFEQ $if%i GF@$iftrue bool@true\n",*(int *)TopStack(stackloh));
        fprintf(stdout, "EQ GF@$iftrue GF@lefttrue  GF@righttrue\n");//if it's not <, it still can be =
        fprintf(stdout, "JUMPIFEQ $if%i GF@$iftrue bool@true\n",*(int *)TopStack(stackloh));
        fprintf(stdout, "JUMP $else%d\n", *(int *)TopStack(stackloh));//if it's > we do else
      } else if (item->data.token->type == T_GEQ) {//same principle here
        fprintf(stdout, "POPS GF@righttrue\n");
        fprintf(stdout, "POPS GF@lefttrue\n");
        fprintf(stdout, "GT GF@$iftrue GF@lefttrue  GF@righttrue\n");
        fprintf(stdout, "JUMPIFEQ $if%i GF@$iftrue bool@true\n",*(int *)TopStack(stackloh));
        fprintf(stdout, "EQ GF@$iftrue GF@lefttrue  GF@righttrue\n");
        fprintf(stdout, "JUMPIFEQ $if%i GF@$iftrue bool@true\n",*(int *)TopStack(stackloh));
        fprintf(stdout, "JUMP $else%d\n", *(int *)TopStack(stackloh));
      }
      break;
    case (O_ID):
      if (item->isFunction) {//here we need to check if it's function because proceeding of it is completely different function
        CgenFuncCall(item->data.funcCall, true, callfromfunc);
      } else if (!item->isFunction) {
        if (item->data.token->keyword == K_NULL) {
          fprintf(stdout, "PUSHS nil@nil\n");
        } else if (item->data.token->type == T_INT) {
          fprintf(stdout, "PUSHS int@%i\n", item->data.token->value.integer);
          if(item->i2f){//if expr have some f64 and int we need to change int to f64 
            fprintf(stdout, "INT2FLOATS\n");
          }
        } else if (item->data.token->type == T_FLOAT) {
          fprintf(stdout, "PUSHS float@%a\n", item->data.token->value.real);
        } else if (item->data.token->type == T_STR) {
          fprintf(stdout, "PUSHS string@%s\n",
                  item->data.token->value.string->str);
        } else if (item->data.token->type == T_ID) {//also if var is i64 but expr is in f64, we need to make it also f64
          fprintf(stdout, "PUSHS LF@%s\n", item->data.token->value.string->str);
          if(item->i2f){
            fprintf(stdout, "INT2FLOATS\n");
          }
        }
      }
      break;
    default:
      break;
    }
    PopStack(expr->exprStack);
    item = TopStack(expr->exprStack);
  }
}

/**
 * Proceeding a return statement
 *
 * @param ret pointer to the AST node where is writed info about return statement in code
 *
 * @return
 */
void CgenReturnStat(ASTReturn *ret) {
  if(ret->expr != NULL){//if it's not a void function, we need to proceed an expr
    CgenExpr(ret->expr, false);
    fprintf(stdout, "POPS GF@%%retval\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
  }else {
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
  }
}

/**
 * Proceeding of an body where al statements is contained in a linked list
 *
 * @param body pointer to the AST node where is located information about body of a function, if, while or else
 *
 * @return
 */
void CgenBody(ASTBody *body) {
  ASTStatement *curr = body->statement;
  while (curr != NULL) {//depend on type of statement we call corresponding function
    if (curr->type == T_RETURN) {
      CgenReturnStat(curr->statement);
    } else if (curr->type == T_FCALL) {
      CgenFuncCall(curr->statement, false, false);
    } else if (curr->type == T_VARDECLDEF) {
      CgenVarDeclDef(curr->statement);
    } else if (curr->type == T_IF) {
      CgenIf(curr->statement);
    } else if (curr->type == T_WHILE) { 
      CgenWhile(curr->statement);
    }
    curr = curr->next;
  }
}

/**
 * Proceeding of an while() or while()||
 *
 * @param cyklus pointer to the AST node where is located information about while, its body, expr and if it's | | type or not
 *
 * @return
 */
void CgenWhile(ASTWhileStatement *cyklus){
  int *i = InvokeAlloc(sizeof(int));
  *i = ifcnt;
  ifcnt++;
  PushStack(stackloh, i);
  if(cyklus->notNullID == NULL){//if it's just while(sth){}
    fprintf(stdout, "LABEL $while%d\n", *i);
    CgenExpr(cyklus->expr, false);//every time we check here condition
    fprintf(stdout, "LABEL $if%d\n", *i);
    CgenBody(cyklus->whileBody);
    fprintf(stdout, "JUMP $while%d\n", *i);
    fprintf(stdout, "LABEL $else%d\n", *i);
  }
  else{//if it's while(sth)|sth|{}
    fprintf(stdout, "LABEL $while%d\n", *i);
    CgenExpr(cyklus->expr, false);
    fprintf(stdout, "POPS GF@notnullable\n");
    fprintf(stdout, "PUSHS GF@notnullable\n");
    fprintf(stdout, "PUSHS nil@nil\n");
    fprintf(stdout, "JUMPIFEQS $else%d\n",*i);
    if(strcmp(cyklus->notNullID, "_")){//if it's not _ we must move it to the variable
    	fprintf(stdout, "PUSHS GF@notnullable\n");
	    fprintf(stdout, "POPS LF@%s\n",cyklus->notNullID);
    }
    CgenBody(cyklus->whileBody);
    fprintf(stdout, "JUMP $while%d\n", *i);
    fprintf(stdout, "LABEL $else%d\n", *i);
  }
  PopStack(stackloh);
}

/**
 * Proceeding of an if()else() or if()||(else)
 *
 * @param ifelse pointer to the AST node where is located information about if(and its else), its body, expr and if it's | | type or not
 *
 * @return
 */
void CgenIf(ASTIfStatement *ifelse) {
  int *i = InvokeAlloc(sizeof(int));//otherwise won't work if we put in stack a ifcnt, we'll have bad value
  *i = ifcnt;
  ifcnt++;
  PushStack(stackloh, i);
  if(ifelse->notNullID == NULL){
    CgenExpr(ifelse->expr, false);//here we check condition
    fprintf(stdout, "LABEL $if%d\n", *i);
    CgenBody(ifelse->ifBody);//here will be body of if
    fprintf(stdout, "JUMP $skip%d\n", *i);//if body of is ended we don't need to do body of else so we skip it
    fprintf(stdout, "LABEL $else%d\n", *i);//if condition does not apply, we jump here
    CgenBody(ifelse->elseBody);
    fprintf(stdout, "LABEL $skip%d\n", *i);//<- here is this skip
  }
  else{
    CgenExpr(ifelse->expr, false);
    fprintf(stdout, "POPS GF@notnullable\n");
    fprintf(stdout, "PUSHS GF@notnullable\n");
    fprintf(stdout, "PUSHS nil@nil\n");//check for null
    fprintf(stdout, "JUMPIFEQS $else%d\n",*i);
    if(strcmp(ifelse->notNullID, "_")){
    	fprintf(stdout, "PUSHS GF@notnullable\n");
	    fprintf(stdout, "POPS LF@%s\n",ifelse->notNullID);
    }
    CgenBody(ifelse->ifBody);
    fprintf(stdout, "JUMP $skip%d\n", *i);
    fprintf(stdout, "LABEL $else%d\n", *i);
    CgenBody(ifelse->elseBody);
    fprintf(stdout, "LABEL $skip%d\n", *i);
  }
  PopStack(stackloh);
}

/**
 * Proceeding of an variable definition
 *
 * @param decloh pointer to the AST node where is located information about variable definition
 *
 * @brief because we do all defvars in a head of a function, it's not quite declaration
 *
 * @return
 */
void CgenVarDeclDef(ASTVarDeclDef *decloh) {
  CgenExpr(decloh->expr, false);//firstly we need to proceed expr, then assign
  if(decloh->isIgnoring){//if it's _
    fprintf(stdout, "POPS GF@inputread\n");//it's like just for clearing stack
  }
  else{
    fprintf(stdout, "POPS LF@%s\n", decloh->name);//otherwise we need to put it in variable, we do it due postfix notation and work in stack primarly
  }
}

/**
 * Proceeding of an function call
 *
 * @param decloh pointer to the AST node where is located information about this function call
 * @param isdef bool for situation when it's var = function()
 * @param callfromfunc bool for situation when it's function1(function2()) (part of an funexp extension)
 *
 * @return
 */
void CgenFuncCall(ASTFuncCall *fcall, bool isdef, bool callfromfunc) {
  ASTParamCall *param = fcall->params;
  if (!strcmp(fcall->name, "ifj.write")) {
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@inputread\n");
    fprintf(stdout, "WRITE GF@inputread\n");//inputread is used everywhere as auxiliary variable for all
    return;
  } else if (!strcmp(fcall->name, "ifj.readi32")) {
    fprintf(stdout, "READ GF@inputread int\n");
    fprintf(stdout, "PUSHS GF@inputread\n");
    return;
  } else if (!strcmp(fcall->name, "ifj.readf64")) { 
    fprintf(stdout, "READ GF@inputread float\n"); 
    fprintf(stdout, "PUSHS GF@inputread\n"); 
    return; 
  } else if (!strcmp(fcall->name, "ifj.readstr")) { 
    fprintf(stdout, "READ GF@inputread string\n"); 
    fprintf(stdout, "PUSHS GF@inputread\n"); 
    return; 
  } else if (!strcmp(fcall->name, "ifj.chr")) { 
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@cnt\n");
    fprintf(stdout, "INT2CHAR GF@inputread GF@cnt\n"); 
    fprintf(stdout, "PUSHS GF@inputread\n"); 
    return; 
  } else if (!strcmp(fcall->name, "ifj.ord")) { 
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@cnt\n");
    CgenExpr(param->next->expr, true);
    fprintf(stdout, "POPS GF@sym1\n");
    fprintf(stdout, "STRI2INT GF@inputread GF@cnt GF@sym1\n");
    fprintf(stdout, "PUSHS GF@inputread\n"); 
    return; 
  } else if (!strcmp(fcall->name, "ifj.concat")) { 
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@sym1\n");
    CgenExpr(param->next->expr, true);
    fprintf(stdout, "POPS GF@sym2\n");
    fprintf(stdout, "CONCAT GF@inputread GF@sym1 GF@sym2\n");
    fprintf(stdout, "PUSHS GF@inputread\n"); 
    return; 
  } else if (!strcmp(fcall->name, "ifj.string")) { 
    CgenExpr(param->expr, true);
    return; 
  } else if (!strcmp(fcall->name, "ifj.length")) { 
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@sym1\n");
    fprintf(stdout, "STRLEN GF@inputread GF@sym1\n");
    fprintf(stdout, "PUSHS GF@inputread\n"); 
    return; 
  } else if (!strcmp(fcall->name, "ifj.i2f")) { 
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@sym1\n");
    fprintf(stdout, "INT2FLOAT GF@inputread GF@sym1\n");
    fprintf(stdout, "PUSHS GF@inputread\n");  
    return; 
  } else if (!strcmp(fcall->name, "ifj.f2i")) { 
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@sym1\n");
    fprintf(stdout, "FLOAT2INT GF@inputread GF@sym1\n");
    fprintf(stdout, "PUSHS GF@inputread\n");  
    return; 
  } else if (!strcmp(fcall->name, "ifj.substring")) { 
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@str1\n");   
    CgenExpr(param->next->expr, true);
    fprintf(stdout, "POPS GF@cnt\n");       
    CgenExpr(param->next->next->expr, true);
    fprintf(stdout, "POPS GF@length2\n");  
    fprintf(stdout, "STRLEN GF@length1 GF@str1\n");
    fprintf(stdout, "LT GF@inputread GF@cnt int@0\n");//check of all bad options, listed in documentation
    fprintf(stdout, "JUMPIFEQ $$null%i$$ GF@inputread bool@true\n",ifcnt);
    fprintf(stdout, "LT GF@inputread GF@length2 int@0\n");
    fprintf(stdout, "JUMPIFEQ $$null%i$$ GF@inputread bool@true\n",ifcnt);
    fprintf(stdout, "GT GF@inputread GF@cnt GF@length2\n");
    fprintf(stdout, "JUMPIFEQ $$null%i$$ GF@inputread bool@true\n",ifcnt);
    fprintf(stdout, "GT GF@inputread GF@length2 GF@length1\n");
    fprintf(stdout, "JUMPIFEQ $$null%i$$ GF@inputread bool@true\n",ifcnt);
    fprintf(stdout, "GT GF@inputread GF@cnt GF@length1\n");
    fprintf(stdout, "JUMPIFEQ $$null%i$$ GF@inputread bool@true\n",ifcnt);
    fprintf(stdout, "JUMPIFEQ $$null%i$$ GF@cnt GF@length1\n",ifcnt);
    fprintf(stdout, "GETCHAR GF@str2 GF@str1 GF@cnt\n");//here we take first symbol, because part in cykle writed by concat, so we need some first char in this
    fprintf(stdout, "ADD GF@cnt GF@cnt int@1\n"); 
    fprintf(stdout, "LABEL $$substring%i$$\n",ifcnt);
    fprintf(stdout, "JUMPIFEQ $$end%i$$ GF@cnt GF@length2\n",ifcnt);//we end if i = j
    fprintf(stdout, "GETCHAR GF@sym1 GF@str1 GF@cnt\n"); //we get char
    fprintf(stdout, "CONCAT GF@str2 GF@str2 GF@sym1\n"); //we put it in final string
    fprintf(stdout, "ADD GF@cnt GF@cnt int@1\n"); 
    fprintf(stdout, "JUMP  $$substring%i$$\n",ifcnt);
    fprintf(stdout, "LABEL $$end%i$$\n",ifcnt);
    fprintf(stdout, "PUSHS GF@str2\n");
    fprintf(stdout, "JUMP  $$skip%i$$\n",ifcnt);
    fprintf(stdout, "LABEL $$null%i$$\n",ifcnt);
    fprintf(stdout, "PUSHS nil@nil\n");
    fprintf(stdout, "LABEL $$skip%i$$\n",ifcnt);
    ifcnt++;
    return; 
  } else if (!strcmp(fcall->name, "ifj.strcmp")) { 
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS GF@str1\n");
    CgenExpr(param->next->expr, true);
    fprintf(stdout, "POPS GF@str2\n");
    fprintf(stdout, "LT GF@inputread GF@str1 GF@str2\n");//here we check all possibilities 
    fprintf(stdout, "JUMPIFEQ $$minus%i$$ GF@inputread bool@true\n",ifcnt);
    fprintf(stdout, "EQ GF@inputread GF@str1 GF@str2\n");//step by step
    fprintf(stdout, "JUMPIFEQ $$zero%i$$ GF@inputread bool@true\n",ifcnt);
    fprintf(stdout, "GT GF@inputread GF@str1 GF@str2\n");
    fprintf(stdout, "JUMPIFEQ $$plus%i$$ GF@inputread bool@true\n",ifcnt);
    fprintf(stdout, "LABEL $$minus%i$$\n",ifcnt);//there is all labels with respectively options
    fprintf(stdout, "PUSHS int@-1\n");
    fprintf(stdout, "JUMP  $$skip%i$$\n",ifcnt);//return value is the same as in a C strcmp
    fprintf(stdout, "LABEL $$plus%i$$\n",ifcnt);
    fprintf(stdout, "PUSHS int@1\n");
    fprintf(stdout, "JUMP  $$skip%i$$\n",ifcnt);
    fprintf(stdout, "LABEL $$zero%i$$\n",ifcnt);
    fprintf(stdout, "PUSHS int@0\n");
    fprintf(stdout, "LABEL $$skip%i$$\n",ifcnt);
    ifcnt++;
    return; 
  }
  if(callfromfunc)fprintf(stdout, "PUSHFRAME\n");//part of funexp extension
  fprintf(stdout, "CREATEFRAME\n");//temp frame for transmission a vars
  int intik = 1;
  while (param != NULL) {//when we call we need to put variables to temp frame so we'll be able to use it in function that is written somewhere below
    fprintf(stdout, "DEFVAR TF@%%%d\n", intik);
    CgenExpr(param->expr, true);
    fprintf(stdout, "POPS TF@%%%d\n", intik);
    fprintf(stdout,"\n");
    intik++;
    param = param->next;
  }
  fprintf(stdout, "CALL $$%s\n", fcall->name);
  if (isdef) {
    fprintf(stdout, "PUSHS GF@%%retval\n");//if this is var = func, we need to push return value so it can be assigned to var
  }
  if(callfromfunc)fprintf(stdout, "POPFRAME\n");
}

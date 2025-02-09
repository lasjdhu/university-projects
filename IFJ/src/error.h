/**
 * @file error.h
 * Project - IFJ Project 2024
 *
 * @author Dias Tursynbayev xtursyd00
 *
 * @brief Error codes and exit
 */

#ifndef ERROR_H
#define ERROR_H

#define OK                    0 // no error
#define LEXICAL_ERROR         1 // lexical error
#define SYNTAX_ERROR          2 // syntax error
#define DEFINITION_ERROR      3 // undefined function or variable
#define PARAMETER_RETURN_ERROR 4 // wrong number or type of parameters or return
#define REDIFINE_ERROR        5 // redefine variable or function or
                                // assignment to non-modifiable variables
#define RETURN_ERROR          6 // missing/overrun expression in return
                                // statement from the function
#define TYPE_ERROR            7 // wrong type of operands in arithmetic,
                                // string or relational expression
#define TYPE_DEDUCTION_ERROR  8 // type of variable or parameter is not
                                // specified and cannot be deduced
#define UNUSED_VAR_ERROR      9 // unused variable in its scope or attempt
                                // to change value of constant
#define OTHER_SEMANTIC_ERROR  10 // other semantic errors
#define INTERNAL_ERROR        99 // internal error - memory allocation, etc.

#endif // ERROR_H

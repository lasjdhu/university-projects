/**
 * @file scanner.c
 * Project - IFJ Project 2024
 *
 * @author Dias Tursynbayev xtursyd00
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief Scanner implementation
 */

#include "scanner.h"
#include "error.h"
#include "jm_string.h"
#include <stdio.h>

// Macro for 1 character token
#define CASE_T(C, T)                                                           \
  case C:                                                                      \
    token->type = T;                                                           \
    return OK

// Macro for changing states
#define CASE_S(C, S)                                                           \
  case C:                                                                      \
    state = S;                                                                 \
    break

// File instance
FILE *file;

// Scanner instance
Scanner *scanner;

/**
 * @brief Get keyword from string
 * @param str String to get keyword
 * @return Keyword or K_UNKNOWN
 */
Keyword GetKeyword(String *str) {
  if (StringEquals(str, "const"))
    return K_CONST;
  if (StringEquals(str, "else"))
    return K_ELSE;
  if (StringEquals(str, "fn"))
    return K_FN;
  if (StringEquals(str, "if"))
    return K_IF;
  if (StringEquals(str, "i32"))
    return K_I32;
  if (StringEquals(str, "f64"))
    return K_F64;
  if (StringEquals(str, "null"))
    return K_NULL;
  if (StringEquals(str, "pub"))
    return K_PUB;
  if (StringEquals(str, "return"))
    return K_RETURN;
  if (StringEquals(str, "u8"))
    return K_U8;
  if (StringEquals(str, "var"))
    return K_VAR;
  if (StringEquals(str, "void"))
    return K_VOID;
  if (StringEquals(str, "while"))
    return K_WHILE;
  if (StringEquals(str, "ifj"))
    return K_IFJ;

  return K_UNKNOWN;
}

/**
 * @brief Get escape code from unprintable char
 * @param c Char to get escape code
 * @return Escape code or NULL
 */
char *GetEscapeCode(char c) {
  if (c >= 1 && c <= 32) {
    char *escape_str = InvokeAlloc(5 * sizeof(char)); // Allocate space for "\000" format
    sprintf(escape_str, "\\%03d", c);
    return escape_str;
  }
  return NULL;
}

/**
 * @brief Get escape sequence from char
 * @return Escape sequence or NULL
 */
char *GetEscapeSequence() {
  char c = getc(file);
  switch (c) {
  case '"':
    return InvokeStrdup("034");
  case 'n':
    return InvokeStrdup("010");
  case 'r':
    return InvokeStrdup("013");
  case 't':
    return InvokeStrdup("009");
  case '\\':
    return InvokeStrdup("092");
  case 'x': {
    char first = getc(file);
    char second = getc(file);

    // Check if first and second char is hex
    if (isxdigit(first) && isxdigit(second)) {
      char hexStr[3] = {first, second, '\0'};
      int hexValue = (int)strtol(hexStr, NULL, 16);
      char *fmtStr = InvokeAlloc(4); // Increased size for decimal ASCII
      sprintf(fmtStr, "%03d", hexValue);
      return fmtStr;
    } else {
      return NULL;
    }
  }
  default:
    return NULL;
  }
}

/**
 * @brief Scanner initialization
 * @param f File
 */
void ScannerInit(FILE *f) {
  file = f;
  scanner = InvokeAlloc(sizeof(Scanner));
  scanner->size = 0;
  scanner->capacity = 100;
  scanner->tokens = InvokeAlloc(sizeof(Token *) * scanner->capacity);
  scanner->current_token = 0;
}

/**
 * @brief Free all tokens and destroy scanner
 */
void ScannerDest() {
  if (file != stdin) {
    fclose(file);
  }

  Token *token = NULL;
  // Free all tokens
  for (size_t i = 0; i < scanner->size; i++) {
    token = scanner->tokens[i];
    // Because of union type it value.string always will be not NULL
    // So we must check like this to free string
    if ((token->type == T_ID && token->keyword == K_UNKNOWN) ||
        token->type == T_STR) {
      FreeString(token->value.string);
    }
    InvokeFree(token);
  }

  InvokeFree(scanner->tokens);
  InvokeFree(scanner);
}

/**
 * @brief Get the next token from the input file
 * @param token Pointer to the token structure to store the result
 * @return Status code (OK or LEXICAL_ERROR)
 */
int GetToken(Token *token) {
  char c;
  String *str = NULL;
  token->value.string = NULL;
  token->keyword = K_UNKNOWN;
  State state = S_START;

  // TODO: S_FLOAT, S_INT to 2-3 parts EBL∀N
  // TODO: S_DIV to 2-3 parts EBL∀N
  // Main scanner loop
  do {
    c = getc(file);
    switch (state) {
    case S_START:
      if (isspace(c)) {
        continue;
      }
      switch (c) {
        CASE_T(EOF, T_EOF);
        CASE_T('.', T_DOT);
        CASE_T(';', T_SEMICOLON);
        CASE_T(':', T_COLON);
        CASE_T(',', T_COMMA);
        CASE_T('(', T_OPAREN);
        CASE_T(')', T_CPAREN);
        CASE_T('[', T_OSQUARE);
        CASE_T(']', T_CSQUARE);
        CASE_T('{', T_OCURLY);
        CASE_T('}', T_CCURLY);
        CASE_T('|', T_PIPE);
        CASE_T('@', T_AT);
        CASE_T('?', T_QUESTM);
        CASE_T('+', T_PLUS);
        CASE_T('-', T_SUB);
        CASE_T('*', T_MUL);

        CASE_S('>', S_GT);
        CASE_S('<', S_LT);
        CASE_S('=', S_EQ);
        CASE_S('!', S_NEQ);
        CASE_S('/', S_DIV);
      case '"': {
        str = StringNew();
        state = S_STR;
        break;
      }
      case '\\': {
        str = StringNew();
        state = S_MLSTR;
        break;
      }

      case '0' ... '9': {
        str = StringNew();
        PushChar(str, c);
        state = S_INT;
        break;
      }

      case '_': {
        str = StringNew();
        PushChar(str, c);
        state = S_UNDERSCORE;
        break;
      }

      case 'a' ... 'z':
      case 'A' ... 'Z':
        str = StringNew();
        PushChar(str, c);
        state = S_ID;
        break;

      default:
        return LEXICAL_ERROR;
      }
      break;
    case S_UNDERSCORE:
      // If char is alphanumeric or underscore
      if (isalnum(c) || c == '_') {
        PushChar(str, c);
        state = S_ID;
      // Else return underscore token
      } else {
        ungetc(c, file);
        token->type = T_UNDERSCORE;
        FreeString(str);
        return OK;
      }
      break;
    case S_ID:
      if (isalnum(c) || c == '_') {
        PushChar(str, c);
      } else {
        ungetc(c, file);
        Keyword keyword = GetKeyword(str);
        if (keyword != K_UNKNOWN) {
          token->type = T_ID;
          token->keyword = keyword;
          FreeString(str);
        } else {
          token->type = T_ID;
          token->value.string = str;
        }
        return OK;
      }
      break;
    // TODO: separate S_INT to 2 states
    case S_INT:
      if (c >= '0' && c <= '9') {
        // If leading zero return lexical error
        if (str->str[0] == '0' && str->length == 1) {
          FreeString(str);
          return LEXICAL_ERROR;
        }
        PushChar(str, c);
      } else if (c == '.') {
        PushChar(str, c);
        state = S_FLOAT;
      } else if (c == 'e' || c == 'E') {
        PushChar(str, c);
        state = S_EXPS;
      } else if (isalpha(c) || c == '_') {
        FreeString(str);
        return LEXICAL_ERROR;
      } else {
        ungetc(c, file);
        token->type = T_INT;
        token->value.integer = atoi(str->str);
        FreeString(str);
        return OK;
      }
      break;
    case S_FLOAT:
      if (c >= '0' && c <= '9') {
        PushChar(str, c);
      } else {
          // If last char is '.' return lexical error
          if (str->str[str->length - 1] == '.') {
              FreeString(str);
              return LEXICAL_ERROR;
          }
          if (c == 'e' || c == 'E') {
              PushChar(str, c);
              state = S_EXPS;
          } else {
              ungetc(c, file);
              token->type = T_FLOAT;
              token->value.real = atof(str->str);
              FreeString(str);
              return OK;
          }
      }
      break;
    case S_EXPS:
      if (c == '+' || c == '-') {
        PushChar(str, c);
      } else if (c >= '1' && c <= '9') {
        PushChar(str, c);
      } else {
        FreeString(str);
        return LEXICAL_ERROR;
      }
      state = S_EXP;
      break;
    case S_EXP:
      if (c >= '1' && c <= '9') {
        PushChar(str, c);
      } else if (c == '.') {
        FreeString(str);
        return LEXICAL_ERROR;
      } else {
        ungetc(c, file);
        token->type = T_FLOAT;
        token->value.real = atof(str->str);
        FreeString(str);
        return OK;
      }
      break;
    case S_GT:
      if (c == '=') {
        token->type = T_GEQ;
        return OK;
      } else {
        ungetc(c, file);
        token->type = T_GT;
        return OK;
      }
    case S_LT:
      if (c == '=') {
        token->type = T_LEQ;
        return OK;
      } else {
        ungetc(c, file);
        token->type = T_LT;
        return OK;
      }
    case S_EQ:
      if (c == '=') {
        token->type = T_EQ;
        return OK;
      } else {
        ungetc(c, file);
        token->type = T_ASSIGN;
        return OK;
      }
    case S_NEQ:
      if (c == '=') {
        token->type = T_NEQ;
        return OK;
      } else {
        ungetc(c, file);
        return LEXICAL_ERROR;
      }
    case S_DIV: {
      if (c == '/') {

        c = getc(file);
        while (c != '\n' && c != EOF) {
          c = getc(file);
        }

        if (c == '\n') {
          ungetc(c, file);
        }
        state = S_START;
        break;
      } else {
        ungetc(c, file);
        token->type = T_DIV;
        return OK;
      }
    }
    case S_STR: {
      if (c == '\n') {
        return LEXICAL_ERROR;
      } else if (c == '"') {
        token->type = T_STR;
        token->value.string = str;
        token->keyword = K_UNKNOWN;
        return OK;
      } else if (c == '\\') {
        char *escape_char = GetEscapeSequence();
        if (escape_char != NULL) {
          PushChar(str, '\\');
          PushString(str, escape_char);
        } else {
          return LEXICAL_ERROR;
        }
      } else if (c == ' ') {
        PushString(str, "\\032");
      } else {
        PushChar(str, c);
      }
      break;
    }

    case S_MLSTR: {
      if (c == '\\') {
        c = getc(file);
        if (c != '\n') {
          while (c != EOF && c != '\n') {
            if (c == '\\') {
              PushString(str, "\\092");
            } else if (c >= 0 && c <= 32) {
              PushString(str, GetEscapeCode(c));
            } else {
              PushChar(str, c);
            }
            c = getc(file);
          }
        }
        PushString(str, "\\010");
      } else {
        return LEXICAL_ERROR;
      }

      while (true) {
        c = getc(file);
        if (isspace(c)) {
          continue;
        } else if (c == '\\') {
          break;
        } else {
          ungetc(c, file);
          token->type = T_STR;
          token->value.string = str;
          token->keyword = K_UNKNOWN;
          return OK;
        }
      }
      break;
    }
    default:
      return LEXICAL_ERROR;
    }
  } while (c != EOF);
  return 0;
}

void GenerateTokens() {
  Token *token = NULL;
  while (true) {
    token = InvokeAlloc(sizeof(Token));
    token->value.string = NULL;
    if (GetToken(token) != 0) {
      InvokeExit(LEXICAL_ERROR);
    }
    scanner->tokens[scanner->size] = token;
    scanner->size += 1;
    if (scanner->size == scanner->capacity - 2) {
      scanner->capacity += 100;
      scanner->tokens =
          InvokeRealloc(scanner->tokens, sizeof(Token *) * scanner->capacity);
    }
    if (token->type == T_EOF)
      break;
    token = NULL;
  }
  scanner->current_token = 0;
}

char *KeywordToString(Keyword keyword) {
  switch (keyword) {
  case K_CONST:
    return "const";
  case K_ELSE:
    return "else";
  case K_FN:
    return "fn";
  case K_IF:
    return "if";
  case K_I32:
    return "i32";
  case K_F64:
    return "f64";
  case K_NULL:
    return "null";
  case K_PUB:
    return "pub";
  case K_RETURN:
    return "return";
  case K_U8:
    return "u8";
  case K_VAR:
    return "var";
  case K_VOID:
    return "void";
  case K_WHILE:
    return "while";
  case K_IFJ:
    return "ifj";
  default:
    return NULL;
  }
}

void PrintToken(const char *key, Token *token) {
  switch (token->type) {
  // case T_COMMENT:
  //     printf("%s: Comment = %s\n", key, token->value.string.str);
  //     break;
  case T_EOF:
    printf("%s: EOF\n", key);
    break;
  case T_INT:
    printf("%s: Integer value = %d\n", key, token->value.integer);
    break;
  case T_FLOAT:
    printf("%s: Float value = %f\n", key, token->value.real);
    break;
  case T_STR:
    printf("%s: String value = %s\n", key, token->value.string->str);
    break;
  case T_ID: {
    const char *keyword_str = KeywordToString(token->keyword);
    if (keyword_str) {
      printf("%s: Keyword = %s\n", key, keyword_str);
    } else {
      printf("%s: Unknown Identifier = %s\n", key, token->value.string->str);
    }
    break;
  }
  case T_DOT:
    printf("%s: Dot\n", key);
    break;
  case T_SEMICOLON:
    printf("%s: Semicolon\n", key);
    break;
  case T_COLON:
    printf("%s: Colon\n", key);
    break;
  case T_COMMA:
    printf("%s: Comma\n", key);
    break;
  case T_OPAREN:
    printf("%s: Open Parenthesis\n", key);
    break;
  case T_CPAREN:
    printf("%s: Close Parenthesis\n", key);
    break;
  case T_OSQUARE:
    printf("%s: Open Square Bracket\n", key);
    break;
  case T_CSQUARE:
    printf("%s: Close Square Bracket\n", key);
    break;
  case T_OCURLY:
    printf("%s: Open Curly Bracket\n", key);
    break;
  case T_CCURLY:
    printf("%s: Close Curly Bracket\n", key);
    break;
  case T_PIPE:
    printf("%s: Pipe\n", key);
    break;
  case T_AT:
    printf("%s: At Symbol\n", key);
    break;
  case T_QUESTM:
    printf("%s: Question Mark\n", key);
    break;
  case T_PLUS:
    printf("%s: Plus\n", key);
    break;
  case T_SUB:
    printf("%s: Subtraction\n", key);
    break;
  case T_MUL:
    printf("%s: Multiplication\n", key);
    break;
  case T_DIV:
    printf("%s: Division\n", key);
    break;
  case T_LT:
    printf("%s: Less Than\n", key);
    break;
  case T_GT:
    printf("%s: Greater Than\n", key);
    break;
  case T_ASSIGN:
    printf("%s: Assignment\n", key);
    break;
  case T_EQ:
    printf("%s: Equals\n", key);
    break;
  case T_NEQ:
    printf("%s: Not Equals\n", key);
    break;
  case T_LEQ:
    printf("%s: Less Than or Equal\n", key);
    break;
  case T_GEQ:
    printf("%s: Greater Than or Equal\n", key);
    break;
  case T_UNDERSCORE:
    printf("%s: Underscore\n", key);
    break;
  default:
    printf("%s: Unknown Token type = %d\n", key, token->type);
  }
}

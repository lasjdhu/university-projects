/**
 * @file jm_string.h
 * Project - IFJ Project 2024
 *
 * @author Dias Tursynbayev xtursyd00
 *
 * @brief Dynamic string implementation header
 */

#ifndef JM_STRING_H
#define JM_STRING_H

#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _string {
  char *str;
  size_t capacity;
  size_t length;
} String;

void FreeString(String *self);
void PushChar(String *self, char c);
void PushString(String *self, const char *str);
String *StringNew(void);
int StringEquals(String *self, const char *other);

#endif // JM_STRING_H

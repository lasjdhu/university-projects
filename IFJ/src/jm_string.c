/**
 * @file jm_string.c
 * Project - IFJ Project 2024
 *
 * @author Dias Tursynbayev xtursyd00
 *
 * @brief Dynamic string implementation
 */

#include "jm_string.h"

/** @brief Allocate new string
 */
String *StringNew(void) {
  String *string = InvokeAlloc(sizeof(String));
  string->str = InvokeAlloc(sizeof(char) * 10);
  string->str[0] = '\0';
  string->capacity = 10;
  string->length = 0;
  return string;
}

/** @brief Push char to string
 * @param self String
 * @param c Char to push
 */
void PushChar(String *self, char c) {
  // Check if string can be extended
  if (self->length == self->capacity - 1) {
    // Double the capacity if needed or set to 1 if string is empty
    self->capacity = self->capacity == 0 ? 1 : self->capacity * 2;

    // Reallocate string for the new capacity
    self->str = InvokeRealloc(self->str, self->capacity);
  }
  // Put new char to string
  self->str[self->length++] = c;

  // Null terminate the string
  self->str[self->length] = '\0';
}

/**
 * @brief Push string to string
 * @param self String
 * @param str String to push
 */
void PushString(String *self, const char *str) {
  // Iterate throgh each char in str
  while (*str) {
    // Push each char to string
    PushChar(self, *str++);
  }
}

/**
 * @brief Free string
 * @param self String
 */
void FreeString(String *self) {
  InvokeFree(self->str);
  InvokeFree(self);
  self = NULL;
}

/**
 * @brief Check if strings are equal
 * @param self String
 * @param other Other string
 * @return True if strings are equal
 */
int StringEquals(String *self, const char *other) {
  // Compare the lengths of the strings and their content
  return (self->length == strlen(other) && strcmp(self->str, other) == 0);
}

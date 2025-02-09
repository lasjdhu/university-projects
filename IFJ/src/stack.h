/**
 * @file stack.h
 * Project - IFJ Project 2024
 *
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief Stack implementation header
 */

#ifndef STACK_H
#define STACK_H

#include "memory.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct stack_item {
  void *data;
  struct stack_item *next;
} stackItem;

typedef struct {
  stackItem *top;
} stack;

void InitStack(stack *s);
int PushStack(stack *s, void *d);
void PopStack(stack *s);
void *TopStack(stack *s);
bool IsEmptyStack(stack *s);
void FreeStack(stack *s);

#endif // STACK_H

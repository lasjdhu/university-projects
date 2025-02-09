/**
 * @file stack.c
 * Project - IFJ Project 2024
 *
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief Stack implementation
 */

#include "stack.h"

/*
 * Initialize stack by setting pointer to the top to NULL
 *
 * @param s - pointer to stack
 *
 * @return
 */
void InitStack(stack *s) { s->top = NULL; }

/*
 * Pushes the pointer to the data into a given stack
 *
 * @param s - pointer to stack
 * @param d - pointer to data
 *
 * @return
 */
int PushStack(stack *s, void *d) {
  stackItem *new_element_ptr = InvokeAlloc(sizeof(stackItem));

  new_element_ptr->data = d;
  new_element_ptr->next = s->top;

  s->top = new_element_ptr;

  return 0;
}

/*
 * Pops top value from stack
 *
 * @param s - pointer to stack
 *
 * @return
 */
void PopStack(stack *s) {
  stackItem *element_ptr;
  if (s->top != NULL) {
    element_ptr = s->top;
    s->top = s->top->next;
    InvokeFree(element_ptr);
  }
}

/*
 * Returns data from the top of the stack
 *
 * @param s - pointer to stack
 *
 * @return pointer to the data on the top
 */
void *TopStack(stack *s) {
  if (s->top != NULL) {
    return s->top->data;
  }
  return NULL;
}

/*
 * Checks if given stack is empty (does not have a pointer to top)
 *
 * @param s - pointer to stack
 *
 * @return bool (is empty)
 */
bool IsEmptyStack(stack *s) { return (s->top == NULL); }

/*
 * Frees stack with every item
 *
 * @param s - pointer to stack
 *
 * @return
 */
void FreeStack(stack *s) {
  // Cycle through tops and pop them
  while (!IsEmptyStack(s)) {
    PopStack(s);
  }
  InvokeFree(s);
}

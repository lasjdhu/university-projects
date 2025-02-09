/**
 * @file array.h
 * Project - IFJ Project 2024
 *
 * @author Albert Tikaiev xtikaia00
 *
 * @brief Dynamic array header file
 */

#ifndef ARRAY_H
#define ARRAY_H

#include "error.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct List {
  void **array;
  size_t size;
  size_t capacity;
} List;

void ArrayInit(List **arr);
void **ArrayAdd(List *arr, void *element);
void ArrayRemove(List *arr, void *element);
void* ArrayFindStr(List *arr, char *element);
void ArrayDest(List *arr);

#endif

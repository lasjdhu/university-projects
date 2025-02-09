/**
 * @file memory.h
 * Project - IFJ Project 2024
 *
 * @author Albert Tikaiev xtikaia00
 *
 * @brief Allocation support header file
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "array.h"
#include "error.h"
#include <string.h>

void AllocatorInit();
void AllocatorDestroy();
void *InvokeAlloc(size_t size);
char *InvokeStrdup(const char *s);
void *InvokeRealloc(void *element, size_t size);
void InvokeFree(void *element);
void InvokeExit(int code);
void InvokeInitVarsArray(List **arr);
void InvokeAddVarsArray(List *arr, void *element);

#endif

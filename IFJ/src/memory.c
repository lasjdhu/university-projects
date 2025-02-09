/**
 * @file memory.c
 * Project - IFJ Project 2024
 *
 * @author Albert Tikaiev xtikaia00
 *
 * @brief Allocation support source file
 */

#include "memory.h"

List *tracking;

/**
 * @brief Initialize array for tracking all memory allocations
 */
void AllocatorInit() { ArrayInit(&tracking); }

/**
 * @brief Free all pointers in dynamic array and free itself
 */
void AllocatorDestroy() {
  if (tracking) {
    ArrayDest(tracking);
  }
}

/**
 * @brief Allocates memory for pointer and add it to tracking list
 *
 * @param size Size of memory that has to be allocated
 *
 * @return Pointer to allocated memory
 */
void *InvokeAlloc(size_t size) {
  if (size <= 0)
    InvokeExit(INTERNAL_ERROR);
  void *element = malloc(size);
  if (element == NULL) {
    InvokeExit(INTERNAL_ERROR);
  }
  ArrayAdd(tracking, element);
  return element;
}

/**
 *  @brief Perform strdup and add it to tracking list
 *
 * @param s Character that has to be strdup
 *
 * @return Pointer from strdup
 */
char *InvokeStrdup(const char *s) {
  char *str = strdup(s);
  if (str == NULL)
    InvokeExit(INTERNAL_ERROR);
  ArrayAdd(tracking, str);
  return str;
}

/**
 *  @brief Reallocates memory for existing pointer
 *
 * @param element Pointer that has to be realloced
 * @param size New size of allocating memory (not additional)
 *
 * @return New reallocated pointer
 */
void *InvokeRealloc(void *element, size_t size) {
  if (size <= 0)
    InvokeExit(INTERNAL_ERROR);
  ArrayRemove(tracking, element);
  void *new_element = realloc(element, size);
  if (element == NULL) {
    InvokeExit(INTERNAL_ERROR);
  }
  //Add new pointer to tracking list
  ArrayAdd(tracking, new_element);
  return new_element;
}

/**
 * @brief Perform free on existing pointer and remove it from tracking list
 *
 * @param element Pointer that has to be freed
 */
void InvokeFree(void *element) {
  if (element) {
    ArrayRemove(tracking, element);
    free(element);
  }
}

/**
 * @brief Function for exiting without memory leaks in error case
 *
 * @param code Exit code from error.h file
 */
void InvokeExit(int code) {
  AllocatorDestroy();
  exit(code);
}

/**
 * @brief Special function for safe memory initializing dynamic array
 * Will contain variable names for code generation
 *
 * @param arr dynamic array of variables
 */
void InvokeInitVarsArray(List **arr) {
  ArrayInit(arr);
  ArrayAdd(tracking, (*arr));
  ArrayAdd(tracking, (*arr)->array);
}

/**
 * @brief Special function for safe memory adding element to dynamic array
 *
 * @param arr dynamic array of variables
 * @param element name of variable
 */
void InvokeAddVarsArray(List *arr, void *element) {
  if(ArrayFindStr(arr, element) != NULL) return;
  void **temp = arr->array;
  ArrayAdd(arr, element);
  //in case realloc changed pointer
  if (arr->array != temp) {
    ArrayRemove(tracking, temp);
    ArrayAdd(tracking, arr->array);
    temp = NULL;
  }
}
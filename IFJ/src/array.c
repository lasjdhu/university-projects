/**
 * @file array.c
 * Project - IFJ Project 2024
 *
 * @author Albert Tikaiev xtikaia00
 *
 * @brief Dynamic array implementation source file
 */

#include "array.h"
#include "memory.h"

/**
 * @brief Initialize and allocate new dynamic array
 *
 * @param arr dynamic array that has to be initialized
 */
void ArrayInit(List **arr) {
  *arr = malloc(sizeof(List));
  if (*arr == NULL) {
    InvokeExit(INTERNAL_ERROR);
  }
  (*arr)->array = malloc(sizeof(void *) * 10);
  if ((*arr)->array == NULL) {
    InvokeExit(INTERNAL_ERROR);
  }
  (*arr)->capacity = 10;
  (*arr)->size = 0;
}

/**
 * @brief Adding new element to dynamic array
 *
 * @param arr dynamic array
 * @param element element that has to be added
 *
 * @return pointer to array inside dynamic array structure
 */
void **ArrayAdd(List *arr, void *element) {
  if (arr) {
    arr->array[arr->size] = element;
    arr->size++;

    if (arr->size == arr->capacity - 1) {
      arr->capacity = arr->capacity * 2;
      arr->array = realloc(arr->array, sizeof(void *) * arr->capacity);
      if (arr->array == NULL) {
        InvokeExit(INTERNAL_ERROR);
      }
      return arr->array;
    }
    return arr->array;
  }
  return NULL;
}

/**
 * @brief Adding existing element from dynamic array
 *
 * @param arr dynamic array
 * @param element element that has to be removed
 */
void ArrayRemove(List *arr, void *element) {
  if (arr) {
    for (size_t i = 0; i < arr->size; i++) {
      if (arr->array[i] == NULL)
        continue;

      if (arr->array[i] == element) {
        for (size_t k = i; k < arr->size - 1; k++) {
          arr->array[k] = arr->array[k + 1];
        }
        arr->array[arr->size - 1] = NULL;
        arr->size--;
        return;
      }
    }
  }
}

/**
 * @brief Special function for dynamic array of variable names
 *
 * @param arr dynamic array
 * @param element element that has to find(as a string)
 *
 * @return pointer of found element or NULL
 */
void* ArrayFindStr(List *arr, char *element) {
  if (arr) {
    for (size_t i = 0; i < arr->size; i++) {
      if (arr->array[i] == NULL)
        continue;

      if (strcmp(arr->array[i], element) == 0)
        return arr->array[i];
    }
  }
  return NULL;
}

/**
 * @brief Free every element and free itself
 *
 * @param arr dynamic array
 */
void ArrayDest(List *arr) {
  if (arr) {

    for (size_t i = 0; i < arr->size; i++) {
      if (arr->array[i] != NULL)
        free(arr->array[i]);
    }

    free(arr->array);
    free(arr);
  }
}

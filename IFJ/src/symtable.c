/**
 * @file symtable.c
 * Project - IFJ Project 2024
 *
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief symbolic table functions implementation,
 * variant of hashtable with open addressing
 */

#include "symtable.h"

// This symtable will be accessed from outside
// only via functions defined in this file
static symtable *global_table = NULL;

/**
 * Linear probing function to resolve collisions
 *
 * @param index - current hashed index
 * @param step - current step in the probe sequence
 *
 * @return unsigned int new hash index
 */
unsigned int LinearProbe(int index, int step) {
  return (index + C * step) % MAX_SIZE;
}

/**
 * Initialize the symtable
 *
 * @return
 */
void SymtableInit() {
  global_table = InvokeAlloc(sizeof(symtable));
  for (int i = 0; i < MAX_SIZE; i++) {
    (*global_table)[i] = NULL;
  }
}

/**
 * Create a pointer to the item which will be filled later
 *
 * @param key - key string
 * @param type - type of item that will be added
 *
 * @return symtable_item_data* pointer to the item data that will be added
 */
symtable_item_data *SymtableAdd(char *key, SymbolType type) {
  // Generate hash from key
  unsigned int index = SymtableHash(key);

  // Linear probing to handle collisions if current index is already busy
  int i = 1;
  while ((*global_table)[index] != NULL && (*global_table)[index]->busy) {
    index = LinearProbe(index, i);
    i++;
  }

  // Allocate memory for new item
  if ((*global_table)[index] == NULL) {
    (*global_table)[index] = InvokeAlloc(sizeof(symtable_item));
    (*global_table)[index]->data = InvokeAlloc(sizeof(symtable_item_data));
  }

  (*global_table)[index]->key = InvokeStrdup(key); // Copy key
  (*global_table)[index]->busy = true;             // Set busy
  // Fill data
  symtable_item_data *data = (*global_table)[index]->data;
  data->name = key;     // Use keys as names
  data->symType = type; // Define the type of item to be added right away
  // Based on the type fill data with initial values
  if (type == ST_FUNC) {
    InvokeInitVarsArray(&data->details.func.params);
    InvokeInitVarsArray(&data->details.func.variables);
    data->details.func.returnMade = false;
  } else if (type == ST_VAR_VAR || type == ST_VAR_CONST) {
    data->details.var.scope = 0;
    data->details.var.type = ST_NOT_DEFINED;
    data->details.var.isUsed = false;
    data->details.var.isModificationUsed = type == ST_VAR_CONST ? true : false;
    data->details.var.isKnown = false;
  }

  return (*global_table)[index]->data;
}

/**
 * Remove an item based on a key from the symtable
 *
 * @param key - key that defines which item will be removed
 *
 * @return
 */
void SymtableRemove(char *key) {
  // Generate hash from key
  unsigned int index = SymtableHash(key);

  // Free item
  int i = 0;
  while ((*global_table)[index] != NULL) {
    if ((*global_table)[index]->busy &&
        strcmp((*global_table)[index]->key, key) == 0) {
      InvokeFree((*global_table)[index]->key);
      InvokeFree((*global_table)[index]->data);
      InvokeFree((*global_table)[index]);
      (*global_table)[index] = NULL;
      return;
    }

    // Linear probing to handle collisions if current index is already busy
    // and keys are not same
    index = LinearProbe(index, i);
    i++;
  }
}

/**
 * Clear the symtable
 *
 * @return
 */
void SymtableClear() {
  // Free whole table with every item
  if (global_table != NULL) {
    for (int i = 0; i < MAX_SIZE; i++) {
      if ((*global_table)[i] != NULL && (*global_table)[i]->busy) {
        InvokeFree((*global_table)[i]->key);
        InvokeFree((*global_table)[i]->data);
        InvokeFree((*global_table)[i]);
        (*global_table)[i] = NULL;
      }
    }

    InvokeFree(global_table);
    global_table = NULL;
  }
}

/**
 * Find a token by key in the symtable.
 *
 * @param key pointer to a key which will be searched for
 *
 * @return symtable_item_data* pointer to the found item OR NULL!
 */
symtable_item_data *SymtableFind(char *key) {
  // Generate hash from key
  unsigned int index = SymtableHash(key);

  int i = 1;
  while ((*global_table)[index] != NULL) {
    if ((*global_table)[index]->busy &&
        strcmp((*global_table)[index]->key, key) == 0) {
      return (*global_table)[index]->data;
    } else {
      // Linear probing to handle collisions if current index is already busy
      // and keys are not same
      index = LinearProbe(index, i);
    }
    i++;
  }

  return NULL;
}

/**
 * DJB hash function for generating a hash value from a string
 *
 * @param key key string to hash
 *
 * @return unsigned int hash value
 */
unsigned int SymtableHash(char *key) {
  unsigned int hash = 5381;
  int c;

  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash % MAX_SIZE;
}

/**
 * Enter a new scope in symtable
 *
 * @return
 */
void SymtableEnterScope() {
  // For every existing item (vars and consts)
  for (int i = 0; i < MAX_SIZE; i++) {
    if ((*global_table)[i] != NULL && (*global_table)[i]->busy) {
      symtable_item_data *data = (*global_table)[i]->data;

      if (data->symType == ST_VAR_VAR || data->symType == ST_VAR_CONST) {
        // Increase scope counter
        data->details.var.scope++;
      }
    }
  }
}

/**
 * Leave the current scope, removing variables or decreasing scope counter
 *
 * @return
 */
void SymtableLeaveScope() {
  // For every existing item (vars and consts)
  for (int i = 0; i < MAX_SIZE; i++) {
    if ((*global_table)[i] != NULL && (*global_table)[i]->busy) {
      symtable_item_data *data = (*global_table)[i]->data;

      if (data->symType == ST_VAR_VAR || data->symType == ST_VAR_CONST) {
        if (data->details.var.scope == 0) {
          if (!data->details.var.isUsed) {
            InvokeExit(UNUSED_VAR_ERROR);
          }

          if (data->symType == ST_VAR_VAR &&
              !data->details.var.isModificationUsed) {
            InvokeExit(UNUSED_VAR_ERROR);
          }

          // Remove variable token from symtable if there is no error
          char *key_to_remove = (*global_table)[i]->key;
          SymtableRemove(key_to_remove);
        } else {
          // Derease scope counter if is not 0 yet
          data->details.var.scope--;
        }
      }
    }
  }
}

/**
 * Checks if item located under given key exists and a function
 *
 * @return symtable_item_data function data
 */
symtable_item_data *SymtableAssertFunction(char *key) {
  symtable_item_data *data = SymtableFind(key);

  if (data == NULL) {
    InvokeExit(DEFINITION_ERROR);
  }

  if (data->symType != ST_FUNC) {
    InvokeExit(DEFINITION_ERROR);
  }

  return data;
}

/**
 * Checks if item located under given key exists and a variable (or constant)
 *
 * @return symtable_item_data function data
 */
symtable_item_data *SymtableAssertVariable(char *key) {
  symtable_item_data *data = SymtableFind(key);

  if (data == NULL) {
    InvokeExit(DEFINITION_ERROR);
  }

  if (data->symType != ST_VAR_VAR && data->symType != ST_VAR_CONST) {
    InvokeExit(DEFINITION_ERROR);
  }

  return data;
}

/**
 * Toggles isUsed property if variable (or constant) exists
 *
 * @return symtable_item_data function data
 */
symtable_item_data *SymtableUpdate_isUsed(char *key) {
  symtable_item_data *data = SymtableFind(key);

  if (data == NULL) {
    InvokeExit(DEFINITION_ERROR);
  }

  if (data->symType != ST_VAR_VAR && data->symType != ST_VAR_CONST) {
    InvokeExit(DEFINITION_ERROR);
  }

  data->details.var.isUsed = true;
  return data;
}

/**
 * Toggles isModified property if variable exists
 *
 * @return symtable_item_data function data
 */
symtable_item_data *SymtableUpdate_isModified(char *key) {
  symtable_item_data *data = SymtableFind(key);

  if (data == NULL) {
    InvokeExit(DEFINITION_ERROR);
  }

  if (data->symType == ST_FUNC) {
    InvokeExit(DEFINITION_ERROR);
  }

  if (data->symType == ST_VAR_CONST) {
    InvokeExit(REDIFINE_ERROR);
  }

  data->details.var.isModificationUsed = true;
  return data;
}

/**
 * Checks if every function in symtable made a return
 *
 * @return
 */
void SymtableAssertReturns() {
  // For every existing item (funcs)
  for (int i = 0; i < MAX_SIZE; i++) {
    if ((*global_table)[i] != NULL && (*global_table)[i]->busy) {
      symtable_item_data *data = (*global_table)[i]->data;

      if (data->symType == ST_FUNC) {
        // Check if return is made
        if (!data->details.func.returnMade)
          InvokeExit(RETURN_ERROR);
      }
    }
  }
}

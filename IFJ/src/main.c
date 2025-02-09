/**
 * @file main.c
 * Project - IFJ Project 2024
 *
 * @author Dmitrii Ivanushkin xivanu00
 *
 * @brief STDIN handler
 */

#include "memory.h"
#include "parser.h"
#include "scanner.h"
#include <stdio.h>

int main() {

  FILE *file = stdin;
  AllocatorInit();
  ScannerInit(file);
  GenerateTokens();
  Parse();
  ScannerDest();
  AllocatorDestroy();

  return 0;
}

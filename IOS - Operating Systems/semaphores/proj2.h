/*
 * Soubor: proj2.h (hlavičkový soubor)
 * Autor: xivanu00 Dmitrii Ivanushkin
 * Duben 2024
 */

#ifndef PROJ2_H
#define PROJ2_H

#include<stdio.h>
#include<stdbool.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

#define BOARDED -1
#define UNBOARDED -2

FILE *f; // Výstupní soubor

int L; // Počet lyžařů
int Z; // Počet nástupních zástavek
int K; // Kapacita skibusu
int TL; // Maximální čas v mikrosekundách, který lyžař čeká, než přijde na zastávku
int TB; // Maximální doba jízdy autobusu mezi dvěma zastávkami

int *c; // Čítač
bool *skiersWaiting; // Lyžař čeká
int *idZ; // id zastávky pro lyžaře (pole)
sem_t *writing; // Semafor pro zápis
sem_t *final; // Semafor pro oznámení finalní zastávky (pole)
sem_t *skierUnboarded; // Semafor pro oznámení, že lyžař vystoupil
sem_t *skierBoarded; // Semafor pro oznámení, že lyžař nastoupil
sem_t *skibusArrived; // Semafor pro oznámení, že skibus dorazil na zastávku pro správného lyžaře (pole)

/*
 * Načtení argumentů
 * @param argc Počet argumentů
 * @param argv Argumenty
 */
void getArgs(int argc, char *argv[]);

/*
 * Kontrola argumentů
 */
void checkArgs();

/*
 * Vypsání chybového stavu
 * @param message Chybový stav
 */
void throwError(char *message);

/*
 * Inicializace semaforů
 */
void initSemaphores();

/*
 * Uvolnění semaforů z paměti
 */
void freeSemaphores();

/*
 * Proces Skibus
 */
void skibusProcess();

/*
 * Proces Lyžař
 * @param idL id lyžaře
 */
void skierProcess(int idL);

/*
 * Cyklus skibusu (arrive -- leave)
 */
void skibusCycle();

/*
 * Checks if any skiers are waiting for the bus after the bus arrives to final
 */
void checkIfSkiersWaiting();

#endif

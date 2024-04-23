/*
 * Soubor: proj2.c (hlavní soubor)
 * Autor: xivanu00 Dmitrii Ivanushkin
 * Duben 2024
 */

#include "proj2.h"

int main(int argc, char *argv[]) {
  getArgs(argc, argv);
  checkArgs();

  f = fopen("proj2.out", "w");
  if (f == NULL) {
    throwError("Chyba při otevírání souboru");
  }

  initSemaphores();

  pid_t skibus_pid = fork();
  if (skibus_pid < 0) {
    throwError("Chyba při volání fork");
  } else if (skibus_pid == 0) {
    skibusProcess();
  }

  for (int i = 0; i < L; i++) {
    pid_t skier_pid = fork();
    if (skier_pid < 0) {
      throwError("Chyba při volání fork");
    } else if (skier_pid == 0) {
      skierProcess(i);
    }
  }

  while (wait(NULL) > 0);

  freeSemaphores();
  fclose(f);

  return 0;
}

void skibusCycle() {
  int currStop = 1;
  int currBoarded = 0;

  while (currStop <= Z) {
    usleep((rand() % TB) * 1000);

    sem_wait(writing);
    fprintf(f, "%d: BUS: arrived to %d\n", (*c)++, currStop);
    fflush(f);
    sem_post(writing);

    sem_wait(mutex);
    for (int i = 0; i < L; i++) {
      if (currStop == idZ[i] && idZ[i] != BOARDED && currBoarded < K) {
        currBoarded++;
        sem_post(&skibusArrived[i]);
        sem_wait(skierBoarded);
      }
    }
    sem_post(mutex);

    sem_wait(writing);
    fprintf(f, "%d: BUS: leaving %d\n", (*c)++, currStop);
    fflush(f);
    sem_post(writing);

    currStop++;
  }

  if (currStop - Z == 1) {
    usleep((rand() % TB) * 1000);
  
    sem_wait(writing);
    fprintf(f, "%d: BUS: arrived to final\n", (*c)++);
    fflush(f);
    sem_post(writing);

    sem_wait(mutex);
    for (int i = 0; i < L; i++) {
      if (idZ[i] == BOARDED && idZ[i] != UNBOARDED) {
        sem_post(final);
        sem_wait(skierUnboarded);
      }
    }
    sem_post(mutex);

    sem_wait(writing);
    fprintf(f, "%d: BUS: leaving final\n", (*c)++);
    fflush(f);
    sem_post(writing);

    checkIfSkiersWaiting();
  }
}

void skibusProcess() {
  srand(getpid());

  sem_wait(writing);
  fprintf(f, "%d: BUS: started\n", (*c)++);
  fflush(f);
  sem_post(writing);

  while (*skiersWaiting) {
    skibusCycle();
  }

  sem_wait(writing);
  fprintf(f, "%d: BUS: finish\n", (*c)++);
  fflush(f);
  sem_post(writing);

  exit(0);
}

void skierProcess(int idL) {
  srand(getpid() * idL);

  sem_wait(writing);
  fprintf(f, "%d: L %d: started\n", (*c)++, idL + 1);
  fflush(f);
  sem_post(writing);

  usleep((rand() % TL) * 1000);
  idZ[idL] = rand() % Z + 1;

  sem_wait(writing);
  fprintf(f, "%d: L %d: arrived to %d\n", (*c)++, idL + 1, idZ[idL]);
  fflush(f);
  sem_post(writing);

  sem_wait(&skibusArrived[idL]);

  sem_wait(writing);
  fprintf(f, "%d: L %d: boarding\n", (*c)++, idL + 1);
  fflush(f);
  sem_post(writing);

  idZ[idL] = BOARDED;
  sem_post(skierBoarded);

  sem_wait(final);

  sem_wait(writing);
  fprintf(f, "%d: L %d: going to ski\n", (*c)++, idL + 1);
  fflush(f);
  sem_post(writing);

  idZ[idL] = UNBOARDED;
  sem_post(skierUnboarded);

  exit(0);
}

void checkIfSkiersWaiting() {
  for (int i = 0; i < L; i++) {
    if (idZ[i] != UNBOARDED) {
      return;
    }
  }

  *skiersWaiting = false;
}

void initSemaphores() {
  c = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  skiersWaiting = mmap(NULL, sizeof(bool), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  idZ = mmap(NULL, L * sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  mutex = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  writing = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  final = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  skierBoarded = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  skierUnboarded = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  skibusArrived = mmap(NULL, L * sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);

  if (c == MAP_FAILED ||
    skiersWaiting == MAP_FAILED ||
    idZ == MAP_FAILED ||
    mutex == MAP_FAILED ||
    writing == MAP_FAILED ||
    final == MAP_FAILED ||
    skierBoarded == MAP_FAILED ||
    skierUnboarded == MAP_FAILED ||
    skibusArrived == MAP_FAILED
  ) {
    throwError("Chyba při vytváření semaforu writing");
  }

  *c = 1;
  *skiersWaiting = true;
  for (int i = 0; i < L; i++) {
    idZ[i] = 0;
  }
  sem_init(mutex, 1, 1);
  sem_init(writing, 1, 1);
  sem_init(final, 1, 0);
  sem_init(skierBoarded, 1, 0);
  sem_init(skierUnboarded, 1, 0);
  for (int i = 0; i < L; i++) {
    sem_init(&skibusArrived[i], 1, 0);
  }
}

void freeSemaphores() {
  sem_destroy(mutex);
  sem_destroy(writing);
  sem_destroy(final);
  sem_destroy(skierBoarded);
  sem_destroy(skierUnboarded);
  sem_destroy(skibusArrived);

  munmap(c, sizeof(int));
  munmap(skiersWaiting, sizeof(bool));
  munmap(idZ, L * sizeof(int));
  munmap(mutex, sizeof(sem_t));
  munmap(writing, sizeof(sem_t));
  munmap(final, sizeof(sem_t));
  munmap(skierBoarded, sizeof(sem_t));
  munmap(skierUnboarded, sizeof(sem_t));
  munmap(skibusArrived, L * sizeof(sem_t));
}

void getArgs(int argc, char *argv[]) {
  if (argc != 6) {
    throwError("Chybný počet argumentů");
  }

  L = atoi(argv[1]);
  Z = atoi(argv[2]);
  K = atoi(argv[3]);
  TL = atoi(argv[4]);
  TB = atoi(argv[5]);
}

void checkArgs() {
  if (L >= 20000) {
    throwError("L (počet lyžařů) musí být < 20000");
  }
  if ((Z <= 0) || (Z > 10)) {
    throwError("Z (počet nástupních zástavek) musí být 0 < Z <= 10");
  }
  if ((K < 10) || (K > 100)) {
    throwError("K (kapacita skibusu) musí být 10 <= K <= 100");
  }
  if ((TL < 0) || (TL > 10000)) {
    throwError("TL (maximální čas v mikrosekundách, který lyžař čeká, než přijde na zastávku) musí být 0 <= TL <= 10000");
  }
  if ((TB < 0) || (TB > 1000)) {
    throwError("TB (maximální doba jízdy autobusu mezi dvěma zastávkami) musí být 0 <= TB <= 1000");
  }
}

void throwError(char *message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}
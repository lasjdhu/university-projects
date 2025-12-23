# Semaphores in C

## April 2024

### Description

There are 3 types of processes in the system: (0) main process, (1) ski bus and
(2) skier. Each skier goes after breakfast to one of the ski bus stops where he
waits for the bus to arrive. After the bus arrives at the boarding point, the
ski bus stops and the skiers board. If the bus is full, the remaining skiers
wait for the next bus. The bus will serve all boarding stops and take the skiers
to the exit stop at the cable car. If there are still more skiers interested in
a ride, bus continues with another round.

### Usage

```sh
make
./proj2 L Z K TL TB
```

- L: number of skiers, L < 20000
- Z: number of boarding stops, 0 < Z <= 10
- K: ski bus capacity, 10 <= K <= 100
- TL: maximum time in microseconds a skier waits before arriving at a stop, 0 <=
  TL <= 10000
- TB: Maximum time for the bus to travel between two stops. 0 <= TB <= 1000

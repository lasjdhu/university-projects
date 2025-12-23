/*
 * IMS Project
 * Author: xivanu00
 */
#include "simlib.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

double DISTANCE_ONE_WAY = 6.0;
int TRACTOR_COUNT = 2;
double TRAILER_CAPACITY_M3 = 21.0;

const double FIELD_SIZE_HA = 50.0;
const double YIELD_PER_HA = 6.5;
const double WHEAT_DENSITY = 0.78;
const double HARVESTER_TANK_M3 = 9.5;
const double HARVESTER_UNLOAD_RATE = 0.125 * 60.0;

const double SPEED_HARVEST_MIN = 4.0;
const double SPEED_HARVEST_MAX = 8.0;
const double SPEED_TRACTOR_FIELD = 10.0;
const double SPEED_TRACTOR_ROAD = 35.0;
const double HEADER_WIDTH = 0.0067;

Histogram *HistHarvesterWait = nullptr;
Facility *SiloPit = nullptr;
Queue *TractorQueue = nullptr;

Process *ptrHarvester = nullptr;
bool FieldHarvested = false;

class Tractor : public Process {
  double capacity_m3;
  double current_load_m3;

public:
  Tractor(double cap) : Process(), capacity_m3(cap), current_load_m3(0) {}

  void Behavior() override {
    while (!FieldHarvested) {
      Into(*TractorQueue);

      if (ptrHarvester && ptrHarvester->Idle()) {
        ptrHarvester->Activate();
      }

      Passivate();

      if (FieldHarvested && current_load_m3 == 0)
        break;

      current_load_m3 += HARVESTER_TANK_M3;

      if ((current_load_m3 + HARVESTER_TANK_M3 <= capacity_m3) &&
          !FieldHarvested) {
        continue;
      }

      double travel_time =
          (0.5 / SPEED_TRACTOR_FIELD) + (DISTANCE_ONE_WAY / SPEED_TRACTOR_ROAD);
      Wait(travel_time * 60);

      Seize(*SiloPit);
      Wait(Uniform(3, 5));
      Release(*SiloPit);

      current_load_m3 = 0;

      Wait(travel_time * 60);
    }
  }
};

class Harvester : public Process {
  double total_grain_m3;
  double harvested_m3;

public:
  Harvester() : Process() {
    total_grain_m3 = (FIELD_SIZE_HA * YIELD_PER_HA) / WHEAT_DENSITY;
    harvested_m3 = 0;
    ptrHarvester = this;
  }

  void Behavior() override {
    double current_tank_m3 = 0;

    while (harvested_m3 < total_grain_m3) {
      double speed = Uniform(SPEED_HARVEST_MIN, SPEED_HARVEST_MAX);
      double area_per_hour = speed * HEADER_WIDTH * 100;
      double grain_per_hour_m3 = (area_per_hour * YIELD_PER_HA) / WHEAT_DENSITY;
      double time_to_fill = (HARVESTER_TANK_M3 / grain_per_hour_m3) * 60;

      Wait(time_to_fill);

      current_tank_m3 = HARVESTER_TANK_M3;
      harvested_m3 += current_tank_m3;

      if (TractorQueue->Empty()) {
        double wait_start = Time;
        Passivate();
        double duration = Time - wait_start;
        (*HistHarvesterWait)(duration);
      }

      Tractor *t = (Tractor *)TractorQueue->GetFirst();

      double transfer_time = current_tank_m3 / HARVESTER_UNLOAD_RATE;
      Wait(transfer_time);

      if (t)
        t->Activate();

      current_tank_m3 = 0;
    }

    FieldHarvested = true;

    while (!TractorQueue->Empty()) {
      Tractor *t = (Tractor *)TractorQueue->GetFirst();
      t->Activate();
    }

    Stop();
  }
};

void RunExperiment(double distance, int tractors, double capacity,
                   std::string label, int seed) {
  Init(0);
  RandomSeed(seed);

  DISTANCE_ONE_WAY = distance;
  TRACTOR_COUNT = tractors;
  TRAILER_CAPACITY_M3 = capacity;
  FieldHarvested = false;

  if (HistHarvesterWait)
    delete HistHarvesterWait;
  if (SiloPit)
    delete SiloPit;
  if (TractorQueue)
    delete TractorQueue;

  HistHarvesterWait = new Histogram("Harvester Wait Hist", 0, 5, 20);
  SiloPit = new Facility("Silo Pit");
  TractorQueue = new Queue("Idle Tractors");

  for (int i = 0; i < TRACTOR_COUNT; i++)
    (new Tractor(TRAILER_CAPACITY_M3))->Activate();
  (new Harvester)->Activate();

  Run();

  double avg_wait = 0.0;
  if (HistHarvesterWait->stat.Number() > 0) {
    avg_wait = HistHarvesterWait->stat.MeanValue();
  }

  std::cout << std::left << std::setw(15) << label << std::setw(10) << distance
            << std::setw(10) << tractors << std::setw(10) << capacity
            << std::setw(15) << std::fixed << std::setprecision(2)
            << (Time / 60.0) << std::setw(15) << std::fixed
            << std::setprecision(2) << avg_wait << std::endl;
}

int main() {
  std::cout << "==============================================================="
               "==============="
            << std::endl;
  std::cout << std::left << std::setw(15) << "EXP ID" << std::setw(10)
            << "DIST(km)" << std::setw(10) << "TRUCKS" << std::setw(10)
            << "CAP(m3)" << std::setw(15) << "TIME(h)" << std::setw(15)
            << "AVG_WAIT(min)" << std::endl;
  std::cout << "==============================================================="
               "==============="
            << std::endl;

  long base_seed = time(nullptr);

  // --- EXPERIMENT 1 ---
  RunExperiment(2.0, 2, 21.0, "Exp1_2km", base_seed + 1);

  std::cout << "---------------------------------------------------------------"
               "---------------"
            << std::endl;

  // --- EXPERIMENT 2 ---
  RunExperiment(2.0, 2, 21.0, "Exp2_2km", base_seed + 2);
  RunExperiment(4.0, 2, 21.0, "Exp2_4km", base_seed + 3);
  RunExperiment(6.0, 2, 21.0, "Exp2_6km", base_seed + 4);
  RunExperiment(8.0, 2, 21.0, "Exp2_8km", base_seed + 5);

  std::cout << "---------------------------------------------------------------"
               "---------------"
            << std::endl;

  // --- EXPERIMENT 3 ---
  RunExperiment(10.0, 2, 21.0, "Exp3_Default", base_seed + 7);
  RunExperiment(10.0, 3, 21.0, "Exp3_VarA", base_seed + 8);
  RunExperiment(10.0, 2, 33.0, "Exp3_VarB", base_seed + 9);

  std::cout << "==============================================================="
               "==============="
            << std::endl;

  if (HistHarvesterWait)
    delete HistHarvesterWait;
  if (SiloPit)
    delete SiloPit;
  if (TractorQueue)
    delete TractorQueue;

  return 0;
}

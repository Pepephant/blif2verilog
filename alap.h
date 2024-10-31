#pragma once
#include "blif2verilog.h"
void alap(vector<cell *> &cells);
void print_schedule(map<cell *, int> schedule);
void print_cycle(map<string, set<string>> OpTable);
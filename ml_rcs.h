#pragma once
#include "blif2verilog.h"
#include <unordered_set>

void ML_RCS(cell *head);
int getNumber(cell *c, std::unordered_set<cell *> &set, vector<cell *> &vec);

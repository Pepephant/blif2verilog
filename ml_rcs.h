#pragma once
#include "blif2verilog.h"
#include <unordered_set>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>

using std::map;
using std::max;
using std::min;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

// void ML_RCS(cell *head);
int ML_RCS_enumerate(cell *head, map<string, int> constrain);

int getNumber(cell *c, std::unordered_set<cell *> &set, vector<cell *> &vec);

//
// Created by Pepephant on 2024/10/28.
//

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iterator>
#include "blif2verilog.h"
#include "alap.h"

using namespace std; 

map<cell *, int> alap(cell* hnop)
{
    int lambda = 0;
    std::set<cell*> cells;
    map<cell *, int> schedule;
    queue<cell *> readyQueue;

    collect(hnop, cells);

    // Schedule the initial node at t0 = 0
    for (auto &c : cells)
    {
        if (c->getNext().empty())
        {
            schedule[c] = 0;
            readyQueue.push(c);
        }
    }

    while (!readyQueue.empty())
    {
        cell *current = readyQueue.front();
        readyQueue.pop();

        for (auto &prev : current->getPrev())
        {
            bool allSuccessorsScheduled = true;
            int minTime = 0;

            for (auto &next : prev->getNext())
            {
                if (schedule.find(next) == schedule.end())
                {
                    allSuccessorsScheduled = false;
                    break;
                }
                minTime = min(minTime, schedule[next] - 1); // Assuming unit delay (dj = 1)
            }

            if (allSuccessorsScheduled && schedule.find(prev) == schedule.end())
            {
                if (prev->getName() == "*") {
                    schedule[prev] = minTime + 1;
                } else {
                    schedule[prev] = minTime;
                }
                readyQueue.push(prev);
            } 

            lambda = min(lambda, minTime);
        }
    }

    for (auto& s: schedule) {
        s.second -= lambda;
    }

    return schedule;
}

void print_schedule(map<cell *, int> schedule) {
    map<int, set<cell*>> timetable;
    for (auto& s: schedule) {
        if (s.first->getName() == "*") {
            if (s.first->getOp() == "!" && s.first->getNext().size() == 1) {
                // Do nothing
            } else {
                continue;
            }
        }
        timetable[s.second].insert(s.first);
    }

    for (auto& t: timetable) {
        map<string, set<string>> OpTable;
        for (auto& c: t.second) {
            if (c->getName() == "*") {
                OpTable[c->getOp()].insert(c->getNext()[0]->getName());
            } else {
                OpTable[c->getOp()].insert(c->getName());
            }
        }
        for (auto& it: OpTable["!"]) {
            OpTable["&"].erase(it);
        }
        cout << "Cycle " << t.first << ": ";
        print_cycle(OpTable);
    }
}

void print_cycle(map<string, set<string>> OpTable) {
    auto set1 = OpTable["&"];
    auto set2 = OpTable["|"];
    auto set3 = OpTable["!"];
    std::cout << "{";
    for (auto& it: set1) {
        cout << it << " ";
    }
    std::cout << "}, {";
    for (auto& it: set2) {
        cout << it << " ";
    }
    std::cout << "}, {";
    for (auto& it: set3) {
        cout << it << " ";
    }
    std::cout << "}\n";
}

int main()
{
    string blif_file = "./test1.blif";
    cell * hnop = vtog(blif_file);
    auto schedule = alap(hnop);
    print_schedule(schedule);
    destory(hnop);
    return 0;
}
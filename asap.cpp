//
// Created by Amazon Panda on 2024/10/18.
//

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include "blif2verilog.h"

using namespace std;

// class cell
// {
// private:
//     vector<cell *> prev;
//     vector<cell *> next;
//     string op;
//     bool isnop;
//     string name;

// public:
//     cell(string name, string op, bool isnop = false) : name(name), op(op), isnop(isnop) {}
//     cell() {}
//     ~cell() {}

//     void addPrev(cell *prevCell)
//     {
//         prev.push_back(prevCell);
//     }

//     void addNext(cell *nextCell)
//     {
//         next.push_back(nextCell);
//     }

//     vector<cell *> getPrev()
//     {
//         return prev;
//     }

//     vector<cell *> getNext()
//     {
//         return next;
//     }

//     string getOp()
//     {
//         return op;
//     }

//     string getName()
//     {
//         return name;
//     }

//     bool getIsnop()
//     {
//         return isnop;
//     }
// };

void asap(vector<cell *> &cells)
{
    unordered_map<cell *, int> schedule;
    queue<cell *> readyQueue;

    // Schedule the initial node at t0 = 0
    for (auto &c : cells)
    {
        if (c->getPrev().empty())
        {
            schedule[c] = 0;
            readyQueue.push(c);
        }
    }

    // Perform the ASAP scheduling
    while (!readyQueue.empty())
    {
        cell *current = readyQueue.front();
        readyQueue.pop();

        for (auto &next : current->getNext())
        {
            bool allPredecessorsScheduled = true;
            int maxTime = 0;

            for (auto &pred : next->getPrev())
            {
                if (schedule.find(pred) == schedule.end())
                {
                    allPredecessorsScheduled = false;
                    break;
                }
                maxTime = max(maxTime, schedule[pred] + 1); // Assuming unit delay (dj = 1)
            }

            if (allPredecessorsScheduled && schedule.find(next) == schedule.end())
            {
                schedule[next] = maxTime;
                readyQueue.push(next);
            }
        }
    }

    // Print the schedule
    for (const auto &entry : schedule)
    {
        cout << "Node " << entry.first->getName() << " scheduled at time " << entry.second << endl;
    }
}
// #include <vector>
// #include <string>
// #include <map>
// #include <unordered_map>
// #include <unordered_set>
// #include <iostream>
// #include "blif2verilog.h"

// using std::map;
// using std::string;
// using std::unordered_map;
// using std::unordered_set;
// using std::vector;

// // class cell
// // {
// // private:
// //     vector<cell *> prev;
// //     vector<cell *> next;
// //     string op;
// //     bool isnop;
// //     string name;

// // public:
// //     cell(string name, string op, bool isnop);
// //     cell();
// //     ~cell();
// //     void addPrev(cell *prev);
// //     void addNext(cell *next);
// //     vector<cell *> getPrev();
// //     vector<cell *> getNext();
// //     string getOp(); //&, !, |, *
// //     string getName();
// //     bool getIsnop();
// // };

// // get the number of the graph
// int getNumber(cell *c, unordered_set<cell *> &set, vector<cell *> &vec)
// {
//     if (set.find(c) != set.end())
//         return 0; // already exists, return 0
//     set.insert(c);

//     int number = 0;
//     if (!c->getIsnop() && c->getOp() != "p")
//     {
//         number = 1;
//         vec.push_back(c);
//     }
//     if (c->getNext().size() == 0)
//     {
//         return number;
//     }
//     for (int i = 0; i < c->getNext().size(); i++)
//     {
//         number += getNumber(c->getNext()[i], set, vec);
//     }
//     return number;
// }

// /*Integer linear program, Time complexity: O(n^n)
//  *整数线性规划 */
// void ML_RCS(cell *head)
// {

//     // 可以使用ASAP和ALAP减少计算量，此处暂时未应用

//     unordered_set<cell *> cellSet;
//     vector<cell *> usedCells;
//     int number = getNumber(head, cellSet, usedCells);

//     unordered_map<cell *, int> cellMap;
//     for (int i = 0; i < usedCells.size(); i++)
//     {
//         cellMap.insert({usedCells[i], i});
//     }

//     std::cout << number <<endl;

//     /*int worstCycles = constrain["&"]*2 + constrain["|"]*3 + constrain["!"]*1;
//     int bestCycles = worstCycles;

//     int* curr = new int[number];
//     for (int i = 0; i < number - 1; i++) curr[i] = 0;
//     curr[number-1] = -1;

//     int* solution = new int[number];
//     vector<vector<cell*>> cycleN;

//     while (true) {
//         //update
//         int i = number-1;
//         curr[number-1] ++;
//         while (bestCycles - ) {
//             curr[i] = 0;
//             i--;
//         }
//     }*/
// }
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include "blif2verilog.h"

using std::map;
using std::max;
using std::min;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;


// get the number of the graph
int getNumber(cell *c, unordered_set<cell *> &set, vector<cell *> &vec)
{
    if (set.find(c) != set.end())
        return 0; // already exists, return 0
    set.insert(c);

    int number = 0;
    if (!c->getIsnop() && c->getOp() != "p")
    {
        number = 1;
        vec.push_back(c);
    }

    for (int i = 0; i < c->getNext().size(); i++)
    {
        number += getNumber(c->getNext()[i], set, vec);
    }
    return number;
}

int getDelay(cell *c)
{
    string op = c->getOp();
    // int p = c->getPrev().size()-1;
    if (op == "&")
        return 2;
    if (op == "|")
        return 3;
    if (op == "!")
        return 1;
    return -1;
}

/*Integer linear program, Time complexity: O(n^n)
 *整数线性规划
 *no multiple precursor
 *不能有多个前驱*/
int ML_RCS_enumerate(cell *head, map<string, int> constrain)
{

    // 可以使用ASAP和ALAP减少计算量，此处暂时未应用

    unordered_set<cell *> cellSet;
    vector<cell *> usedCells;
    int number = getNumber(head, cellSet, usedCells);

    unordered_map<cell *, int> cellMap;
    int worstCycles = 0;
    for (int i = 0; i < usedCells.size(); i++)
    {
        cellMap.insert({usedCells[i], i});
        worstCycles += getDelay(usedCells[i]);
    }

    int bestCycles = worstCycles;

    int *curr = new int[number];
    for (int i = 0; i < number - 1; i++)
        curr[i] = 0;
    curr[number - 1] = -1;

    int *solution = new int[number];
    vector<vector<cell *>> cycleN(worstCycles);

    while (true)
    {
        // update
        int i = number - 1;
        curr[number - 1]++;
        while (bestCycles - getDelay(usedCells[i]) < curr[i])
        {
            curr[i] = 0;
            i--;
            if (i < 0)
            {
                for (int j = 0; j < bestCycles; j++)
                {
                    std::cout << "cycle" << j << ":";
                    for (int k = 0; k < number; k++)
                    {
                        if (solution[k] == j)
                        {
                            std::cout << usedCells[k]->getName() << ' ';
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << bestCycles;
                return bestCycles;
            }
            curr[i]++;
        }

        // set cycleN
        cycleN.clear();
        for (int i = 0; i < number; i++)
        {
            cycleN[curr[i]].push_back(usedCells[i]);
        }

        // check constrain
        int cAnd = 0, cOr = 0, cNot = 0;
        int wAnd[2] = {0},
            wOr[3] = {0},
            wNot[1] = {0};
        int p = 0;

        bool passed = true;
        int currentCycles = 0;
        for (int i = 0; i < bestCycles; i++)
        {
            cAnd -= wAnd[p % 2];
            cOr -= wOr[p % 3];
            cNot -= wNot[p % 1];
            wAnd[p % 2] = 0;
            wOr[p % 3] = 0;
            wNot[p % 1] = 0;
            for (int j = 0; j < cycleN[i].size(); j++)
            {
                cell *cCell = cycleN[i][j];
                string op = cCell->getOp();
                if (op == "&")
                    wAnd[p % 2]++;
                else if (op == "|")
                    wOr[p % 3]++;
                else if (op == "!")
                    wNot[p % 1]++;
            }

            if (wAnd[p % 2] > 0)
            {
                cAnd += wAnd[p % 2];
                currentCycles = max(currentCycles, i + 2);
                if (cAnd > constrain["&"])
                {
                    passed = false;
                    break;
                }
            }
            if (wOr[p % 3] > 0)
            {
                cOr += wOr[p % 3];
                currentCycles = max(currentCycles, i + 3);
                if (cOr > constrain["|"])
                {
                    passed = false;
                    break;
                }
            }
            if (wNot[p % 1] > 0)
            {
                cNot += wNot[p % 1];
                currentCycles = max(currentCycles, i + 1);
                if (cNot > constrain["!"])
                {
                    passed = false;
                    break;
                }
            }
            p++;
        }
        if (!passed)
            continue;

        // check precursor
        for (int i = 0; i < number; i++)
        {
            auto prevs = usedCells[i]->getPrev();
            for (int j = 0; j < prevs.size(); j++)
            {
                auto prev = prevs[j];
                if (prev->getIsnop())
                    continue;
                while (cellMap.find(prev) != cellMap.end())
                {
                    prev = prev->getPrev()[0]; // 没在cellMap的cell至多有一个前驱
                }
                if (curr[cellMap[prev]] + getDelay(prev) > curr[i])
                {
                    passed = false;
                    break;
                }
            }
            if (!passed)
                break;
        }
        if (!passed)
            continue;

        bestCycles = min(bestCycles, currentCycles);
        for (int i = 0; i < number; i++)
        {
            solution[i] = curr[i];
        }
    }
}

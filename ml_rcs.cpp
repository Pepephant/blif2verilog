#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include "blif2verilog.h"

using std::map;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

// class cell
// {
// private:
//     vector<cell *> prev;
//     vector<cell *> next;
//     string op;
//     bool isnop;
//     string name;

// public:
//     cell(string name, string op, bool isnop);
//     cell();
//     ~cell();
//     void addPrev(cell *prev);
//     void addNext(cell *next);
//     vector<cell *> getPrev();
//     vector<cell *> getNext();
//     string getOp(); //&, !, |, *
//     string getName();
//     bool getIsnop();
// };

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
    if (c->getNext().size() == 0)
    {
        return number;
    }
    for (int i = 0; i < c->getNext().size(); i++)
    {
        number += getNumber(c->getNext()[i], set, vec);
    }
    return number;
}

/*Integer linear program, Time complexity: O(n^n)
 *整数线性规划 */
void ML_RCS(cell *head)
{

    // 可以使用ASAP和ALAP减少计算量，此处暂时未应用

    unordered_set<cell *> cellSet;
    vector<cell *> usedCells;
    int number = getNumber(head, cellSet, usedCells);

    unordered_map<cell *, int> cellMap;
    for (int i = 0; i < usedCells.size(); i++)
    {
        cellMap.insert({usedCells[i], i});
    }

    std::cout << number <<endl;

    /*int worstCycles = constrain["&"]*2 + constrain["|"]*3 + constrain["!"]*1;
    int bestCycles = worstCycles;

    int* curr = new int[number];
    for (int i = 0; i < number - 1; i++) curr[i] = 0;
    curr[number-1] = -1;

    int* solution = new int[number];
    vector<vector<cell*>> cycleN;

    while (true) {
        //update
        int i = number-1;
        curr[number-1] ++;
        while (bestCycles - ) {
            curr[i] = 0;
            i--;
        }
    }*/
}

#include <deque>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class cell
{
public:
    string name;
    string op;
    vector<cell *> next;
    vector<cell *> prev;

    string getName() { return name; }
    string getOp() { return op; }
    vector<cell *> &getNext() { return next; }
};

map<string, int> operation_cycles = {
    {"&", 2},
    {"|", 3},
    {"!", 1},
    {"p", 0} // Assuming 'p' is a no-cost operation
};

map<int, vector<cell *>> asap(set<cell *> &cells)
{
    map<int, vector<cell *>> levels;
    map<cell *, int> in_degree;

    // Initialize in-degrees
    for (auto cell : cells)
    {
        in_degree[cell] = 0;
    }

    // Calculate in-degrees
    for (auto cell : cells)
    {
        for (auto next_cell : cell->getNext())
        {
            in_degree[next_cell]++;
        }
    }

    // Queue for nodes with zero in-degree, excluding fnop
    deque<cell *> queue;
    for (auto cell : cells)
    {
        if (in_degree[cell] == 0 && cell->getName() != "fnop")
        {
            queue.push_back(cell);
        }
    }

    int level = 0;
    int cumulative_cycles = 0;

    while (!queue.empty())
    {
        int level_size = queue.size();
        int max_cycles = 0;

        for (int i = 0; i < level_size; ++i)
        {
            cell *current_cell = queue.front();
            queue.pop_front();
            levels[level].push_back(current_cell);

            // Calculate max clock cycles for the current level
            if (operation_cycles.find(current_cell->getOp()) != operation_cycles.end())
            {
                max_cycles = max(max_cycles, operation_cycles[current_cell->getOp()]);
            }

            // Update in-degrees and queue
            for (auto next_cell : current_cell->getNext())
            {
                in_degree[next_cell]--;
                if (in_degree[next_cell] == 0)
                {
                    queue.push_back(next_cell);
                }
            }
        }

        // Accumulate max clock cycles for the current level
        cumulative_cycles += max_cycles;
        levels[level].push_back((cell *)new pair<int, int>(max_cycles, cumulative_cycles));
        level++;
    }

    // Add fnop to the last level
    for (auto cell : cells)
    {
        if (cell->getName() == "fnop")
        {
            levels[level].push_back(cell);
        }
    }
    levels[level].push_back((cell *)new pair<int, int>(0, cumulative_cycles));

    return levels;
}

void print_asap_schedule(map<int, vector<cell *>> &levels)
{
    for (auto &level : levels)
    {
        auto cells = level.second;
        pair<int, int> *cycles = (pair<int, int> *)cells.back();
        cells.pop_back();

        cout << "Level " << level.first << ":" << endl;
        for (auto cell : cells)
        {
            cout << "  Cell " << cell->getName() << " with operation " << cell->getOp() << endl;
        }
        cout << "  Max clock cycles for this level: " << cycles->first << endl;
        cout << "  Cumulative clock cycles up to this level: " << cycles->second << endl;

        delete cycles; // Clean up the dynamically allocated pair
    }
}
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

class model
{
private:
    string name;

public:
    model();
    model(string name);
    ~model();
    string getName();
};

class input
{
private:
    vector<string> inputs;

public:
    input(vector<string> inputs);
    input();
    ~input();
    vector<string> getInputs();
};

class output
{
private:
    vector<string> outputs;

public:
    output(vector<string> outputs);
    output();

    ~output();
    vector<string> getOutputs();
};

class wire
{
private:
    vector<string> wires;

public:
    wire(vector<string> wires);
    ~wire();
    vector<string> getWires();
};

class names
{
private:
    input inputs;
    output outputs;
    vector<vector<int>> values;
    string expr;

public:
    names(input inputs, output outputs, vector<vector<int>> values);
    ~names();
    input getInputs();
    output getOutputs();
    vector<vector<int>> getValues();
    string getExpr();
    void toExpr();
};

class cell
{
private:
    vector<cell *> prev;
    vector<cell *> next;
    string op;  //如果该节点为null，则为单一元素，如果为！，则为 "！name",如果为 & 或 |，则为 "name = prev1 op prve2..."
    bool isnop;
    string name; //如果该字段为null,则该cell为中间节点

public:
    cell(string name, string op, bool isnop);
    cell(string op, bool isnop);

    cell();
    ~cell();
    void addPrev(cell *prev);
    void addNext(cell *next);
    vector<cell *> getPrev();
    vector<cell *> getNext();
    string getOp();
    string getName();
    bool getIsnop();
    
};


cell *vtog(string vfilename);

vector<string> split_blank(const string &str);
#pragma once
#include <iostream>
#include <string>
#include <vector>

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


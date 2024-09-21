#include "blif2verilog.h"
model::model(string name)
{
    this->name = name;
}
model::model()
{
}
model::~model()
{
}
string model::getName()
{
    return this->name;
}

input::input(vector<string> inputs)
{
    this->inputs = inputs;
}
input::input()
{
}
input::~input()
{
}

vector<string> input::getInputs()
{
    return this->inputs;
}

output::output(vector<string> outputs)
{
    this->outputs = outputs;
}
output::output()
{
}
output::~output()
{
}

vector<string> output::getOutputs()
{
    return this->outputs;
}

wire::wire(vector<string> wires)
{
    this->wires = wires;
}
wire::~wire()
{
}

vector<string> wire::getWires()
{
    return this->wires;
}

names::names(input inputs, output outputs, vector<vector<int>> values)
{
    this->inputs = inputs;
    this->outputs = outputs;
    this->values = values;
}
names::~names()
{
}

input names::getInputs()
{
    return this->inputs;
}

output names::getOutputs()
{
    return this->outputs;
}

vector<vector<int>> names::getValues()
{
    return this->values;
}

void names::toExpr()
{
    // string expr = "assign ";
    // expr += this->outputs.getOutputs()[0] + " = ";
    vector<string> inputs = this->inputs.getInputs();
    vector<vector<int>> values = this->values;
    vector<string> exprs;
    for (int v = 0; v < values.size(); v++)
    {
        string expr;
        vector<int> value = values[v];
        vector<string> new_intputs;
        for (int i = 0; i < inputs.size(); i++)
        {
            if(value[i]==1)
                new_intputs.push_back(inputs[i]);
            else if(value[i]==0)
                new_intputs.push_back("!" + inputs[i]);
        }
        for (int i = 0; i < new_intputs.size();i++)
        {
            if(i==new_intputs.size()-1)
                expr += new_intputs[i];
            else
                expr += new_intputs[i] + " & ";
        }
        exprs.push_back(expr);
    }
    string expr = "assign " + this->outputs.getOutputs()[0] + " = ";
    for (int i = 0; i < exprs.size(); i++)
        if (i == exprs.size() - 1)
            expr += "(" + exprs[i] + ");";
        else
            expr += "(" + exprs[i] + ")" + " | ";
    this->expr = expr;
}

string names::getExpr()
{
    return this->expr;
}
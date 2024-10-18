#include "blif2verilog.h"
#include <fstream>
#include <sstream>


class cell;

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
            if (value[i] == 1)
                new_intputs.push_back(inputs[i]);
            else if (value[i] == 0)
                new_intputs.push_back("!" + inputs[i]);
        }
        for (int i = 0; i < new_intputs.size(); i++)
        {
            if (i == new_intputs.size() - 1)
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

cell::cell(string name, string op, bool isnop)
{
    this->name = name;
    this->op = op;
    this->isnop = isnop;
}
cell::cell( string op, bool isnop)
{
    this->op = op;
    this->isnop = isnop;
}
void cell::addNext(cell *next)
{
    this->next.push_back(next);
}
void cell::addPrev(cell *prev)
{
    this->prev.push_back(prev);
}

string cell::getName()
{
    return this->name;
}

string cell::getOp()
{
    return this->op;
}

bool cell::getIsnop()
{
    return this->isnop;
}

vector<cell *> cell::getPrev()
{
    return this->prev;
}

vector<cell *> cell::getNext()
{
    return this->next;
}

cell *vtog(string vfilename)
{
    ifstream file(vfilename);
    if (!file.is_open())
    {
        cout << "Error: File not found" << endl;
        return nullptr;
    }
    vector<string> lines;
    string line;
    while (getline(file, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }
    file.close();

    model m;
    input inputs;
    output outputs;
    vector<names> ns;

    // 解析blif文件转化为model、input、output、ns
    for (auto it = lines.begin(); it < lines.end(); ++it)
    {
        vector<string> tokens = split_blank(*it);
        if (tokens[0] == ".model")
        {
            m = model(tokens[1]);
        }
        else if (tokens[0] == ".inputs")
        {
            tokens.erase(tokens.begin());
            inputs = input(tokens);
        }
        else if (tokens[0] == ".outputs")
        {
            tokens.erase(tokens.begin());
            outputs = output(tokens);
        }
        else if (tokens[0] == ".names")
        {
            vector<string> outputs = {tokens[tokens.size() - 1]};
            output o(outputs);
            vector<string> inputs(tokens.begin() + 1, tokens.end() - 1);
            input input(inputs);
            auto next_it = it + 1;
            vector<string> next_tokens = split_blank(*next_it);
            vector<vector<int>> table;

            while (next_tokens[0] != ".names" && next_tokens[0] != ".end")
            {
                string str = next_tokens[0];
                vector<int> v;
                for (char c : str)
                {
                    if (c == '0')
                        v.push_back(0);
                    else if (c == '1')
                        v.push_back(1);
                    else
                        v.push_back(-1);
                }
                table.push_back(v);
                next_tokens = split_blank(*(++next_it));
            }
            ns.push_back(names(input, o, table));
        }
    }
    // 遍历ns，生成cell
    map<string, cell *> cells;
    map<string, cell *> inputcell;
    for (auto it = ns.begin(); it < ns.end(); ++it)
    {
        names n = *it;
        input inputs = n.getInputs();
        output outputs = n.getOutputs();
        if (n.getValues().size() == 1) // 不需要中间节点
        {
            cell *c = new cell(outputs.getOutputs()[0], "&", false);
            vector<int> value = n.getValues()[0];
            for (auto it = value.begin(); it < value.end(); ++it)
                if (*it == 1)
                {
                    cell *prev = new cell(inputs.getInputs()[it - value.begin()], nullptr, false);
                    c->addPrev(prev);
                    prev->addNext(c);
                    if (find(inputs.getInputs().begin(), inputs.getInputs().end(), prev->getName())!=inputs.getInputs().end())
                        inputcell.insert(pair<string, cell *>(prev->getName(), prev));
                }
                else if (*it == 0)
                {
                    cell *prev = new cell(inputs.getInputs()[it - value.begin()], "!", false);
                    c->addPrev(prev);
                    prev->addNext(c);
                    if (find(inputs.getInputs().begin(), inputs.getInputs().end(), prev->getName())!=inputs.getInputs().end())
                        inputcell.insert(pair<string, cell *>(prev->getName(), prev));
                }
            // cells.push_back(c);
            cells.insert(pair<string, cell *>(c->getName(), c));
        }
        else // 需要中间节点
        {
            cell *c = new cell(outputs.getOutputs()[0], "|", false);
            vector<vector<int>> values = n.getValues();
            for (auto it = values.begin(); it < values.end(); ++it)
            {
                cell *iresult = new cell( "&", false);
                for (auto it1 = it->begin(); it1 < it->end(); ++it1)
                {
                    if (*it1 == 1)
                    {
                        cell *prev = new cell(inputs.getInputs()[it1 - it->begin()], nullptr, false);
                        iresult->addPrev(prev);
                        prev->addNext(iresult);
                        if (find(inputs.getInputs().begin(), inputs.getInputs().end(), prev->getName())!=inputs.getInputs().end())
                            inputcell.insert(pair<string, cell *>(prev->getName(), prev));
                    }
                    else if (*it1 == 0)
                    {
                        cell *prev = new cell(inputs.getInputs()[it1 - it->begin()], "!", false);
                        iresult->addPrev(prev);
                        prev->addNext(iresult);
                        if (find(inputs.getInputs().begin(), inputs.getInputs().end(), prev->getName())!=inputs.getInputs().end())
                            inputcell.insert(pair<string, cell *>(prev->getName(), prev));
                    }
                }
                c->addPrev(iresult);
                iresult->addNext(c);
            }
            cells.insert(pair<string, cell *>(c->getName(), c));
        }
    }
    // 组成有向图
    set<string> nsoutput;
    for (auto it = ns.begin(); it < ns.end(); ++it)
        nsoutput.insert(it->getOutputs().getOutputs()[0]);
    // 得到中间节点
    for (auto it = outputs.getOutputs().begin(); it < outputs.getOutputs().end(); ++it)
    {
        nsoutput.erase(*it);
    }
    // 拼接中间节点
    for (auto it = nsoutput.begin(); it != nsoutput.end(); ++it)
    {
        for (auto ins = ns.begin(); ins < ns.end(); ++ins)
        {
            vector<string> inputs = ins->getInputs().getInputs();
            if (find(inputs.begin(), inputs.end(), *it)!=inputs.end())
            {
                string output = ins->getOutputs().getOutputs()[0];
                cell *prev = cells.find(*it)->second;
                cell *next = cells.find(output)->second;
                for (auto cprev = next->getPrev().begin(); cprev < next->getPrev().end(); ++cprev)
                {
                    if ((*cprev)->getName() == *it)
                    {
                        next->getPrev().erase(cprev);
                        delete *cprev;
                    }
                }
                next->addPrev(prev);
                prev->addNext(next);
            }
        }
    }
    // 加上nop
    cell *fnop = new cell("fnop", nullptr, true);
    cell *hnop = new cell("hnop", nullptr, true);
    vector<string> in = inputs.getInputs();
    vector<string> out = outputs.getOutputs();
    for (auto it = inputcell.begin(); it != inputcell.end(); ++it)
    {
        cell *bottom = it->second;
        bottom->addPrev(fnop);
        fnop->addNext(bottom);
    }
    for (auto it = out.begin(); it < out.end(); ++it)
    {
        cell *top = cells.find(*it)->second;
        top->addNext(hnop);
        hnop->addPrev(top);
    }
    return hnop;
}

cell::~cell()
{

}

vector<string> split_blank(const string &str)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (tokenStream >> token)
        tokens.push_back(token);
    return tokens;
}
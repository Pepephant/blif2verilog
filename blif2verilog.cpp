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
output names::getOutputs() { return this->outputs; }
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

cell::cell(string name, string op, bool isnop, int n)
{
    this->name = name;
    this->op = op;
    this->isnop = isnop;
    this->next.reserve(10);
    this->prev.reserve(10);
    // this->n = n;
}
// cell::cell(string op, bool isnop)
// {
//     this->op = op;
//     this->isnop = isnop;
// }
// cell::cell(bool isnop)
// {
//     this->isnop = isnop;
// }
// cell::cell(bool isnop, string name)
// {
//     this->name = name;
//     this->isnop = isnop;
// }
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
void cell::setOp(string op)
{
    this->op = op;
}
void cell::setName(string name)
{
    this->name = name;
}

bool cell::getIsnop()
{
    return this->isnop;
}

vector<cell *> &cell::getPrev()
{
    return this->prev;
}

vector<cell *> &cell::getNext()
{
    return this->next;
}
// int cell::getN()
// {
//     return this->n;
// }
// void cell::setN(int n)
// {
//     this->n = n;
// }

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
            cell *c = new cell(outputs.getOutputs()[0], "&", false, 1);
            vector<int> value = n.getValues()[0];
            for (auto it = value.begin(); it < value.end(); ++it)
                if (*it == 1)
                {
                    cell *prev = new cell(inputs.getInputs()[it - value.begin()], "p", false, 1);
                    c->addPrev(prev);
                    prev->addNext(c);
                    if (find(inputs.getInputs().begin(), inputs.getInputs().end(), prev->getName()) != inputs.getInputs().end())
                        inputcell.insert(pair<string, cell *>(prev->getName(), prev));
                }
                else if (*it == 0)
                {
                    cell *notcell = new cell("*", "!", false, 0);
                    cell *prev = new cell(inputs.getInputs()[it - value.begin()], "p", false, 0);
                    // c->addPrev(prev);
                    // prev->addNext(c);
                    notcell->addPrev(prev);
                    prev->addNext(notcell);
                    c->addPrev(notcell);
                    notcell->addNext(c);
                    if (find(inputs.getInputs().begin(), inputs.getInputs().end(), prev->getName()) != inputs.getInputs().end())
                        inputcell.insert(pair<string, cell *>(prev->getName(), prev));
                }
            // cells.push_back(c);
            cells.insert(pair<string, cell *>(c->getName(), c));
        }
        else // 需要中间节点
        {
            cell *c = new cell(outputs.getOutputs()[0], "|", false, 1);
            vector<vector<int>> values = n.getValues();
            for (auto it = values.begin(); it < values.end(); ++it)
            {
                cell *iresult = new cell("*", "&", false, 1);
                for (auto it1 = it->begin(); it1 < it->end(); ++it1)
                {
                    if (*it1 == 1)
                    {
                        cell *prev = new cell(inputs.getInputs()[it1 - it->begin()], "p", false, 1);
                        iresult->addPrev(prev);
                        prev->addNext(iresult);
                        if (find(inputs.getInputs().begin(), inputs.getInputs().end(), prev->getName()) != inputs.getInputs().end())
                            inputcell.insert(pair<string, cell *>(prev->getName(), prev));
                    }
                    else if (*it1 == 0)
                    {
                        cell *notcell = new cell("*", "!", false, 0);
                        cell *prev = new cell(inputs.getInputs()[it1 - it->begin()], "p", false, 0);
                        // iresult->addPrev(prev);
                        // prev->addNext(iresult);
                        notcell->addPrev(prev);
                        prev->addNext(notcell);
                        iresult->addPrev(notcell);
                        notcell->addNext(iresult);
                        if (find(inputs.getInputs().begin(), inputs.getInputs().end(), prev->getName()) != inputs.getInputs().end())
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
    // for (auto it = outputs.getOutputs().begin(); it < outputs.getOutputs().end(); ++it)
    // {
    //     cout << *it << endl;
    //     //nsoutput.erase(*it);
    // }
    for (string s : outputs.getOutputs())
        nsoutput.erase(s);
    // exit(0);
    // 拼接中间节点
    for (auto it = nsoutput.begin(); it != nsoutput.end(); ++it)
    {
        for (auto ins = ns.begin(); ins < ns.end(); ++ins)
        {
            vector<string> inputs = ins->getInputs().getInputs();
            if (find(inputs.begin(), inputs.end(), *it) != inputs.end())
            {
                string output = ins->getOutputs().getOutputs()[0];
                cell *prev = cells.find(*it)->second;
                cell *next = cells.find(output)->second;
                // auto cprev = next->getPrev().begin();
                //  cout<<(*cprev)->getName()<<endl;
                // cout<<next->getPrev()[0]->getName()<<endl;
                // exit(0);
                for (auto cprev = next->getPrev().begin(); cprev < next->getPrev().end(); ++cprev)
                {
                    // cell*
                    if ((*cprev)->getName() == "*") // 中间节点
                    {
                        for (auto cprev1 = (*cprev)->getPrev().begin(); cprev1 < (*cprev)->getPrev().end(); ++cprev1)
                        {
                            if ((*cprev1)->getName() == (*it))
                            {
                                (*cprev)->addPrev(prev);
                                prev->addNext(*cprev);
                                // if ((*cprev1)->getN() == 0)
                                //     prev->setN(0);
                                (*cprev)->getPrev().erase(cprev1);
                                // delete *cprev1;
                            }
                        }
                    }
                    else if ((*cprev)->getName() == (*it)) // 单一节点
                    {
                        // if((*cprev)->getN() == 0)
                        //     prev->setN(0);
                        next->getPrev().erase(cprev);
                        /// delete *cprev;
                        next->addPrev(prev);
                        prev->addNext(next);
                    }
                    // if ((*cprev)->getName() == *it)
                    // {
                    //     next->getPrev().erase(cprev);
                    //     delete *cprev;
                    // }
                }
                // next->addPrev(prev);
                // prev->addNext(next);
            }
        }
    }
    // 加上nop
    cell *fnop = new cell("fnop", "p", true, 1);
    cell *hnop = new cell("hnop", "p", true, 1);
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
    // return fnop;
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

void print_tree(cell *root, int i)
{
    if (root == nullptr)
        return;
    string spaces(15, ' ');
    cout << i;
    cout << spaces << root->getName() + " " + root->getOp() + " " << endl;
    // cout<<root->getN()<< endl;
    vector<cell *> &prev = root->getPrev();
    for (auto c : prev)
    {
        print_tree(c, i + 1);
    }
}

void collect(cell *root, set<cell *> &cells)
{

    if (root == nullptr)
        return;
    cells.insert(root);
    vector<cell *> &prev = root->getPrev();
    for (auto c : prev)
        collect(c, cells);
}

void destroy(cell *root)
{
    if (root == nullptr)
        return;
    set<cell *> cells;
    collect(root, cells);
    for(auto c:cells)
        delete c;
}
#include "blif2verilog.h"
#include <fstream>
#include <sstream>
using namespace std;

vector<string> split_blank(const string &str);
bool is_re(const string &str, const vector<string> &v);

int main()
{
    string filename;
    cout << "Enter the file name: ";
    cin >> filename;
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: File not found" << endl;
        return 1;
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

    for (auto it = lines.begin(); it < lines.end(); ++it)
    {
        // if (it->empty())
        //     continue;
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
    for (int i = 0; i < ns.size(); i++)
    {
        ns[i].toExpr();
    }

    ofstream ofile("../test2.v");
    ofile << "module " << m.getName() << "(clk, rst";
    for (string s : inputs.getInputs())
        ofile << ", " << s;
    for (string s : outputs.getOutputs())
        ofile << ", " << s;
    ofile << ");" << endl;
    ofile << "input clk, rst;" << endl;
    for (string s : outputs.getOutputs())
        ofile << "output " << s << ";" << endl;
    for (string s : inputs.getInputs())
        ofile << "input " << s << ";" << endl;
    ofile << endl;
    // wire
    for (string s : inputs.getInputs())
        ofile << "wire " << s << ";" << endl;
    for (string s : outputs.getOutputs())
        ofile << "wire " << s << ";" << endl;
    for (names n : ns)
    {
        string o = n.getOutputs().getOutputs()[0];
        if (is_re(o, inputs.getInputs()) || is_re(o, outputs.getOutputs()))
            continue;
        ofile << "wire " << o << ";" << endl;
    }
    ofile << endl;
    // assign
    for (names n : ns)
    {
        ofile << n.getExpr() << endl;
    }
    ofile << endl;
    ofile << "endmodule" << endl;
    ofile.close();
    return 0;
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

// check if str is in v
// return true if str is in v
bool is_re(const string &str, const vector<string> &v)
{
    for (string s : v)
    {
        if (str == s)
            return true;
    }
    return false;
}
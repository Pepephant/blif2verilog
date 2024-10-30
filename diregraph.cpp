// 将.blif文件转换为有向图
#include "blif2verilog.h"
#include "ml_rcs.h"

using namespace std;
int main()
{
    string blif_file = "./test1.blif";
    cell * hnop = vtog(blif_file);
    // ML_RCS_enumerate(fnop);
    // ML_RCS(hnop);
    // vector<string> a = {"../test.blif", "../test2.blif"};
    // for (auto it = a.begin(); it < a.end(); it++)
    //     cout << *it << endl;
    vector<cell*>& v = hnop->getPrev();
    for(cell* c:v)
    {
        print_tree(c,0);
        cout << endl;
    }
    destory(hnop);
    return 0;
}
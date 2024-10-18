// 将.blif文件转换为有向图
#include "blif2verilog.h"

int main()
{
    string blif_file = "../test.blif";
    cell * hnop = vtog(blif_file);
    return 0;
}
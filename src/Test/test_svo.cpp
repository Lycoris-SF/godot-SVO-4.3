#include "test_svo.h"

int main() {

    //SparseVoxelOctree svo_test;  // 创建对象实例

    // 在这里添加测试代码
    // 例如：svo.insert(x, y, z); 或者 svo.query(x, y, z);
    //svo_test.insert(1, 1, 1);

    return 0;
}

TestLog::TestLog() : test_countA(0), test_countB(0), log_textA(""), log_textB("") {}

void TestLog::test_print()
{
    std::cout << log_textA << test_countA << std::endl;
    std::cout << log_textB << test_countB << std::endl;
}

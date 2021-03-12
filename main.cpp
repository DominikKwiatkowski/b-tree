#include "tree.h"
#include "testInterface.h"
int main()
{
    int d = 2;
    std::string treeFileName = "tree.dat";
    std::string dataFileName = "data.dat";
    std::string testFile = "test.txt";
    Tree tree = Tree(d, treeFileName, dataFileName);
    TestInterface test = TestInterface(tree, testFile, dataFileName,treeFileName,d);
    test.performTest();
}
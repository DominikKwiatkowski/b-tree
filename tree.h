#pragma once
#include <vector>

#include "Page.h"
class Tree
{
public:
    const int d;
    bool empty = true;
    std::string treeFileName;
    std::string dataFileName;
    int pageNumber = 0;
    int recordNumber = 0;

    std::vector<int> emptyPages;
    std::vector<int> emptyAddresses;

    int totalDiskOperation = 0;
    int lastDiskOperation = 0;

    Tree(int d, std::string treeFileName, std::string dataFileName);
    int getNextPageAddress();
    int getNextNextPageAddress();
    void handeOnePageAdd();
    int find(int index);
    int findPosiotion(Page * currentPage, int index);
    void modify(int index, int record[Record::PARAM]);

    void print();

    // return code=0 function end correctly
    // return code=1 index exist error
    int add();
    int add(int index);
    int getNewAddress();
    int add(int index, int record[Record::PARAM]);
    int addCompensation(Page* currentPage);
    void addToPage(Page* currentPage, Index newIndex, int newPageAddress = -1);

    // return code=0 function end correctly
    // return code=1 index non exist error
    void deleteRecord(int index);
    void replaceRecord(Page* currentPage, int index);
    int deleteCompensation(Page* currentPage);
    void mergePage(Page* &currentPage);
    void deleteRecordOnPage(Page* currentPage, int index);
};
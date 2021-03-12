#pragma once
#include "Record.h"

struct Index
{
    int ind;
    int addr;
};

class Page
{
public:
    // non disk 
    int d;
    const int pageSize;
    int pageNumber;
    std::string treeFileName;

    // disk
    int parentPageNumber;
    int elemCount;
    bool isLeaf = true;
    Index* indexTable;
    int* childTable;
    Page(int d, int pageNumber, std::string treeFileName);
    ~Page();

    void savePage();
    void readPage(int pageNumber);

    void print(int pageNumber=0);
    void printDebug(int &count,int pageNumber=0);
    void printAllPages();

    void splitRootPage(int firstPageNumber, int secondPageNumber);
    int splitPage(int firstPageNumber, int secondPageNumber);
    int changeParent();
    void chageParentForPage(int pageNumber);
    void copy(Page* copiedPage);
};
#include "Page.h"

#include <iostream>
using namespace std;

Page::Page(int d, int pageNumber, string treeFileName)
    :d(d), pageSize(2 * sizeof(int) + sizeof(bool) + 2*d * sizeof(Index) + (2*d + 1) * sizeof(int)),pageNumber(pageNumber)
{
    indexTable = new Index[2*d+1];
    childTable = new int[2*d+2];
    this->treeFileName = treeFileName;
    elemCount = 0;
    parentPageNumber = -1;
}

Page::~Page()
{
    delete[] childTable;
    delete[] indexTable;
}

void Page::savePage()
{
    FILE *file = fopen(treeFileName.c_str(), "r+b");
    fseek(file, pageSize * pageNumber, 0);
    fwrite(&parentPageNumber, sizeof(int),1,file);
    fwrite(&elemCount, sizeof(int), 1, file);
    fwrite(&isLeaf, sizeof(bool), 1, file);
    fwrite(indexTable, sizeof(Index), 2 * d, file);
    fwrite(childTable, sizeof(int), 2 * d + 1, file);
    fclose(file);
}

void Page::readPage(int pageNumber)
{
    this->pageNumber = pageNumber;
    FILE* file = fopen(treeFileName.c_str(), "r+b");
    fseek(file, pageSize * pageNumber, 0);
    fread(&parentPageNumber, sizeof(int), 1, file);
    fread(&elemCount, sizeof(int), 1, file);
    fread(&isLeaf, sizeof(bool), 1, file);
    fread(indexTable, sizeof(Index), 2 * d, file);
    fread(childTable, sizeof(int), 2 * d + 1, file);
    fclose(file);
}

void Page::print(int pageNumber)
{
    readPage(pageNumber);
    printf("My page number is %d\n", pageNumber);
    printf("I have index: ");
    for(int i=0;i<elemCount;i++)
    {
        printf("%d ", this->indexTable[i].ind);
    }
    if (!isLeaf)
    {
        printf("\nThis pages are my children ");
        for (int i = 0; i < elemCount + 1; i++)
        {
            printf("%d ", this->childTable[i]);
        }
        printf("\n");
        //system("pause");
        
        for (int i = 0; i < elemCount + 1; i++)
        {
            Page childPage(d, 0, treeFileName);
            childPage.print(childTable[i]);
        }
    }
    else
    {
        printf("\nI am a leaf\n");
    }
}

void Page::printDebug(int &count,int pageNumber)
{
    readPage(pageNumber);
    if (!isLeaf)
    {
        for (int i = 0; i < elemCount + 1; i++)
        {
            Page childPage(d, 0, treeFileName);
            childPage.printDebug(count,childTable[i]);
            if (i < elemCount)
            {
                printf("%d ", this->indexTable[i].ind);
                count++;
            }
        }
    }
    else
    {
        for (int i = 0; i < elemCount; i++)
        {
            printf("%d ", this->indexTable[i].ind);
            count++;
        }
    }
}

int Page::changeParent()
{
    if (isLeaf)
        return 0;
    for(int i=0;i<this->elemCount+1;i++)
    {
        chageParentForPage(childTable[i]);
    }
    return 2 * (this->elemCount + 1);
}

void Page::chageParentForPage(int pageNumber)
{
    Page childPage = Page(d, 0, treeFileName);
    childPage.readPage(pageNumber);
    childPage.parentPageNumber = this->pageNumber;
    childPage.savePage();

}
void Page::splitRootPage(int firstPageNumber,int secondPageNumber)
{
    // first, we create 2 children page and assign propers value
    Page leftChildren(d, firstPageNumber, treeFileName);
    Page rightChildren(d, secondPageNumber, treeFileName);

    if(!isLeaf)
    {
        leftChildren.isLeaf = false;
        rightChildren.isLeaf = false;

        memcpy(leftChildren.childTable, this->childTable, (d+1)* sizeof(int));
        memcpy(rightChildren.childTable, &this->childTable[d + 1], (d+1)* sizeof(int));
    }

    memcpy(leftChildren.indexTable, indexTable, d*sizeof(Index));
    memcpy(rightChildren.indexTable, &indexTable[d + 1], d*sizeof(Index));

    leftChildren.parentPageNumber = 0;
    rightChildren.parentPageNumber = 0;

    leftChildren.elemCount = d;
    rightChildren.elemCount = d;

    leftChildren.savePage();
    rightChildren.savePage();

    leftChildren.changeParent();
    rightChildren.changeParent();
    // second, we take care about root page
    indexTable[0] = indexTable[d];
    elemCount = 1;
    childTable[0] = leftChildren.pageNumber;
    childTable[1] = rightChildren.pageNumber;
    isLeaf = false;
    savePage();
}

int Page::splitPage(int firstPageNumber, int secondPageNumber)
{
    if(pageNumber == 0)
    {
        splitRootPage(firstPageNumber, secondPageNumber);
        return 2;
    }

    Page newPage(d, firstPageNumber, treeFileName);

    if (!isLeaf)
    {
        newPage.isLeaf = false;
        memcpy(newPage.childTable, &childTable[d + 1], (d + 1)* sizeof(int));
    }
    memcpy(newPage.indexTable, &indexTable[d + 1], d*sizeof(Index));

    newPage.parentPageNumber = parentPageNumber;
    newPage.elemCount = d;
    newPage.savePage();
    newPage.changeParent();

    elemCount = d;

    savePage();
    return 1;
}

void Page::printAllPages()
{
    FILE* file = fopen(treeFileName.c_str(), "rb");
    int counter = 0;
    while (fread(&parentPageNumber, sizeof(int), 1, file) > 0)
    {
        printf("Page %d\n", counter);
        fread(&elemCount, sizeof(int), 1, file);
        fread(&isLeaf, sizeof(bool), 1, file);
        fread(indexTable, sizeof(Index), 2 * d, file);
        fread(childTable, sizeof(int), 2 * d + 1, file);

        printf("Parent page Number: %d\n",parentPageNumber);
        printf("Element Count: %d\n", elemCount);
        printf("isLeaf: %s\n", isLeaf ? "true" : "false");
        printf("[Index Address]: ");
        for (int i = 0; i < 2*d; i++)
        {
            printf("[%d %d] ", this->indexTable[i].ind,this->indexTable[i].addr);
        }
        printf("\n");
        printf("Child: ");
        for (int i = 0; i < 2*d + 1; i++)
        {
            printf("%d ", this->childTable[i]);
        }
        printf("\n");
        counter++;
    }
    fclose(file);
}

void Page::copy(Page* copiedPage)
{
    this->elemCount = copiedPage->elemCount;
    this->isLeaf = copiedPage->isLeaf;
    memcpy(this->indexTable, copiedPage->indexTable, sizeof(Index) * (2 * d + 1));
    this->pageNumber = copiedPage->pageNumber;
    this->parentPageNumber = copiedPage->parentPageNumber;
    memcpy(this->childTable, copiedPage->childTable, sizeof(int) * (2 * d + 2));
}


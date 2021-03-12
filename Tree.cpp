#include <iostream>

#include "tree.h"
using namespace std;

Tree::Tree(int d, string treeFileName, string dataFileName)
    :d(d),treeFileName(treeFileName),dataFileName(dataFileName)
{
    // Create new empty files
    FILE* file = fopen(treeFileName.c_str(), "w");
    fclose(file);
    file = fopen(dataFileName.c_str(), "w");
    fclose(file);
}

int binarySearch(Page* page, int index)
{
    if (page->elemCount == 0)
    {
        return 0;
    }
    for(int i=0;i<page->elemCount;i++)
    {
        if(index<=page->indexTable[i].ind)
        {
            return i;
        }
    }
    return page->elemCount;
    /*int currPos = page->elemCount / 2;

    int pageAdd = page->elemCount / 4;
    if (pageAdd == 0)
    {
        pageAdd = 1;
    }
    while(1)
    {
        if(index == page->indexTable[currPos].ind)
        {
            return -1;
        }

        if (currPos == 0)
        {
            if (index < page->indexTable[currPos].ind)
            {
                return currPos;
            }
            else
                currPos += pageAdd;
        }

        if(currPos == page->elemCount -1)
        {
            if(index > page->indexTable[currPos].ind)
            {
                return currPos+1;
            }
            else
            {
                currPos -= pageAdd;
            }
        }

        if(index> page->indexTable[currPos-1].ind && index< page->indexTable[currPos].ind)
        {
            return currPos;
        }
        if(index > page->indexTable[currPos].ind)
        {
            currPos += pageAdd;
        }
        else
        {
            currPos -= pageAdd;
        }
        if (pageAdd != 1)
        {
            pageAdd /= 2;
        }
    }
    */
}

int Tree::getNextPageAddress()
{
    if(emptyPages.empty())
    {
        return pageNumber;
    }
    return emptyPages.back();
}

int Tree::getNextNextPageAddress()
{
    if (emptyPages.size() > 1)
    {
        return emptyPages[emptyPages.size() - 2];
    }
    else if (emptyPages.size() == 1)
        return pageNumber;
    else
        return pageNumber + 1;
}

void Tree::handeOnePageAdd()
{
    if(emptyPages.empty())
    {
        pageNumber++;
        return;
    }
    emptyPages.pop_back();
}

int Tree::find(int index)
{
    lastDiskOperation = 0;
    if (empty)
    {
        printf("tree is empty\n");
        return -1;
    }
    Page* currentPage = new Page(d, 0, treeFileName);
    currentPage->readPage(0);
    lastDiskOperation++;
    Index value;
    int ret = findPosiotion(currentPage, index);
    if(ret ==-1)
    {
        value = currentPage->indexTable[binarySearch(currentPage, index)];
        if(value.ind!=index)
        {
            printf("Index not found: %d\n", index);
            return -1;
        }
    }
    else
    {
        value = currentPage->indexTable[ret];
    }

    Record newRecord(value.addr, dataFileName);
    newRecord.print();
    delete currentPage;
    totalDiskOperation += lastDiskOperation;
    return value.addr;
}

void Tree::modify(int index, int record[Record::PARAM])
{
    int address = find(index);
    if(address==-1)
    {
        printf("Not Found\n");
    }
    Record newRecord(address, dataFileName);
    memcpy(newRecord.record, record, sizeof(int) * Record::PARAM);
    newRecord.writeRecord();
}


int Tree::findPosiotion(Page * currentPage, int index)
{
    int currentPageNumber;
    while (!currentPage->isLeaf)
    {
        int position = binarySearch(currentPage, index);
        if (currentPage->indexTable[position].ind == index)
        {
            return position;
        }
        else
        {
            currentPageNumber = currentPage->childTable[position];
            currentPage->readPage(currentPageNumber);
            lastDiskOperation++;
        }
    }
    return -1;
}

int Tree::add()
{
    int index = 0;
    // First record in file

    cout << "Give index of record";
    cin >> index;
    return add(index);
}

int Tree::add(int index)
{
    int record[Record::PARAM];
    for(int i=0;i<Record::PARAM;i++)
    {
        record[i] = rand();
    }
    return add(index, record);
}

int Tree::getNewAddress()
{
    if(emptyAddresses.empty())
    {
        return recordNumber++;
    }
    else
    {
        int value = emptyAddresses.back();
        emptyAddresses.pop_back();
        return value;
    }
}
int Tree::add(int index, int record[Record::PARAM])
{
    lastDiskOperation = 0;
    int address;
    int currentPageNumber = 0;

    address = getNewAddress();

    Record newRecord(index,  address,record, dataFileName);
    Page *currentPage = nullptr;
    if(empty)
    {
        pageNumber++;
        currentPage = new Page(d, currentPageNumber, treeFileName);
        empty = false;
    }
    else
    {
        currentPage = new Page(d, currentPageNumber, treeFileName);
        currentPage->readPage(currentPageNumber);
        lastDiskOperation++;
    }

    if (findPosiotion(currentPage, index) != -1)
        return 1;

    Index newIndex;
    newIndex.ind = index;
    newIndex.addr = address;

    newRecord.writeRecord();
    addToPage(currentPage, newIndex);
    delete currentPage;
    totalDiskOperation += lastDiskOperation;
    return 0;
}

int Tree::addCompensation(Page *currentPage)
{
    Page* parentPage = new Page(d, currentPage->parentPageNumber, treeFileName);
    Page* brotherPage = new Page(d, currentPage->parentPageNumber, treeFileName);
    parentPage->readPage(currentPage->parentPageNumber);
    lastDiskOperation++;

    // 1-left compensation
    // 2-right compensation
    int compensationWay = 0;
    int position = -1;
    // check if left brothers have space
    for(int i=0;i<parentPage->elemCount + 1;i++)
    {
        if (parentPage->childTable[i] == currentPage->pageNumber)
            position=i;
        else if(compensationWay==0)
        {
            brotherPage->readPage(parentPage->childTable[i]);
            lastDiskOperation++;

            if (brotherPage->elemCount <2*d)
            {
                if (position == -1)
                    compensationWay = 1;
                else
                    compensationWay = 2;
            }
        }
    }

    if(compensationWay==1)
    {
        for(int i=position-1;i>=0;i--)
        {
            brotherPage->readPage(parentPage->childTable[i]);
            lastDiskOperation++;
            brotherPage->indexTable[brotherPage->elemCount] = parentPage->indexTable[i];
            parentPage->indexTable[i] = currentPage->indexTable[0];
            for (int j = 0; j < currentPage->elemCount - 1; j++)
            {
                swap(currentPage->indexTable[j], currentPage->indexTable[j + 1]);
            }
            if (!currentPage->isLeaf)
            {
                brotherPage->childTable[brotherPage->elemCount+1] = currentPage->childTable[0];
                brotherPage->chageParentForPage(brotherPage->childTable[brotherPage->elemCount+1]);
                for (int j = 0; j < currentPage->elemCount; j++)
                {
                    swap(currentPage->childTable[j], currentPage->childTable[j + 1]);
                }
            }
            currentPage->elemCount--;
            brotherPage->elemCount++;
            currentPage->savePage();
            lastDiskOperation++;
            if(brotherPage->elemCount<=2*d)
            {
                brotherPage->savePage();
                lastDiskOperation++;
                parentPage->savePage();
                lastDiskOperation++;
                break;
            }
            else
            {
                currentPage->copy(brotherPage);
            }
        }
    }
    else if(compensationWay==2)
    {
        for (int i = position + 1; i<=parentPage->elemCount; i++)
        {
            brotherPage->readPage(parentPage->childTable[i]);
            lastDiskOperation++;

            for (int j = brotherPage->elemCount; j >0; j--)
            {
                swap(brotherPage->indexTable[j-1], brotherPage->indexTable[j]);
            }
            if(!currentPage->isLeaf)
            {
                for (int j = brotherPage->elemCount+1; j > 0; j--)
                {
                    swap(brotherPage->childTable[j - 1], brotherPage->childTable[j]);
                }
                brotherPage->childTable[0] = currentPage->childTable[currentPage->elemCount];
                brotherPage->chageParentForPage(brotherPage->childTable[0]);
            }
            brotherPage->indexTable[0] = parentPage->indexTable[i-1];
            currentPage->elemCount--;
            parentPage->indexTable[i-1] = currentPage->indexTable[currentPage->elemCount];
            brotherPage->elemCount++;
            currentPage->savePage();
            lastDiskOperation++;
            if (brotherPage->elemCount <= 2 * d)
            {
                brotherPage->savePage();
                lastDiskOperation++;
                parentPage->savePage();
                lastDiskOperation++;
                break;
            }
            else
            {
                currentPage->copy(brotherPage);
            }
        }
    }
    else
    {
        delete parentPage;
        delete brotherPage;
        return 0;
    }
    delete parentPage;
    delete brotherPage;
    return 1;
}

void Tree::addToPage(Page *currentPage, Index newIndex, int newPageAddress)
{
    int newPos = binarySearch(currentPage, newIndex.ind);

    if(newPageAddress!=-1)
    {
        for (int i = newPos+1; i < currentPage->elemCount + 1;i++)
        {
            swap(newPageAddress, currentPage->childTable[i]);
        }
        currentPage->childTable[currentPage->elemCount + 1] = newPageAddress;
    }
    for (int i = newPos; i < currentPage->elemCount; i++)
    {
        swap(newIndex, currentPage->indexTable[i]);
    }

    currentPage->indexTable[currentPage->elemCount] = newIndex;
    currentPage->elemCount++;



    //page is too big, we need to do split
    if (currentPage->elemCount > 2 * d)
    {
        if (currentPage->pageNumber != 0)
        {
            // try compensation
            if (addCompensation(currentPage))
            {
                return;
            }
        }
        newPageAddress = getNextPageAddress();
        if (currentPage->isLeaf)
            lastDiskOperation += d;
        const int pageAdded = currentPage->splitPage(newPageAddress, getNextNextPageAddress());
        lastDiskOperation += pageAdded;
        //case of add to root
        if (pageAdded == 2)
        {
            handeOnePageAdd();
            handeOnePageAdd();
        }
        //else split
        else if (pageAdded == 1)
        {
            
            handeOnePageAdd();
            newIndex = currentPage->indexTable[d];
            currentPage->savePage();
            lastDiskOperation++;
            currentPage->readPage(currentPage->parentPageNumber);
            lastDiskOperation++;
            addToPage(currentPage, newIndex, newPageAddress);
        }
    }
    else
    {
        currentPage->savePage();
        lastDiskOperation++;
    }
}

void Tree::print()
{
    if (empty)
        printf("Empty tree!");
    Page page = Page(d, 0, treeFileName);
    page.print();
}

void Tree::deleteRecord(int index)
{
    lastDiskOperation = 0;
    if(empty)
        return;

    Page* currentPage = new Page(d, 0, treeFileName);
    currentPage->readPage(0);
    lastDiskOperation++;
    findPosiotion(currentPage, index);

    if(!currentPage->isLeaf)
    {
        replaceRecord(currentPage, index);
    }

    deleteRecordOnPage(currentPage, index);
    totalDiskOperation +=lastDiskOperation;
}

void Tree::replaceRecord(Page* currentPage, int index)
{
    Page* parentPage = new Page(d, currentPage->pageNumber, treeFileName);
    parentPage->readPage(currentPage->pageNumber);
    lastDiskOperation++;
    int position = binarySearch(parentPage, index);
    currentPage->readPage(parentPage->childTable[position]);
    lastDiskOperation++;
    while(!currentPage->isLeaf)
    {
        currentPage->readPage(currentPage->childTable[currentPage->elemCount]);
        lastDiskOperation++;
    }
    swap(currentPage->indexTable[currentPage->elemCount - 1], parentPage->indexTable[position]);
    parentPage->savePage();
    lastDiskOperation++;
    delete parentPage;
}

int Tree::deleteCompensation(Page *currentPage)
{
    Page* parentPage = new Page(d, currentPage->parentPageNumber, treeFileName);
    Page* brotherPage = new Page(d, currentPage->parentPageNumber, treeFileName);
    parentPage->readPage(currentPage->parentPageNumber);
    lastDiskOperation++;
    // 1-left compensation
    // 2-right compensation
    int compensationWay = 0;
    int position = -1;
    // check if left brothers have space
    for (int i = 0; i < parentPage->elemCount + 1; i++)
    {
        if (parentPage->childTable[i] == currentPage->pageNumber)
            position = i;
        else if (compensationWay == 0)
        {
            brotherPage->readPage(parentPage->childTable[i]);
            lastDiskOperation++;
            if (brotherPage->elemCount > d)
            {
                if (position == -1)
                    compensationWay = 1;
                else
                    compensationWay = 2;
            }
        }
    }

    if (compensationWay == 1)
    {
        for (int i = position - 1; i >= 0; i--)
        {
            brotherPage->readPage(parentPage->childTable[i]);
            lastDiskOperation++;
            for (int j = currentPage->elemCount; j > 0; j--)
            {
                swap(currentPage->indexTable[j - 1], currentPage->indexTable[j]);
            }
            if (!currentPage->isLeaf)
            {
                for (int j = currentPage->elemCount + 1; j > 0; j--)
                {
                    swap(currentPage->childTable[j - 1], currentPage->childTable[j]);
                }
                currentPage->childTable[0] = brotherPage->childTable[brotherPage->elemCount];
                currentPage->chageParentForPage(currentPage->childTable[0]);
            }
            currentPage->indexTable[0] = parentPage->indexTable[i];
            brotherPage->elemCount--;
            parentPage->indexTable[i] = brotherPage->indexTable[brotherPage->elemCount];
            currentPage->elemCount++;
            currentPage->savePage();
            lastDiskOperation++;
            if (brotherPage->elemCount >= d)
            {
                brotherPage->savePage();
                lastDiskOperation++;
                parentPage->savePage();
                lastDiskOperation++;
                break;
            }
            else
            {
                currentPage->copy(brotherPage);
            }
        }
    }
    else if (compensationWay == 2)
    {
        for (int i = position + 1; i <= parentPage->elemCount; i++)
        {
            brotherPage->readPage(parentPage->childTable[i]);
            lastDiskOperation++;
            currentPage->indexTable[currentPage->elemCount] = parentPage->indexTable[i-1];
            parentPage->indexTable[i-1] = brotherPage->indexTable[0];
            for (int j = 0; j < brotherPage->elemCount - 1; j++)
            {
                swap(brotherPage->indexTable[j], brotherPage->indexTable[j + 1]);
            }
            if (!currentPage->isLeaf)
            {
                currentPage->childTable[currentPage->elemCount + 1] = brotherPage->childTable[0];
                currentPage->chageParentForPage(currentPage->childTable[currentPage->elemCount + 1]);
                for (int j = 0; j < brotherPage->elemCount; j++)
                {
                    swap(brotherPage->childTable[j], brotherPage->childTable[j + 1]);
                }
            }
            brotherPage->elemCount--;
            currentPage->elemCount++;
            currentPage->savePage();
            lastDiskOperation++;
            if (brotherPage->elemCount >= d)
            {
                brotherPage->savePage();
                lastDiskOperation++;
                parentPage->savePage();
                lastDiskOperation++;
                break;
            }
            else
            {
                currentPage->copy(brotherPage);
            }
        }
    }
    else
    {
        delete parentPage;
        delete brotherPage;
        return 0;
    }
    delete parentPage;
    delete brotherPage;
    return 1;
}

void Tree::deleteRecordOnPage(Page* currentPage, int index)
{
    if (currentPage->isLeaf)
    {
        int position = binarySearch(currentPage, index);
        if(currentPage->elemCount == position)
        {
            cout << "Index does not exist: " << index << endl;
            return;
        }
        for(int i=position;i<currentPage->elemCount-1;i++)
        {
            swap(currentPage->indexTable[i], currentPage->indexTable[i + 1]);
        }
        emptyAddresses.push_back(currentPage->indexTable[currentPage->elemCount-1].addr);
        currentPage->elemCount--;
    }

    if(currentPage->elemCount<d)
    {
        // try compensation first
        if (currentPage->pageNumber != 0)
        {
            if (deleteCompensation(currentPage))
            {
                return;
            }
            else
            {
                mergePage(currentPage);
                deleteRecordOnPage(currentPage, -1);
            }
        }
        else if(currentPage->elemCount == 0)
        {
            //root become empty no more element in tree
            if(index>=0)
            {
                empty = true;
                pageNumber = 0;
                recordNumber = 0;
                emptyPages.clear();
                emptyAddresses.clear();
                currentPage->savePage();
                lastDiskOperation++;
                return;
            }
            // child 0 should become new root
            Page* newRoot = new Page(d, 0, treeFileName);
            newRoot->readPage(currentPage->childTable[0]);
            lastDiskOperation++;
            emptyPages.push_back(newRoot->pageNumber);
            newRoot->pageNumber = 0;
            lastDiskOperation+=newRoot->changeParent();
            newRoot->savePage();
            lastDiskOperation++;
            delete newRoot;
        }
        else
        {
            currentPage->savePage();
            lastDiskOperation++;
        }
    }
    else
    {
        currentPage->savePage();
        lastDiskOperation++;
    }
}

void Tree::mergePage(Page* &currentPage)
{
    Page* parentPage = new Page(d, currentPage->parentPageNumber, treeFileName);
    Page* brotherPage = new Page(d, currentPage->parentPageNumber, treeFileName);
    int position = 0;
    parentPage->readPage(currentPage->parentPageNumber);
    lastDiskOperation++;
    for(int i=0;i<parentPage->elemCount+1;i++)
    {
        if(parentPage->childTable[i]==currentPage->pageNumber)
        {
            position = i;
            break;
        }
    }

    if(position == 0)
    {
        brotherPage->readPage(parentPage->childTable[++position]);
        lastDiskOperation++;
        swap(currentPage, brotherPage);
    }
    else
    {
        brotherPage->readPage(parentPage->childTable[position - 1]);
        lastDiskOperation++;
    }

    brotherPage->indexTable[brotherPage->elemCount] = parentPage->indexTable[position-1];
    memcpy(&brotherPage->indexTable[brotherPage->elemCount + 1], currentPage->indexTable, sizeof(Index) * currentPage->elemCount);
    memcpy(&brotherPage->childTable[brotherPage->elemCount + 1], currentPage->childTable, sizeof(int) * (currentPage->elemCount+1));
    brotherPage->elemCount = brotherPage->elemCount + 1 + currentPage->elemCount;
    if (brotherPage->elemCount != 2 * d)
        cout << "ERROR when merge";
    for(int i=position-1;i<parentPage->elemCount - 1;i++)
    {
        swap(parentPage->indexTable[i], parentPage->indexTable[i + 1]);
    }
    for (int i = position; i < parentPage->elemCount; i++)
    {
        swap(parentPage->childTable[i], parentPage->childTable[i + 1]);
    }
    parentPage->elemCount--;
    lastDiskOperation+=brotherPage->changeParent();
    brotherPage->savePage();
    lastDiskOperation++;
    emptyPages.push_back(currentPage->pageNumber);
    swap(currentPage, parentPage);
    delete parentPage;
    delete brotherPage;
}

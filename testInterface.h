#pragma once
#include "tree.h"
#include <fstream>
#include <iostream>

class TestInterface
{
    int d;
    Tree tree;
    std::string inputFile;
    std::string dataFileName;
    std::string treeFileName;
public:
    TestInterface(Tree tree, std::string inputFile, std::string dataFileName,std::string treeFileName,int d)
        :tree(tree),inputFile(inputFile),dataFileName(dataFileName),treeFileName(treeFileName),d(d)
    {
        
    }

    void performTest()
    {
        std::fstream file(inputFile);
        char operation;
        int index;
        int record[Record::PARAM];
        Record records(0, dataFileName);
        Page page(d, 0, treeFileName);
        std::fstream log("logfile.txt");
        while(file>>operation)
        {
            switch (operation)
            {
            case 'a':
                file >> index;
                for (int i = 0; i < Record::PARAM; i++)
                {
                    file >> record[i];
                }
                if (tree.add(index, record) == 1)
                {
                    printf("Index exist!!!\n");
                }
                break;

            case 'i':
                file >> index;
                if (tree.add(index) == 1)
                {
                    printf("Index exist!!!\n");
                }
                break;


            case 'p':
                tree.print();
                break;

            case 'r':
                file >> index;
                tree.find(index);
                break;
            case 'f':
                records.printAllRecord();
                break;

            case 's':
                page.printAllPages();
                break;

            case 'k':
                file >> index;
                tree.deleteRecord(index);
                break;

            case 't':
                printf("Total disk operation: %d\n", tree.totalDiskOperation);
                break;
            case 'l':
                printf("Last disk operation: %d\n", tree.lastDiskOperation);
                break;
            case 'm':
                file >> index;
                for (int i = 0; i < Record::PARAM; i++)
                {
                    file >> record[i];
                }
                tree.modify(index, record);
                break;
            case 'z':
                system("pause");
                break;
            case 'd':
                int count = 0;
                page.printDebug(count);
                printf("\ncount = %d\n", count);
                break;
            }
        }

        while (true)
        {
            printf("mozliwe operacje:\n");
            printf("a. dodanie indeksu i rekordu\n");
            printf("i. dodanie indeksu i wylosowanie rekordu\n");
            printf("r. znalezienie indeksu i rekordu\n");
            printf("k. usuniecie indeksu i rekordu\n");
            printf("m. modyfikacja rekordu\n");
            std::cin >> operation;

            switch (operation)
            {
            case 'a':
                std::cin >> index;
                for (int i = 0; i < Record::PARAM; i++)
                {
                    std::cin >> record[i];
                }
                if (tree.add(index, record) == 1)
                {
                    printf("Index exist!!!\n");
                }
                break;

            case 'i':
                std::cin >> index;
                if (tree.add(index) == 1)
                {
                    printf("Index exist!!!\n");
                }
                break;

            case 'r':
                std::cin >> index;
                tree.find(index);
                break;

            case 'k':
                std::cin >> index;
                tree.deleteRecord(index);
                break;

            case 'm':
                std::cin >> index;
                for (int i = 0; i < Record::PARAM; i++)
                {
                    std::cin >> record[i];
                }
                tree.modify(index, record);
                break;

            }
            printf("Czy chcesz wyswietlic drzewo:\n");
            printf("1. tak\n");
            printf("2. nie\n");
            std::cin >> operation;
            if(operation=='1')
            {
                tree.print();
                printf("Czy chcesz wyswietlic pliki i operacje dyskowe:\n");
                printf("1. tak\n");
                printf("2. nie\n");
                std::cin >> operation;
                if (operation == '1')
                {
                    records.printAllRecord();
                    system("pause");
                    page.printAllPages();
                    printf("Total disk operation: %d\n", tree.totalDiskOperation);
                    printf("Last disk operation: %d\n", tree.lastDiskOperation);
                }
                
            }
        }
    }
};

#pragma once
#include <stdio.h>
#include <string>

class Record
{
public:
    static constexpr int PARAM = 5;
    static constexpr int recordSize = (1 + PARAM) * sizeof(int);
    int index;
    int address;
    int record[5];

    std::string dataFileName;

    Record(int index, int address, int record[PARAM], std::string dataFileName)
    {
        this->dataFileName = dataFileName;
        this->index = index;
        memcpy_s(this->record, sizeof(int) * PARAM, record, sizeof(int) * PARAM);
        this->address = address;
    }
    
    Record(int address, std::string dataFileName)
    {
        this->dataFileName = dataFileName;
        this->address = address;
        readRecord();
    }

    void print()
    {
        printf("%d: ", index);
        for(int i=0;i<PARAM;i++)
        {
            printf("%d ", record[i]);
        }
        printf("\n");
    }

    void readRecord()
    {
        FILE* file = fopen(dataFileName.c_str(), "r+b");
        fseek(file, address*recordSize, 0);
        fread(&index, sizeof(int), 1,file);
        fread(record, sizeof(int), PARAM, file);
        fclose(file);
    }

    void writeRecord()
    {
        FILE* file = fopen(dataFileName.c_str(), "r+b");
        fseek(file, address * recordSize, 0);
        fwrite(&index, sizeof(int), 1, file);
        fwrite(record, sizeof(int), PARAM, file);
        fclose(file);
    }

    void printAllRecord()
    {
        printf("Index: Record\n");
        FILE* file = fopen(dataFileName.c_str(), "rb");
        while(fread(&index, sizeof(int), 1, file)>0)
        {
            fread(record, sizeof(int), PARAM, file);
            print();
        }
    }
};

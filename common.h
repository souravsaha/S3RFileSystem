#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define SIZE 20*1024
#define DATA_BLOCK_SIZE 16
#define DATA_BLOCK_NUMBER 13

typedef struct DataBlock
{
    char content[DATA_BLOCK_SIZE];
};

typedef struct DataAccess
{
    int dbIndex[DATA_BLOCK_NUMBER];
};

typedef struct Inode
{
    int fileMode;
    int linkCount;
    int blocksCount;
    
    DataAccess* da;

};

typedef struct SuperBlock
{
    // int magic;

    //
    int inodeCount;
    int freeInodeCount;
    int iNodeOffset;
    int dataBlockOffset;
    int dataBlockCount;
    int freeDataBlockCount;

    // lists
    int* inodeList;
    int* dataBlockList;

    int iNodeSize;
    int dataBlockSize;
};


#endif
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>

#define SIZE 1024*1024
#define DATA_BLOCK_SIZE 128
#define DATA_BLOCK_NUMBER 13
#define FILE_SYSTEM_ALREADY_INITIALIZED 67895


// just buffer
typedef struct DATABLOCK
{
    char content[DATA_BLOCK_SIZE];
}DataBlock;


typedef struct INODE
{
    int fileMode;
    int linkCount;
    int blocksCount;
    
    int dbIndex[DATA_BLOCK_NUMBER];

}Inode;

typedef struct SUPERBLOCK
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
}SuperBlock;

typedef struct WHOLEFS
{
    SuperBlock* sb;
    DataBlock* db;
    Inode* ib;
}WholeFS;
#endif
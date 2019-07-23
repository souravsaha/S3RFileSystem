#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


#define SIZE 1024*1024
#define DATA_BLOCK_SIZE 16
#define DATA_BLOCK_NUMBER 13

typedef struct DATABLOCK
{
    char content[DATA_BLOCK_SIZE];
}DataBlock;

typedef struct DATAACCESS
{
    int dbIndex[DATA_BLOCK_NUMBER];
}DataAccess;

typedef struct INODE
{
    int fileMode;
    int linkCount;
    int blocksCount;
    
    DataAccess* da;

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


#endif
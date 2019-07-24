#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

#define SIZE 1024*1024
#define DATA_BLOCK_SIZE 128
#define DIRECT_DATA_BLOCK_NUMBER 32
#define FILE_SYSTEM_ALREADY_INITIALIZED 67895
#define FILE_SYSTEM_NOT_INITIALIZED 89702

#define NO_OF_INODES 1024
#define NO_OF_DATA_BLOCKS 1024

#define FILE_MODE 0
#define FOLDER_MODE 1

#define FREE 0
#define OCCUPIED 1

#define DIRECTORY_ENTRY_LENGTH 16
// just buffer
typedef struct DATABLOCK
{
    char content[DATA_BLOCK_SIZE];
}DataBlock;


typedef struct INODE
{
    int fileMode; // file or directory
    int linkCount; // no of open instances
    int fileSize; // file size in bytes
    
    /*Later purpose */

    /*
    int uid;
    int gid;
    time_t aTime;
    time_t mTime;
    time_t cTime;
     */

    // list of data blocks
    int directDBIndex[DIRECT_DATA_BLOCK_NUMBER];
    //int indirect2DBIndex[2ND_DIRECT_DATA_BLOCK_NUMBER];

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
    int inodeList[NO_OF_INODES];
    int dataBlockList[NO_OF_DATA_BLOCKS];

    int iNodeSize;
    int dataBlockSize;
}SuperBlock;

typedef struct WHOLEFS
{
    SuperBlock sb;
    DataBlock db[NO_OF_DATA_BLOCKS];
    Inode ib[NO_OF_INODES];
}WholeFS;

#endif
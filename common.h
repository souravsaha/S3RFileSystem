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

#define Malloc(n,type) (type *) malloc( (unsigned) ((n)*sizeof(type)))

#define DIRECTORY_ENTRY_LENGTH 16
#define FILE_NAME "temp.txt"

/* Define some specific offsets for file handling 
#define INTEGER_SIZE 4

#define SUPER_BLOCK_START_OFFSET INTEGER_SIZE
#define SUPER_BLOCK_SIZE (8+1024+1024)*INTEGER_SIZE

#define INODE_SIZE ((32+3)*INTEGER_SIZE)
#define INODE_ARRAY_START_OFFSET (SUPER_BLOCK_START_OFFSET+SUPER_BLOCK_SIZE)
#define INODE_ARRAY_SIZE INODE_SIZE*NO_OF_INODES

#define DATA_BLOCK_ARRAY_START_OFFSET INODE_ARRAY_START_OFFSET+INODE_ARRAY_SIZE
#define DATA_BLOCK_ARRAY_SIZE DATA_BLOCK_SIZE*NO_OF_DATA_BLOCKS
*/
// just buffer
typedef struct DATABLOCK // size = 128
{
    char content[DATA_BLOCK_SIZE];
}DataBlock;


typedef struct INODE // 140 (35*4)
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

typedef struct SUPERBLOCK // size (8+1024+1024)*sizeof(int) = 8224
{
    // int magic;

    //inode
    int inodeCount;
    int freeInodeCount;
    int iNodeOffset;
    
    // data blocks
    int dataBlockOffset;
    int dataBlockCount;
    int freeDataBlockCount;

    // free lists
    int inodeList[NO_OF_INODES];
    int dataBlockList[NO_OF_DATA_BLOCKS];

    int iNodeSize;
    int dataBlockSize;
}SuperBlock;

typedef struct WHOLEFS
{
    SuperBlock sb;
    Inode ib[NO_OF_INODES];
    char* fileSystemName;
    DataBlock db[NO_OF_DATA_BLOCKS];
    
}WholeFS;

#endif
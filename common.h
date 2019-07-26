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

#define DIRECTORY_INDICATOR "/"
#define FREE 0
#define OCCUPIED 1

#define Malloc(n,type) (type *) malloc( (unsigned) ((n)*sizeof(type)))

#define DIRECTORY_ENTRY_LENGTH 16
#define FILE_NAME "S3R.fs"

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
    int pwdInodeNumber;

}WholeFS;

Inode* strToInode(char* buffer,int len);
int getPwdInodeNumber(WholeFS* fs);
Inode* strToInode(char* buffer,int len);
WholeFS* readFS(char *fileName);
void writeInode(WholeFS* fs,int index,int mode);
void writeDataBlock(WholeFS* fs,int index,int offset,char* content,int len);
DataBlock* readDataBlock(WholeFS* fs,int index);
int getFirstFreeInodeIndex(WholeFS* fs);
int getFirstFreeDataBlockIndex(WholeFS* fs);
int getParentInode(WholeFS* fs);
Inode* getInode(WholeFS* fs,int index);

int isDBlockFree(Inode* i,int index);
void calculateDataBlockNoAndOffsetToWrite(WholeFS* fs,Inode* i,int inodeIndex, int* index,int* offset);
int searchFilenameInDataBlock(char* db,char* name,int len);
int getDBlockNumberFromSize(int size);
void writeFS(WholeFS *fs, int inodeIndex);

#endif
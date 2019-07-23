#include "common.h"

// TODO take filesize as argument
// 
void initFS()
{
    FILE *fp = fopen("/tmp/S3R.fs", "w");
    int code = FILE_SYSTEM_ALREADY_INITIALIZED;

    fprintf(fp,"%d",code);
    fseek(fp, sizeof(code)+SIZE-1 , SEEK_SET);
    fputc('\0', fp);
    fclose(fp);

}

/*
* ReadFs returns the existing structure into memory.
* If Doesnt exist creates from scratch
* It also changes the starting code to mark the changes
*/

void readFS()
{
    // check if its a fresh copy
    FILE *fp = fopen("/tmp/S3R.fs", "w");
    int code = FILE_SYSTEM_ALREADY_INITIALIZED;
    int tmp;
    fscanf(fp,"%d",&tmp);

    if(tmp==code) // fresh
    {
        WholeFS* fs = calloc(1,sizeof(WholeFS));
        if(fs==NULL)
            assert(0);
        fs->sb = calloc(1,sizeof(SuperBlock));

        int noOfInodes = SIZE/(sizeof(Inode)*8); // 8% of total space allocated to inode
        fs->sb->inodeCount = noOfInodes;
        fs->sb->freeInodeCount = noOfInodes;
        fs->sb->iNodeSize = sizeof(Inode);
        fs->sb->dataBlockSize = 
        fs->ib = calloc(noOfInodes,sizeof(Inode));

        int dataBlockNum = (SIZE-(sizeof(SuperBlock)+noOfInodes*sizeof(Inode)))/DATA_BLOCK_SIZE;
        fs->sb->dataBlockCount = dataBlockNum;
        fs->sb->freeDataBlockCount = dataBlockNum;
        fs->sb->inodeList = calloc(DATA_BLOCK_SIZE*10, sizeof(int));
        fs->sb->dataBlockList = calloc(DATA_BLOCK_SIZE*10, sizeof(int));
        int sizeOfINodes = sizeof(Inode);
        fs->sb->iNodeOffset = SIZE- (dataBlockNum*DATA_BLOCK_SIZE + noOfInodes*sizeOfINodes)+sizeof(int);

        
    }
    else
    {
        
    }
    
    
}

void writeFS()
{

}

int main()
{
    initFS();
    return 0;
}
#include "common.h"

// TODO take filesize as argument
// 
void initFS()
{
    FILE *fp = fopen("/tmp/S3R.fs", "w+");
    int code = FILE_SYSTEM_NOT_INITIALIZED;

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
    FILE *fp = fopen("/tmp/S3R.fs", "r+");
    int code = FILE_SYSTEM_NOT_INITIALIZED;
    int tmp;

    //read the starting char
    //fseek(fp,0,SEEK_SET);
    fscanf(fp,"%d",&tmp);
    printf("%d",tmp);

    if(tmp==code) // fresh
    {
        WholeFS* fs = calloc(1,sizeof(WholeFS));
        if(fs==NULL)
            assert(0);
        

        // super block
        int noOfInodes = NO_OF_INODES; // 8% of total space allocated to inode
        fs->sb.inodeCount = noOfInodes;
        fs->sb.freeInodeCount = noOfInodes;
        fs->sb.iNodeSize = sizeof(Inode);
        fs->sb.dataBlockSize = DATA_BLOCK_SIZE;
        
        int dataBlockNum = NO_OF_DATA_BLOCKS;
        fs->sb.dataBlockCount = dataBlockNum;
        fs->sb.freeDataBlockCount = dataBlockNum;
        
        int sizeOfINodes = sizeof(Inode);
        
        fs->sb.iNodeOffset = SIZE - (dataBlockNum*DATA_BLOCK_SIZE + noOfInodes*sizeOfINodes)+sizeof(int);
        fs->sb.dataBlockOffset = fs->sb.iNodeOffset+fs->sb.inodeCount*fs->sb.iNodeSize;

        // write to beginning of file that the filesystem has already been written
        //printf("\nFile pointer position : %ld",ftell(fp));
        fseek(fp,0,SEEK_SET);
        //printf("\nFile pointer position : %ld",ftell(fp));
        //rewind(fp);
        tmp = FILE_SYSTEM_ALREADY_INITIALIZED;
        fprintf(fp,"%d",tmp);
        //printf("\nFile pointer position : %ld",ftell(fp));
        
    }
    else
    {
        
    }
    
    fclose(fp);
}

void writeFS()
{

}

int main()
{
    initFS();
    readFS();
    return 0;
}
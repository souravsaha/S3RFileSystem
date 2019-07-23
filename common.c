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

WholeFS* readFS()
{
    // check if its a fresh copy
    FILE *fp = fopen("/tmp/S3R.fs", "r+");
    int code = FILE_SYSTEM_NOT_INITIALIZED;
    int tmp;

    //read the starting char
    //fseek(fp,0,SEEK_SET);
    fscanf(fp,"%d",&tmp);
    printf("%d",tmp);

    WholeFS* fs = NULL:
    if(tmp==code) // fresh
    {
        fs = calloc(1,sizeof(WholeFS));
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
    else // read from the file to memory structure
    {
        
    }
    
    fclose(fp);
    return fs;
}

// set the file mode(file or folder) at the given index
void writeInode(WholeFS* fs,int index,int mode)
{
    fs->ib[index].fileMode = mode;
    fs->ib[index].linkCount = 1;

}

void writeDataBlock(WholeFS* fs,int index,char* content,int len)
{
    assert(len<=DATA_BLOCK_SIZE);
    DataBlock db = fs->db[index];

    int i;
    for(i=0;i<len;i++)
    {
        db[i] = content[i];
    }
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
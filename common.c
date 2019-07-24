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

    WholeFS* fs = NULL;
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

// given the data block and its index, writes it into the memory
void writeDataBlock(WholeFS* fs,int index,int offset,char* content,int len)
{
    assert(len<=DATA_BLOCK_SIZE);
    DataBlock db = fs->db[index];

    int i;
    for(i=0;i<len;i++)
    {
        db.content[i+offset] = content[i];
    }
}

DataBlock* readDataBlock(WholeFS* fs,int index)
{
    return &(fs->db[index]);
}

// find the first free inode from freelist
int getFirstFreeInode(WholeFS* fs)
{
    int i;
    // first two inodes are reserved
    for(i=2;i<fs->sb.inodeCount;i++)
    {
        if(fs->sb.inodeList[i]==FREE)
        {
            fs->sb.inodeList[i] = OCCUPIED;
            return i;
        }
    }
    assert(0);
}

// uses current working directory to get parent inode index 
int getParentInode(WholeFS* fs)
{
    return 1;
}

Inode* getInode(WholeFS* fs,int index)
{
    return &(fs->ib[index]);
}

void system_touch(WholeFS* fs,char* name)
{
    // get free inode
    int index = getFirstFreeInode(fs);
    
    // TODO : get the parent directory inode no
    int parent = getParentInode(fs);

    // write into data block of parent
    // assuming everything is in direct block
    Inode* parentInode = getInode(fs,parent);
    int blockNo = parentInode->fileSize/DATA_BLOCK_SIZE;
    int blockOffset = parentInode->fileSize%DATA_BLOCK_SIZE;

    assert(blockNo<DIRECT_DATA_BLOCK_NUMBER);

    // add a 16byte entry in the parent directory

    char buffer[16];
    sprintf(buffer,"%d %s\n",parent,name);

    // still can append in the last block
    if(blockOffset+16<DATA_BLOCK_SIZE)
    {
        // write to the specific data block
        writeDataBlock(fs,blockNo,blockOffset,buffer,16);
        DataBlock* d = readDataBlock(fs,blockNo);

        printf("%s\n",d->content);
        // print the content of the block 

    }else // new bloxk neededxxxxxxx
    {
        assert(0);
    }

}


void writeFS()
{

}

int main()
{
    initFS();
    WholeFS* fs = readFS();
    system_touch(fs,"try.txt");
    return 0;
}
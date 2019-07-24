#include "common.h"

int getPwdInodeNumber(WholeFS* fs)
{
    return 1;
}
// TODO take filesize as argument
// 
void initFS()
{
    FILE *fp = fopen("S3R.fs", "w+");
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
    FILE *fp = fopen("S3R.fs", "r+");
    int code = FILE_SYSTEM_NOT_INITIALIZED;
    int tmp;

    //read the starting char
    //fseek(fp,0,SEEK_SET);
    fscanf(fp,"%d",&tmp);
    //printf("%d",tmp);

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

        /* root inode need to be initialized */
        int pwdInodeNumber = getPwdInodeNumber(fs);
        fs->ib[pwdInodeNumber].fileMode = FOLDER_MODE;
        fs->ib[pwdInodeNumber].linkCount = 1;
        fs->ib[pwdInodeNumber].directDBIndex[0] = pwdInodeNumber;

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
    DataBlock* db = &(fs->db[index]);
    //printf("\n add: %u",db);
    int i;
    for(i=0;i<len;i++)
    {
        db->content[i+offset] = content[i];
    }
}

DataBlock* readDataBlock(WholeFS* fs,int index)
{
    //printf("\n add: %u",&(fs->db[index]));
    return &(fs->db[index]);
}

// find the first free inode index from freelist
// mark it allocated
int getFirstFreeInodeIndex(WholeFS* fs)
{
    int i;
    if(fs->sb.freeInodeCount==0)
        assert(0);

    // first two inodes are reserved
    for(i=2;i<fs->sb.inodeCount;i++)
    {
        if(fs->sb.inodeList[i]==FREE)
        {
            //fs->sb.inodeList[i] = OCCUPIED;
            //fs->sb.freeInodeCount--;
            return i;
        }
    }
    assert(0);
}

// find the first free data block index from freelist
// mark it allocated
int getFirstFreeDataBlockIndex(WholeFS* fs)
{
    int i;
    if(fs->sb.freeDataBlockCount==0)
        assert(0);
        
    // first two data blocks are reserved
    for(i=2;i<fs->sb.dataBlockCount;i++)
    {
        printf("%d = %d \n", i, fs->sb.dataBlockList[i]);
        if(fs->sb.dataBlockList[i]==FREE)
        {
            //fs->sb.dataBlockList[i] = OCCUPIED;
            //fs->sb.freeDataBlockCount--;
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

int isDBlockFree(Inode* i,int index)
{
    // check if the list of data block indexes in inode is 
    return i->directDBIndex[index]==FREE;
}
// calculate block number and offset to write data into
// if ending of the block is reached new block allocated
void calculateDataBlockNoAndOffsetToWrite(WholeFS* fs,Inode* i,int inodeIndex, int* index,int* offset)
{
    int blockIndexInInode = i->fileSize/DATA_BLOCK_SIZE;
    int blockOffset = i->fileSize%DATA_BLOCK_SIZE;
    int idx = i->directDBIndex[blockIndexInInode];
    if(blockIndexInInode>=DIRECT_DATA_BLOCK_NUMBER)
        assert(0); // just to avoid double links
    

    //if(blockOffset == 0) // need to write at data block beginning
    //{
    // check if the block is already allocated or not
    //if(isDBlockFree(i,blockIndexInInode))
    //{
    idx = getFirstFreeDataBlockIndex(fs);
    fs->sb.dataBlockList[idx] = OCCUPIED;
    fs->sb.freeDataBlockCount--;
    //i->directDBIndex[blockIndexInInode] = idx;
    //}

    //}
    
    *index = idx;
    *offset = blockOffset;
    
}


int system_touch(WholeFS* fs,char* name)
{
    /*Make the file */
    
    int inodeIndex = getFirstFreeInodeIndex(fs); // get free inode
    
    fs->sb.inodeList[inodeIndex] = OCCUPIED;
    fs->ib[inodeIndex].fileMode = FILE_MODE; // this is a file
    fs->ib[inodeIndex].fileSize = 0;
    fs->ib[inodeIndex].linkCount = 1;
    fs->sb.freeInodeCount--;

    /* Add an entry in parent */
    // TODO : get the parent directory inode no
    int parent = getParentInode(fs);
    //printf("Before touch : %d\n",fs->ib[parent].fileSize);
    // write into data block of parent
    // assuming everything is in direct block
    Inode* parentInode = getInode(fs,parent);
    
    /* This is a temporary solution.. 
        Write a function like write buffer that uses a while loop to write data to
        data buffers where data may not fit in a single data block
        can be written simply by using existing functions "calculateDataBlockNoAndOffsetToWrite" and 
        code from this function
     */

    int blockNo,blockOffset;

    calculateDataBlockNoAndOffsetToWrite(fs,parentInode,inodeIndex, &blockNo,&blockOffset);
    
    assert(blockNo<DIRECT_DATA_BLOCK_NUMBER);

    fs->ib[inodeIndex].directDBIndex[0] = blockNo;
    
    // add a 16byte entry in the parent directory
    printf("\nblockNo = %d offset= %d\n",blockNo,blockOffset);

    char buffer[16];
    sprintf(buffer,"%d %s\n",blockNo,name);
    int pwdInodeNumber = getPwdInodeNumber(fs);

    // still can append in the last block
    if(blockOffset+16<DATA_BLOCK_SIZE)
    {
        // write to the specific data block
        writeDataBlock(fs, pwdInodeNumber, blockOffset, buffer, 16);

        // increase the file size
        // Don't do it here! Will be taken care by writefs ;
        //parentInode->fileSize += DIRECTORY_ENTRY_LENGTH;
        

        DataBlock* d = readDataBlock(fs,pwdInodeNumber);
        
        //printf("\n add: %u",d);
        printf("%s\n",d->content);
        // print the content of the block 

    }else // new bloxk needed
    {
        assert(0);
    }
    //printf("Direct DB Index, inode index : %d, value = %d\n", inodeIndex, fs->ib[inodeIndex].directDBIndex[0]);
    return inodeIndex;
}

int getDBlockNumberFromSize(int size)
{
    return 0;
}

void writeFS(WholeFS *fs, int inodeIndex)
{
    FILE *fp = fopen("S3R.fs", "r+");
    int rootInodeIndex =  getPwdInodeNumber(fs);
    int offset = fs->ib[rootInodeIndex].fileSize % DATA_BLOCK_SIZE;
    int i;
    
    //printf("FileSize after touch : %d\n", fs->ib[rootInodeIndex].fileSize);
    // write data block in the file    
    fseek(fp, sizeof(int), SEEK_SET);
    fseek(fp, fs->sb.dataBlockOffset, SEEK_CUR);
    fseek(fp, getDBlockNumberFromSize(fs->ib[rootInodeIndex].fileSize) * DATA_BLOCK_SIZE, SEEK_CUR);
    fseek(fp, offset , SEEK_CUR);
    
    //printf("Data Block Offset : %d \n",fs->sb.dataBlockOffset);
    
    printf("offset value : %d\n", offset);
    
    DataBlock* db = &(fs->db[rootInodeIndex]);
    for( i = 0 ; i < DIRECTORY_ENTRY_LENGTH ; i++)
    {
        printf("%c", db->content[i + offset]);
        fputc(db->content[i + offset], fp);
    }

    fs->ib[rootInodeIndex].fileSize += DIRECTORY_ENTRY_LENGTH;

    // write inode in the file
    fseek(fp, sizeof(int), SEEK_SET);

    fseek(fp, fs->sb.iNodeOffset, SEEK_CUR);
    fseek(fp, rootInodeIndex * fs->sb.iNodeSize, SEEK_CUR);
    
    fprintf(fp, "%d ", fs->ib[rootInodeIndex].fileMode);
    printf("%d ", fs->ib[rootInodeIndex].fileMode);
    fprintf(fp, "%d ", fs->ib[rootInodeIndex].linkCount);
    printf("%d ", fs->ib[rootInodeIndex].linkCount);
    fprintf(fp, "%d ", fs->ib[rootInodeIndex].fileSize);
    printf("%d ", fs->ib[rootInodeIndex].fileSize);
    
    for(i = 0; i < DIRECT_DATA_BLOCK_NUMBER; i++)
        fprintf(fp, "%d ", fs->ib[rootInodeIndex].directDBIndex[i]);
    

    fseek(fp, sizeof(int), SEEK_SET);

    fseek(fp, fs->sb.iNodeOffset, SEEK_CUR);
    fseek(fp, inodeIndex * fs->sb.iNodeSize, SEEK_CUR);
 
    fprintf(fp, "%d ", fs->ib[inodeIndex].fileMode);
    fprintf(fp, "%d ", fs->ib[inodeIndex].linkCount);
    fprintf(fp, "%d ", fs->ib[inodeIndex].fileSize);

    for(i = 0; i<DIRECT_DATA_BLOCK_NUMBER; i++)
        fprintf(fp, "%d ", fs->ib[inodeIndex].directDBIndex[i]);
    fclose(fp);
}

int main()
{
    int inodeIndex;
    initFS();
    WholeFS* fs = readFS();
    inodeIndex = system_touch(fs,"try1.txt");
    writeFS(fs, inodeIndex);
    inodeIndex = system_touch(fs,"try2.txt");
    writeFS(fs, inodeIndex);
    return 0;
}
#include "common.h"
#include "file_handling_functions.h"

int getPwdInodeNumber(WholeFS* fs)
{
    return 1;
}
// TODO take filesize as argument
// 
void initFS(char *fileName)
{
    FILE *fp = fopen(fileSystemName, "w+");
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

WholeFS* readFS(char *fileName)
{
    // check if its a fresh copy
    FILE *fp = fopen(fileName, "r+");
    int code = FILE_SYSTEM_NOT_INITIALIZED;
    int tmp;

    //read the starting char
    //fseek(fp,0,SEEK_SET);
    fscanf(fp,"%d",&tmp);
    //printf("%d",tmp);

    WholeFS* fs = NULL;
    fs->fileSystemName = Malloc(512, char);
    strcpy(fs->fileSystemName, fileName);
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
        
        fs->sb.iNodeOffset = sizeof(SuperBlock)+sizeof(int);
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

/* TODO complete the function */
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

// given a block, searches filename in the block
// returns the offset where the block is found 
// else -1
int searchFilenameInDataBlock(char* db,char* name,int len)
{
    char* ptr = strstr(db,name);
    if(ptr)
    {
        
    }
    else
        return -1;
    
}

int system_rm(WholeFS* fs,char* name,int len)
{
    // get current inode number of current working directory
    int dirInode = getPwdInodeNumber(fs);
    // get Inode of the pwd directory
    Inode* parentDirInode = getInode(fs,dirInode);
    int totalBlocksOccupied = parentDirInode->fileSize/DATA_BLOCK_SIZE;
    
    // only 1st level link
    assert(totalBlocksOccupied<=DIRECT_DATA_BLOCK_NUMBER);

    // get the data block of the directory files 
    int isFileDeleted = 0;

    while(!isFileDeleted)
    {
        int i=0;
        // iterate through the directory data blocks in search of 
        // the line containing the filename
        for(i=0;i<totalBlocksOccupied;i++)
        {
            if(i<DIRECT_DATA_BLOCK_NUMBER)
            {
                
                //DataBlock* db = &(fs->db[i]);

                // calculate offset of data block in file
                int dataBlockIndex = parentDirInode->directDBIndex[i];
                
                char* buff = readDataBlockFromFile(fs,dataBlockIndex);
                // read every 16 bit line from the data block and
                // check if there is a match
                int blockOffset = searchFilenameInDataBlock(buff,name,len);
                if(blockOffset != -1)
                {
                    // write the inode no 0 at offset
                    // inode_no filename --> 0 filename
                    // TODO
                    isFileDeleted = 1;
                    break;
                }
            }
        }
    }
    if(isFileDeleted)
        return 1;  // modify if needed
    else
        return 0;
    
}


int system_touch(WholeFS* fs,char* name, int fileType)
{
    /*Make the file */
    
    int inodeIndex = getFirstFreeInodeIndex(fs); // get free inode
    
    fs->sb.inodeList[inodeIndex] = OCCUPIED;
    fs->ib[inodeIndex].fileMode = fileType; // this is a file / directory
    fs->ib[inodeIndex].fileSize = 0;
    fs->ib[inodeIndex].linkCount = 1;
    fs->sb.freeInodeCount--;

    /* Add an entry in parent */
    // TODO : get the parent directory inode no
    int parent = getPwdInodeNumber(fs);
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
/* TODO complete the function */
int getDBlockNumberFromSize(int size)
{
    return 0;
}

void writeFS(WholeFS *fs, int inodeIndex)
{
    FILE *fp = fopen(fs->fileSystemName, "r+");
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

int system_mkdir(WholeFS* fs,char* name, int fileType)
{
    int inode = system_touch(fs, name, fileType);
    return inode;
}
int main(int argc, char const *argv[])
{
    int inode;
    initFS(argv[1]);
    WholeFS* fs = readFS(argv[1]);
    inode = system_touch(fs, "try1.txt", FILE_MODE);
    writeFS(fs, inode);
    inode = system_touch(fs, "try2.txt", FILE_MODE);
    writeFS(fs, inode);
    inode = system_mkdir(fs, "try3", FOLDER_MODE);
    //int inodeIndex;
    //initFS();
    //WholeFS* fs = readFS();
    /*
    printf("*********************************************************\n");
    printf("wholefs = %ld\n",sizeof(WholeFS));
    printf("superBlock = %ld\n",sizeof(SuperBlock));
    printf("Inode: %ld\n",fs->sb.iNodeOffset);
    printf("Datablock: %ld\n",fs->sb.dataBlockOffset);
    printf("*********************************************************\n");
    

    printf("*********************************************************\n");
    printf("%d\n%d\n%d\ninode offset %d\n%d\ndata block offset %d\n%d\n",
    SUPER_BLOCK_START_OFFSET,
    SUPER_BLOCK_SIZE,
    INODE_SIZE,
    INODE_ARRAY_START_OFFSET,
    INODE_ARRAY_SIZE,
    DATA_BLOCK_ARRAY_START_OFFSET,
    DATA_BLOCK_ARRAY_SIZE
    );
    printf("*********************************************************\n");
    */
    return 0;
}
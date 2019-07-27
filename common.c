#include "common.h"
#include "util.h"
#include "file_handling_functions.h"

int getPwdInodeNumber(WholeFS* fs)
{
    return 1;
}
// TODO take filesize as argument


void initFS(char *fileName)
{

    if( access( fileName, F_OK ) != -1 ) {
    // file exists
        return;
    } 
    else {
    // file doesn't exist
    
    FILE *fp = fopen(fileName, "w+");
    int  temp = 0;
    
    fscanf(fp,"%d ",&temp);
    printf("temp %d \n", temp);
    /*
    if(temp==FILE_SYSTEM_ALREADY_INITIALIZED)
        return;   
    fclose(fp);
     */
    fprintf(fp,"%d ",FILE_SYSTEM_NOT_INITIALIZED);
    fseek(fp, sizeof(int)+SIZE-1 , SEEK_SET);
    fputc('\0', fp);
    fclose(fp);
    }
}

Inode* strToInode(char* buffer,int len)
{
    
    Inode* newNode = (Inode*)malloc(sizeof(Inode));
    assert(newNode!=NULL);

    sscanf(buffer,"%d ",&(newNode->fileMode));
    sscanf(buffer,"%d ",&(newNode->linkCount));
    sscanf(buffer,"%d ",&(newNode->fileSize));

    int i;
    for ( i = 0; i < DIRECT_DATA_BLOCK_NUMBER; i++)
    {
        sscanf(buffer,"%d ",&(newNode->directDBIndex[i]));
    }

    return newNode;
}

/*
* ReadFs returns the existing structure into memory.
* If Doesnt exist creates from scratch
* It also changes the starting code to mark the changes
*/

WholeFS* readFS(char const *fileName,int* isInitialized)
{
    // check if its a fresh copy
    FILE *fp = fopen(fileName, "r+");
    int code = FILE_SYSTEM_NOT_INITIALIZED;
    int tmp;

    //read the starting char
    //fseek(fp,0,SEEK_SET);
    fscanf(fp,"%d ",&tmp);
    //printf("%d",tmp);

    WholeFS* fs = NULL;

    // setting name as it must be done before all
    fs = calloc(1,sizeof(WholeFS));
    if(fs==NULL)
        assert(0);
    fs->fileSystemName = Malloc(512, char);
    strcpy(fs->fileSystemName, fileName);
        
    printf("Temp Code %d\n", tmp);
    if(tmp==code) // fresh
    {
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
        *isInitialized = 1;
    }
    else // read from the file to memory structure
    {
        printf("****superblock print");
        readSuperBlock(fs);
        
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
    return -1; // dummy
}


/* TODO complete the function */
int getDBlockNumberFromSize(int size)
{
    return 1;
}

void writeFS(WholeFS *fs, int inodeIndex)
{
    FILE *fp = fopen(fs->fileSystemName, "r+");
    int rootInodeIndex =  getPwdInodeNumber(fs);
    int offset = fs->ib[rootInodeIndex].fileSize % DATA_BLOCK_SIZE;
    int i;
    
    //writeSuperNodeBlockToFile(fs,char* blockBuffer);

    //printf("FileSize after touch : %d\n", fs->ib[rootInodeIndex].fileSize);
    // write data block in the file
    fseek(fp, fs->sb.dataBlockOffset, SEEK_SET);
    /* TODO complete getDBlockNumberFromSize*/
    fseek(fp, getDBlockNumberFromSize(fs->ib[rootInodeIndex].fileSize) * DATA_BLOCK_SIZE, SEEK_CUR);
    fseek(fp, offset , SEEK_CUR);

    //printf("Data Block Offset : %d \n",fs->sb.dataBlockOffset);

    printf("offset value : %d \n", offset);
    printf("data block offset : %d \n", fs->sb.dataBlockOffset);


    DataBlock* db = &(fs->db[rootInodeIndex]);

    char* temp = db->content + offset;
    fprintf(fp, "%s", temp);
    /*
    for( i = 0 ; i < DIRECTORY_ENTRY_LENGTH ; i++)
    {
        printf("%c", db->content[i + offset]);
        fputc(db->content[i + offset], fp);
    }*/
    fs->ib[rootInodeIndex].fileSize += DIRECTORY_ENTRY_LENGTH;

    // write inode in the file

    fseek(fp, fs->sb.iNodeOffset, SEEK_SET);
    fseek(fp, rootInodeIndex * fs->sb.iNodeSize, SEEK_CUR);

    fprintf(fp, "%d ", fs->ib[rootInodeIndex].fileMode);
    //printf("%d ", fs->ib[rootInodeIndex].fileMode);
    fprintf(fp, "%d ", fs->ib[rootInodeIndex].linkCount);
    //printf("%d ", fs->ib[rootInodeIndex].linkCount);
    fprintf(fp, "%d ", fs->ib[rootInodeIndex].fileSize);
    //printf("%d ", fs->ib[rootInodeIndex].fileSize);

    for(i = 0; i < DIRECT_DATA_BLOCK_NUMBER; i++)
        fprintf(fp, "%d ", fs->ib[rootInodeIndex].directDBIndex[i]);

    fclose(fp);
}



/*int main(int argc, char const *argv[])
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
    return 0;
}*/

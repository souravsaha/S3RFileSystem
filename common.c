#include "common.h"
#include "util.h"
#include "file_handling_functions.h"


int getPwdInodeNumber(WholeFS* fs)
{
    //printf("PWD Inode : %d\n",fs->pwdInodeNumber);
    return fs->pwdInodeNumber;
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
    
    //printf("In strToInode\n");
    //printf("buff : %s\n",buffer);
    Inode* newNode = (Inode*)malloc(sizeof(Inode));
    assert(newNode!=NULL);

    /* 
    sscanf(buffer,"%d ",&(newNode->fileMode));
    sscanf(buffer,"%d ",&(newNode->linkCount));
    sscanf(buffer,"%d ",&(newNode->fileSize));

    int i;

    for(i = 0; i < DIRECT_DATA_BLOCK_NUMBER; i++)
    {
        sscanf(buffer,"%d ",&(DBIndex));
        newNode->directDBIndex[i] = DBIndex;
    }
    printf("newNode fileMode : %d,linkCount : %d,DB[0]: %d\n",newNode->fileMode,newNode->linkCount,newNode->directDBIndex[0]);

    */

    // Returns first token 
    char* token = strtok(buffer," "); 
    newNode->fileMode = atoi(token);

    token = strtok(NULL, " "); 
    newNode->linkCount = atoi(token);

    token = strtok(NULL, " "); 
    newNode->fileSize = atoi(token);

    // Keep printing tokens while one of the 
    // delimiters present in str[]
    int i=0;
    while (i<DIRECT_DATA_BLOCK_NUMBER && token != NULL) { 
        //printf("%s\n", token); 
        token = strtok(NULL, " "); 
        if(token!=NULL)
            newNode->directDBIndex[i++] = atoi(token);

    }
    //printf("newNode fileMode : %d,linkCount : %d,filesize: %d DB[0]: %d\n",newNode->fileMode,newNode->linkCount,newNode->fileSize,newNode->directDBIndex[0]);
    return newNode;
}

/*
int main()
{
    char buffer[] = {"1 2 3 4 5 6 "};
    strToInode(buffer,90);
    return 0;
}
 */
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
        fs->pwdInodeNumber = 1;
        strcpy(fs->pwdPath,"/");

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
        char *buff = readInodeBlockFromFile(fs,fs->pwdInodeNumber);
        Inode *inode = strToInode(buff,sizeof(Inode));
        fs->ib[fs->pwdInodeNumber].fileMode = inode->fileMode;
        fs->ib[fs->pwdInodeNumber].linkCount = inode->linkCount;
        fs->ib[fs->pwdInodeNumber].directDBIndex[0] =inode->directDBIndex[0];
        fs->ib[fs->pwdInodeNumber].fileSize = inode->fileSize;
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
    //printf("in getFirstFreeDataBlockIndex function\n");
    int i;
    if(fs->sb.freeDataBlockCount==0)
        assert(0);

    // first two data blocks are reserved
    for(i=2;i<fs->sb.dataBlockCount;i++)
    {
        //printf("Data Block Index %d = %d \n", i, fs->sb.dataBlockList[i]);
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


    if(blockOffset == 0) // need to write at data block beginning
    {
        // check if the block is already allocated or not
        if(isDBlockFree(i,blockIndexInInode))
        {
            idx = getFirstFreeDataBlockIndex(fs);
            //printf("Data Block index : %d\n",idx);
            fs->sb.dataBlockList[idx] = OCCUPIED;
            fs->sb.freeDataBlockCount--;
            i->directDBIndex[blockIndexInInode] = idx;
        }
    }


    *index = idx;
    *offset = blockOffset;

}

// getFirstNewdata block
void getFirstNewDataBlock(WholeFS* fs,Inode* i,int inodeIndex, int* index,int* offset)
{
    int blockIndexInInode = i->fileSize/DATA_BLOCK_SIZE;
    int blockOffset = i->fileSize%DATA_BLOCK_SIZE;
    int idx = i->directDBIndex[blockIndexInInode];
    if(blockIndexInInode>=DIRECT_DATA_BLOCK_NUMBER)
        assert(0); // just to avoid double links


    //if(blockOffset == 0) // need to write at data block beginning
    {
        // check if the block is already allocated or not
        //if(isDBlockFree(i,blockIndexInInode))
        {
            idx = getFirstFreeDataBlockIndex(fs);
            printf("Data Block index : %d\n",idx);
            fs->sb.dataBlockList[idx] = OCCUPIED;
            fs->sb.freeDataBlockCount--;
            i->directDBIndex[blockIndexInInode] = idx;
        }
    }

    *index = idx;
    *offset = blockOffset;

}

// given a block, searches filename 'name' with length 'len' in the block
// returns the index of the block where the block is found
/// else -1
int searchFilenameInDataBlock(char* db,char* name,int len)
{
    return 0;
    
    // buffer to store filename beginning and ending with space
    char save[len+2+1]; 
    save[0] = ' ';
    int i = 1;
    for(i=0;i<len;i++)
    {
        save[i+1] = name[i];
    }
    save[i+1] = '\0';
    //save[i+2] = '\0';

    printf("[searchFilenameInDataBlock]save = %s\n",save);

    char* ptr = strstr(db,save);

    // matcstring
    int j,matchpoint=-1;
    int found = 0;
    for (i = 0; i < DATA_BLOCK_SIZE;i++)
    {
        int t=i;
        int matchcount = 0;
        for(j=0;j<strlen(save);j++)
        {
            if(db[t]!=save[j])
            {
                break;
            }
            else{ // char match
                matchcount++;
                t++;
            }
        }
        if(matchcount==strlen(save))
        {
            matchpoint = i;
            break;
        }

    }
    
    //printf("[searchFilenameInDataBlock]matchIdx = %d\n",matchpoint);
    // extract the number


    if(0)//(index!= -1)
    {
        /*
        --ptr; //go to the last digit of inode

        // read while you get digit
        int sum = 0;
        int mult = 1;
        while(*ptr - '0' >= 0 && *ptr-0 <= 9)
        {
            int d = *ptr - '0';
            sum = sum+ d*mult;
            mult = mult*10;
            printf("[searchFilenameInDataBlock]inode digit %d sum=%d\n",d,sum);

            ptr--;
        }
        return 1;
         */

        long int ptrDiff = ptr - db;
        int index = ptrDiff/DIRECTORY_ENTRY_LENGTH;
        printf("[searchFilenameInDataBlock]index = %d\n",index);
        return index;

    }
    else
        return -1;
    
}


/* TODO complete the function */
int getDBlockNumberFromSize(WholeFS* fs, int inodeIndex, int nDataBlock)
{
    int nEntryInSingleIndirect = DATA_BLOCK_SIZE/sizeof(int);
    int nDirectDataBlock = DIRECT_DATA_BLOCK_NUMBER - 3;
    
    if(nDataBlock < nDirectDataBlock)
        return fs->ib[inodeIndex].directDBIndex[nDataBlock-1];

    else if((nDataBlock >= nDirectDataBlock) && (nDataBlock < nDirectDataBlock + nEntryInSingleIndirect))
        return readSingleRedirectDataBlock(fs->ib[inodeIndex].directDBIndex[nDirectDataBlock],nDataBlock - nDirectDataBlock);
}

void writeFS(WholeFS *fs, int inodeIndex)
{
    FILE *fp = fopen(fs->fileSystemName, "r+");
    int rootInodeIndex =  getPwdInodeNumber(fs);
    int offset = fs->ib[rootInodeIndex].fileSize % DATA_BLOCK_SIZE;
    int i;
    

    int nDataBlocks = getNumberOfDataBlockFromSize(fs->ib[inodeIndex].fileSize);
    //writeSuperNodeBlockToFile(fs,char* blockBuffer);

    //printf("FileSize after touch : %d\n", fs->ib[rootInodeIndex].fileSize);
    // write data block in the file
    fseek(fp, fs->sb.dataBlockOffset, SEEK_SET);
    /* TODO complete getDBlockNumberFromSize*/
    fseek(fp, getDBlockNumberFromSize(fs,rootInodeIndex,nDataBlocks) * DATA_BLOCK_SIZE, SEEK_CUR);
    //fseek(fp, fs->ib[rootInodeIndex].directDBIndex[0] * DATA_BLOCK_SIZE, SEEK_CUR);
    fseek(fp, offset , SEEK_CUR);

    //printf("Data Block Offset : %d \n",fs->sb.dataBlockOffset);

    //printf("offset value : %d \n", offset);
    //printf("data block offset : %d \n", fs->sb.dataBlockOffset);


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

    fseek(fp, fs->sb.iNodeOffset, SEEK_SET);
    fseek(fp, inodeIndex * fs->sb.iNodeSize, SEEK_CUR);

    fprintf(fp, "%d ", fs->ib[inodeIndex].fileMode);
    fprintf(fp, "%d ", fs->ib[inodeIndex].linkCount);
    fprintf(fp, "%d ", fs->ib[inodeIndex].fileSize);


    //printf("Inode WriteFS : %d, DB[0]: %d\n",inodeIndex,fs->ib[inodeIndex].directDBIndex[0]);
    for(i = 0; i<DIRECT_DATA_BLOCK_NUMBER; i++)
        fprintf(fp, "%d ", fs->ib[inodeIndex].directDBIndex[i]);
   

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

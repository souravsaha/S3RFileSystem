#include "common.h"
#include "util.h"
#include "file_handling_functions.h"




void system_ls(WholeFS* fs,int inodeIndex)
{
    /* printf("system_ls\n");
    printf("-------------------\n");
    printf("inodeIndex : %d\n",inodeIndex); */

    char *data = readInodeBlockFromFile(fs,inodeIndex);
    if(strlen(data) == 0) // empty
    {
        printf("Empty Directory\n");
        return;
    }
    Inode* inode = strToInode(data,sizeof(Inode));
    //printf("inode->fileSize: %d\n",inode->fileSize);



    int nDataBlock = getNumberOfDataBlockFromSize(inode->fileSize);
    /* printf("nDataBlocks : %d\n", nDataBlock);
    printf("-------------------\n"); */

    int indexDataBlock, i = 0;
    //printf("i : %d, nDataBlock : %d\n",i,nDataBlock);
    while(i < nDataBlock)
    {
       //printf("in while loop\n");
       indexDataBlock =  getDataBlockIndex(fs,inodeIndex,i++);
       //printf("indexDataBlock : %d\n",indexDataBlock);
       char *data = readDataBlockFromFile(fs,indexDataBlock);
       //printf("data : %s\n",data);
       //printf("data :%s\n",readDataBlockFromFile(fs,indexDataBlock));
       printDirectoryContent(fs,data,inode->fileSize);
    }
}

int system_cd(WholeFS* fs, char* _path)
{
    char *path = Malloc(strlen(_path),char);
    strcpy(path,_path); 
    char *dir;
    int i = 0;
    int inodeIndex;
    if(path[0] == '/')
    {
        //printf("In SYSTEM_cd.... Path : %s\n",path);
        inodeIndex = 1;
        dir = strsep(&path,"/");
        dir = strsep(&path,"/");
    }

    else
    {
        inodeIndex = fs->pwdInodeNumber;
        dir = strsep(&path,"/");
    }
        
    //printf("DIR : %s \n",dir);
    while(dir != NULL && strlen(dir) > 0 )
    {
        inodeIndex = getInodeIndexFromName(fs, dir, inodeIndex);
        //printf("DIR : %s, Inode : %d\n",dir,inodeIndex);
        if(inodeIndex == 0)
        {
            perror("path not found\n");
            return -1;
        }
        
        dir = strsep(&path,"/");

    }

    //printf("Path : %s\n",_path);
    fs->pwdInodeNumber = inodeIndex;
    if(_path[0] == '/')
    {

        strcpy(fs->pwdPath,_path);
    }    
    else
    {
        if(strcmp(fs->pwdPath,"/"))
            strcat(fs->pwdPath,"/");
        strcat(fs->pwdPath,_path);
    } 
    printf("Path : %s \n",fs->pwdPath);
    
    return 0;
}


void system_pwd(WholeFS *fs)
{
    printf("Path: %s \n",fs->pwdPath);
}

int system_rm(WholeFS* fs,char* name,int len)
{
    //printf("###################################################################");
    // get current inode number of current working directory
    int dirInode = getPwdInodeNumber(fs);

    
    // get Inode of the pwd directory
    Inode* parentDirInode = getInode(fs,dirInode);

    // calculate no of data blocks for parent dir
    int totalBlocksOccupied = parentDirInode->fileSize/DATA_BLOCK_SIZE;
    if(parentDirInode->fileSize%DATA_BLOCK_SIZE != 0)
        totalBlocksOccupied++;


    // only 1st level link
    assert(totalBlocksOccupied<=DIRECT_DATA_BLOCK_NUMBER);

    // get the data block of the directory files
    int isFileDeleted = 0;
    int isFound = 0;
    //while(isFileDeleted == 0 )
    //{
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

                // printf("before Buffer:\n",buff);
                // for (int k = 0; k < 128; k++)
                // {
                //     printf("%c",buff[k]);
                // }
                // printf("\n");
                        

                int offset = 0;
                int i, index;
                char dirName[12];
                sscanf(buff + offset,"%d %s",&index,dirName);

                int findInode = -1;
                int findOffset = -1;

                //pirintf("Directory name : %s\n",dirName);
                while(offset <DATA_BLOCK_SIZE)
                {
                    if(strcmp(dirName, name) == 0)
                    {
                        //return index;
                        findOffset = offset;
                        findInode = index;
                        break;
                    }
                    offset+= 16;
                    sscanf(buff + offset,"%d %s",&index,dirName);
                }
                // file has been found
                if(findInode != -1)
                {
                    //printf("offset %d\n",findOffset);
                    isFound = 1;
                    Inode* inodePtr = getInode(fs,index);
                    if(inodePtr->fileMode == FOLDER_MODE)
                    {
                        printf("\n%s is a directory",name);
                        return 0;
                    }
                    else
                    {
                        // clear inode, data blocks TODO
                        // mark all data blocks free
                        int dataBlocksAllocated = inodePtr->fileSize/DATA_BLOCK_SIZE;
                        if(inodePtr->fileSize%DATA_BLOCK_SIZE)
                            dataBlocksAllocated++;
                        
                        // mark data blocks free
                        int i;
                        for(i=0;i<dataBlocksAllocated;i++)
                            fs->sb.dataBlockList[inodePtr->directDBIndex[i]] = FREE;
                        // mark corresponding inodes free
                        fs->sb.inodeList[findInode] = FREE;

                        // other superblock adjustments
                        fs->sb.freeDataBlockCount+=dataBlocksAllocated;
                        fs->sb.freeInodeCount += 1;

                        // write superblock
                        writeSuperBlock(fs);

                        // make a buffer with space after name
                        char* namespace = (char*)malloc((strlen(name)+2)*sizeof(char));
                        strcpy(namespace,name);
                        namespace[strlen(name)]=' ';
                        namespace[strlen(name)]='\0';
                        
                        sprintf(buff+offset,"%d %s",0,namespace);
                        isFileDeleted = 1;
                        // printf("after Buffer:\n",buff);
                        // for (int k = 0; k < 128; k++)
                        // {
                        //     printf("%c",buff[k]);
                        // }
                        // printf("\n");

                        int k=writeEntireDataBlockToFile(fs,buff,dataBlockIndex);
                        //printf("%d wrriten to file for rm\n",k);
                        return 1;
                    }
                    
                   
                }
                

                
            }
            else
            {
                assert(0);
            }
            
        }

    printf("\nFile Not found") ;
    return 0;

} 

char catBuffer[1000000];

int system_touch(WholeFS* fs,char* name, int fileType)
{
    /*Make the file */
    int inodeIndex = getFirstFreeInodeIndex(fs); // get free inode
    int dataBlockidx = getFirstFreeDataBlockIndex(fs); //get first free data block
    fs->sb.dataBlockList[dataBlockidx] = OCCUPIED;
    fs->sb.freeDataBlockCount--;
    //printf("Free Inode no :%d\n",inodeIndex);

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
    
    // TODO inodeIndex is wrongly passed but it is not used so not affecting 
    // should pass parentInodeIndex
    calculateDataBlockNoAndOffsetToWrite(fs,parentInode,inodeIndex, &blockNo,&blockOffset);

    assert(blockNo<DIRECT_DATA_BLOCK_NUMBER);

    fs->ib[inodeIndex].directDBIndex[0] = dataBlockidx;

    // add a 16byte entry in the parent directory
    //printf("\nblockNo = %d offset= %d\n",blockNo,blockOffset);

    char buffer[16];
    sprintf(buffer,"%d %s ",inodeIndex,name);
    //printf("Buffer content %s \n", buffer);
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
        //printf("%s\n",d->content);
        // print the content of the block

    }else // new bloxk needed
    {
        assert(0);
    }
    //printf("Direct DB Index, inode index : %d, value = %d\n", inodeIndex, fs->ib[inodeIndex].directDBIndex[0]);
    writeFS(fs, inodeIndex);
    return inodeIndex;


}

int system_cat(WholeFS* fs,char* name,int mode)
{
    if(mode==0) // read mode
    {
        int inodeIndex = getInodeIndexFromName(fs, name, getPwdInodeNumber(fs));
        Inode* in = getInode(fs,inodeIndex);

        
        int noDataBlocks = in->fileSize/DATA_BLOCK_SIZE;
        if(in->fileSize%DATA_BLOCK_SIZE)
            noDataBlocks++;

        int i;
        for ( i = 0; i < noDataBlocks; i++)
        {
            int dataBlockIndex = in->directDBIndex[i];
            char* buff =readDataBlockFromFile(fs,dataBlockIndex);
            /*print contents */
            int j;
            printf("\n");
            for ( j = 0; j < DATA_BLOCK_SIZE; j++)
            {
                printf("%c",buff[j]);
            }
            printf("\n");
        }
        
    }else // write mode
    {
        // TODO check if file already exists
        //
        //int inodeNo = system_touch(fs,name,FILE_MODE);
        printf("\nPlease Write the contents press ctrl+D twice when you are done>>\n");

        char ch=getchar();
        int idx = 0;
        while(ch!=EOF){
            catBuffer[idx++] = ch;
            ch=getchar();
        }
        catBuffer[idx] = 0; // end with null
        //printf("%s",catBuffer);
        
        int inodeNo = system_touch(fs,name,FILE_MODE);
        Inode* in = getInode(fs,inodeNo);
        
        char* ptr = catBuffer;
        int blockNo=-1,blockOffset = -1;
        
        int toWrite = idx;
        while(toWrite>0)
        { 
            calculateDataBlockNoAndOffsetToWrite(fs,in,0, &blockNo,&blockOffset);
            int emptyInThisBlock = DATA_BLOCK_SIZE - blockOffset;
            int contentSizeToWrite = toWrite<emptyInThisBlock ? toWrite:emptyInThisBlock;
            //printf("towrite: %d emptyInThisBlock: %d blockNo: %d offset:%d\n",toWrite,emptyInThisBlock,blockNo,blockOffset);

            int n = appendNbytesInDataBlockToFile(fs,ptr,blockOffset,  blockNo,contentSizeToWrite);
            if(n!=contentSizeToWrite)
                assert(0);
            
            // increment ptr and filesize decrease toWrite
            ptr+=contentSizeToWrite;
            in->fileSize+=contentSizeToWrite;
            toWrite -= contentSizeToWrite;
            
        }
        // write inode
        writeInodeToFile(fs,in,inodeNo);
    }
}

int system_mkdir(WholeFS* fs,char* name, int fileType)
{
    char *appendedFolderName = Malloc(strlen(name) + 2, char);
    //appendedFolderName[0] = DIRECTORY_INDICATOR;
    strcpy(appendedFolderName,name);    
    //printf("Modified Folder Name %s", appendedFolderName);
    int inode = system_touch(fs, appendedFolderName, fileType);
    return inode;
}

/* 
void system_cp(WholeFS *fs, char* copyFrom, char* copyTo)
{

    FILE *fp = fopen("S3R.fs", "r+");

    int currentInodeIndex, newInodeIndex;
    currentInodeIndex = getParentInode(copyFrom);
    newInodeIndex = getInodeFromPath(copyTo);
    int i, inodeNum;
    char name[500], dirName[500] = "root";
    char *inodeStr = (char*)malloc(100*sizeof(char));

    strcpy(name, getName(copyFrom));

    int* DBindex = (int *)malloc(sizeof(int));
    int* offset = (int *)malloc(sizeof(int));
    Inode* i = (Inode*)malloc(sizeof(Inode));

    strcpy(inodeStr,readInodeBlockFromFile(fs, currentInodeIndex));
    i = strToInode(inodeStr,strlen(inodeStr));
    calculateDataBlockNoAndOffsetToWrite(fs, i, currentInodeIndex, DBindex, offset);

    fseek(fp, sizeof(int), SEEK_SET);
    fseek(fp, fs->sb.dataBlockOffset, SEEK_CUR);
    fseek(fp, DBindex, SEEK_CUR);
    fseek(fp, offset , SEEK_CUR);
    fscanf(fp, "%d %s", &inodeNum,dirName);


    int dbOffset = fs->ib[newInodeIndex].fileSize % DATA_BLOCK_SIZE;
    fseek(fp, sizeof(int), SEEK_SET);
    fseek(fp, fs->sb.dataBlockOffset, SEEK_CUR);
    fseek(fp, getDBlockNumberFromSize(fs->ib[newInodeIndex].fileSize) * DATA_BLOCK_SIZE, SEEK_CUR);
    fseek(fp, dbOffset , SEEK_CUR);
    fprintf(fp, "%d %s", inodeNum, dirName);
}

void system_mv(WholeFS *fs, char* moveFrom, char* moveTo)
{
    
    FILE *fp = fopen("S3R.fs", "r+");

    int currentInodeIndex, newInodeIndex;
    currentInodeIndex = getParentInode(moveFrom);
    newInodeIndex = getInodeFromPath(moveTo);
    int i, inodeNum;
    char name[500], dirName[500];
    char *inodeStr = (char*)malloc(100*sizeof(char));
    
    strcpy(name, getName(moveFrom));

    int* DBindex = (int *)malloc(sizeof(int));
    int* offset = (int *)malloc(sizeof(int));
    Inode* i = (Inode*)malloc(sizeof(Inode));

    strcpy(inodeStr,readInodeBlockFromFile(fs, currentInodeIndex));
    i = strToInode(inodeStr,strlen(inodeStr));
    calculateDataBlockNoAndOffsetToWrite(fs, i, currentInodeIndex, DBindex, offset);

    //fseek(fp, sizeof(int), SEEK_SET);  
    fseek(fp, fs->sb.dataBlockOffset, SEEK_SET);
    fseek(fp, DBindex, SEEK_CUR);
    fseek(fp, offset , SEEK_CUR);
    fscanf(fp, "%d %s", &inodeNum,dirName);

    fseek(fp, sizeof(int) + fs->sb.dataBlockOffset + DBindex + offset, SEEK_SET);
    fprintf(fp, "%d", 0);

    int dbOffset = fs->ib[newInodeIndex].fileSize % DATA_BLOCK_SIZE;
    fseek(fp, sizeof(int), SEEK_SET);
    fseek(fp, fs->sb.dataBlockOffset, SEEK_CUR);
    fseek(fp, getDBlockNumberFromSize(fs->ib[newInodeIndex].fileSize) * DATA_BLOCK_SIZE, SEEK_CUR);
    fseek(fp, dbOffset , SEEK_CUR);
    fprintf(fp, "%d %s", inodeNum, dirName);

}*/
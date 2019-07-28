#include "common.h"
#include "util.h"
#include "file_handling_functions.h"




void system_ls(WholeFS* fs,int inodeIndex)
{
    printf("system_ls\n");
    printf("-------------------\n");
    printf("inodeIndex : %d\n",inodeIndex);

    char *data = readInodeBlockFromFile(fs,inodeIndex);
    if(strlen(data) == 0) // empty
    {
        printf("Empty Directory\n");
        return;
    }
    Inode* inode = strToInode(data,sizeof(Inode));
    //printf("inode->fileSize: %d\n",inode->fileSize);



    int nDataBlock = getNumberOfDataBlockFromSize(inode->fileSize);
    printf("nDataBlocks : %d\n", nDataBlock);
    printf("-------------------\n");

    int indexDataBlock, i = 0;
    printf("i : %d, nDataBlock : %d\n",i,nDataBlock);
    while(i < nDataBlock)
    {
       printf("in while loop\n");
       indexDataBlock =  getDataBlockIndex(fs,inodeIndex,i++);
       printf("indexDataBlock : %d\n",indexDataBlock);
       char *data = readDataBlockFromFile(fs,indexDataBlock);
       printf("data : %s\n",data);
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
        printf("In SYSTEM_cd.... Path : %s\n",path);
        inodeIndex = 1;
        dir = strsep(&path,"/");
        dir = strsep(&path,"/");
    }

    else
    {
        inodeIndex = fs->pwdInodeNumber;
        dir = strsep(&path,"/");
    }
        
    printf("DIR : %s \n",dir);
    while(dir != NULL && strlen(dir) > 0 )
    {
        inodeIndex = getInodeIndexFromName(fs, dir, inodeIndex);
        printf("DIR : %s, Inode : %d\n",dir,inodeIndex);
        if(inodeIndex == 0)
        {
            perror("path not found\n");
            return -1;
        }
        
        dir = strsep(&path,"/");

    }

    printf("Path : %s\n",_path);
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
    printf("PWD Inode : %d, Path : %s \n",inodeIndex,fs->pwdPath);
    
    return 0;
}


void system_pwd(WholeFS *fs)
{
    printf("pwd Path: %s \n",fs->pwdPath);
}

/* int system_rm(WholeFS* fs,char* name,int len)
{
    printf("###################################################################");
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
                printf("b4 Buffer: %s\n",buff);
                // read every 16 bit line from the data block and
                // check if there is a match
                int entryNo = searchFilenameInDataBlock(buff,name,strlen(name));
                if(entryNo != -1)
                {
                    // write the inode no 0 at offset
                    // inode_no filename --> 0 filename
                    // TODO

                    char* entryBuffer = makeDirString(name,strlen(name),0);


                    if(entryBuffer==NULL)
                        assert(0);

                    printf("[system_rm]Composed: %s",entryBuffer);
                    
                    strncpy(buff+entryNo*DIRECTORY_ENTRY_LENGTH,entryBuffer,DIRECTORY_ENTRY_LENGTH);

                    //mark inode no and corresponding data blocks free 

                    isFileDeleted = 1;
                
                    printf("after Buffer: %s\n",buff);
                    break;
                }
            }
        }
    }

    printf("###################################################################");
    if(isFileDeleted)
        return 1;  // modify if needed
    else
        return 0;

} */


int system_touch(WholeFS* fs,char* name, int fileType)
{
    /*Make the file */
    int inodeIndex = getFirstFreeInodeIndex(fs); // get free inode
    int dataBlockidx = getFirstFreeDataBlockIndex(fs); //get first free data block
    fs->sb.dataBlockList[dataBlockidx] = OCCUPIED;
    fs->sb.freeDataBlockCount--;
    printf("Free Inode no :%d\n",inodeIndex);

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

    fs->ib[inodeIndex].directDBIndex[0] = dataBlockidx;

    // add a 16byte entry in the parent directory
    printf("\nblockNo = %d offset= %d\n",blockNo,blockOffset);

    char buffer[16];
    sprintf(buffer,"%d %s ",inodeIndex,name);
    printf("Buffer content %s \n", buffer);
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
    writeFS(fs, inodeIndex);
    return inodeIndex;


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
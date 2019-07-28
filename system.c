#include "common.h"
#include "util.h"
#include "file_handling_functions.h"

void system_ls(WholeFS* fs,int inodeIndex)
{
    printf("system_ls\n");
    printf("-------------------\n");
    printf("inodeIndex : %d\n",inodeIndex);

    char *data = readInodeBlockFromFile(fs,inodeIndex);
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
       printDirectoryContent(fs,data,DATA_BLOCK_SIZE);
    }
}

int system_cd(WholeFS* fs, char* path)
{
    char *dir;
    int i = 0;
    int inodeIndex = 1;
    if(path[0] == '/')
    {
        dir = strtok(path,"/:");
        while(dir!=NULL)
        {
            inodeIndex = getInodeIndexFromName(fs,dir,inodeIndex);
            if(inodeIndex == 0)
            {
                perror("path not found\n");
                return -1;
            }
            dir = strtok(NULL,"/:");
        }

        fs->pwdInodeNumber = inodeIndex;
        printf("PWD Inode : %d\n",inodeIndex);
        //return 0;
    }
    return 0;
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

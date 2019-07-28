#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "common.h"
#include<fcntl.h>
#include "file_handling_functions.h"
#include "common.h"
#include "util.h"
#include "file_handling_functions.h"

DataBlock* getDataBlockFromIndex(int index)
{
    char *directoryEntry = "abc 12          xyz 100                   def 17";
    DataBlock* db = (DataBlock *)malloc(sizeof(DataBlock));
    strcpy(db->content, directoryEntry);
    //db->content = directoryEntry;
    return db;
}

int getNumberOfDataBlockFromSize(int size)
{
    return ((int)size/DATA_BLOCK_SIZE + 1);
}


/* char* readDataBlock(WholeFS*fs, FILE *fp, int index)
{
    int offset = sizeof(int) + fs->sb.dataBlockOffset + (index * DATA_BLOCK_SIZE);
    fseek(fp, offset, SEEK_SET);

    char data[128];
    fgets(data,DATA_BLOCK_SIZE,fp);
    return data;
} */

int readSingleRedirectDataBlock(int index,int n)
{
    return 0;
}

void printDirectoryContent(WholeFS* fs,char *data,int size)
{
    int offset = 0;
    
    char name[12], *buffer;
    int inodeIndex;
    Inode* inode;

    while(offset < size)
    {
        //int currentPosition = offset;
        

        // Working code
        /* Start
        char c = data[currentPosition++];
        while(c !='\0')
        {
            printf("%c",c);
            c = data[currentPosition++];
        }
        End */
         
        sscanf(data+offset,"%d %s",&inodeIndex,name);
        //printf("index: %d, name : %s \n",inodeIndex,name);
        buffer = readInodeBlockFromFile(fs,inodeIndex);
        //printf("buffer : %s\n",buffer);
        inode = strToInode(buffer,sizeof(Inode));

        printf("fileMode : %d ,fileSize: %d, linkCount: %d, name: %s \n",inode->fileMode,inode->fileSize,inode->linkCount,name);

        offset += DIRECTORY_ENTRY_LENGTH;
    }
}


int getDataBlockIndex(WholeFS* fs, int inodeIndex, int nDataBlock)
{
    printf("In getDataBlockIndex\n");
    int nEntryInSingleIndirect = DATA_BLOCK_SIZE/sizeof(int);
    int nDirectDataBlock = DIRECT_DATA_BLOCK_NUMBER - 3;
    
    char *buffer = readInodeBlockFromFile(fs,inodeIndex);
    Inode *inode = strToInode(buffer,sizeof(Inode));
    printf("Inode index : %d, DB[0] : %d\n",inodeIndex,inode->directDBIndex[0]);


    if(nDataBlock < nDirectDataBlock)
        return inode->directDBIndex[nDataBlock];
    
    else if((nDataBlock >= nDirectDataBlock) && (nDataBlock < nDirectDataBlock + nEntryInSingleIndirect))
        return readSingleRedirectDataBlock(fs->ib[inodeIndex].directDBIndex[nDirectDataBlock],nDataBlock - nDirectDataBlock);
}

int getInodeIndexFromName(WholeFS *fs,char *name,int pdIndex)
{

    Inode* inode = strToInode(readInodeBlockFromFile(fs,pdIndex),sizeof(Inode));

    //TODO : need to do whole content, only first direct block access,
    char *dataBlockContent = readDataBlockFromFile(fs,inode->directDBIndex[0]);

    int offset = 0, currentOffset;
    int i, index;
    char dirName[12];
    sscanf(dataBlockContent + offset,"%d %s",&index,dirName);
    //printf("Directory name : %s\n",dirName);
    while(offset <128)
    {
        if(strcmp(dirName, name) == 0)
        {
            return index;
        }
        offset+= 16;
        sscanf(dataBlockContent + offset,"%d %s",&index,dirName);
    }

    return 0;
}

/* void testReadDataBlock()
{
    SUPERBLOCK sb = {}
    WholeFS *fs =
    FILE* fp = fopen("S3R.fs","r");

} */


/* void testPrintDirectory()
{
    char content[128];
    strcpy(content+4,"home");
    strcpy(content+20,"dev");
    int i;
    for (i = 8; i < 15; i++)
        content[i] ='\0';
    for (i = 24; i < 128; i++)
        content[i] ='\0';
    int size = 32;
    printDirectoryContent(content,size);
} */

/* char* makeDirString(char* filename, int len, int inodeNo)
{
    int i, j, inodeNumDigit=0;
    char* buffer = (char*)malloc((INODE_NO_STRING_SIZE + FILE_NAME_STRING_SIZE)*sizeof(char));
    //int len = strlen(filename);

    int copyInode = inodeNo;
    while (copyInode!=0)
    {
        inodeNumDigit ++;
        copyInode /= 10;
    }
    assert(inodeNo>=0);
    assert(inodeNumDigit<=INODE_NO_STRING_SIZE);
    assert(len<=FILE_NAME_STRING_SIZE);
    //printf("inode digits: %d\n",inodeNumDigit);

    
    if (inodeNo==0)
    {
        //printf("This inode is invalid.\n");
        
        return;
    }

    if (inodeNo==1)
    {
        printf("This is the root inode.\n");
        return;
    }
     
    if(inodeNo==1 || inodeNo==0)
    {
        inodeNumDigit = 1;
        buffer[0] = inodeNo+'0';
    }
    else{
        for (i=inodeNumDigit-1; i>=0; i--)
        {
            buffer[i] = (copyInode%10)+'0';
            //printf("%c\n",buffer[i]);
            copyInode /= 10;
        }
    }
    // add one space
    buffer[inodeNumDigit] = ' ';

    // write string
    int k=0;
    for (i=inodeNumDigit+1; i<INODE_NO_STRING_SIZE + len; i++)
    {
        buffer[i] = filename[k];
        //printf("%c\n",buffer[i]);
        k++;
    }
    // fill the rest with space
    for (; i < INODE_NO_STRING_SIZE + FILE_NAME_STRING_SIZE; i++)
    {
        buffer[i] =  ' ';
        //printf("%c\n",buffer[i]);
    }
    printf("%s", buffer);
    return buffer;
} 

*/

/*
int main(int argc, char const *argv[])
{
    char path[] = "/abc/def/ghij/xyz";
    char *directoryEntry = "abc 12          xyz 14                    def 17";
    //printf("String: %s, Length : %ld\n",directoryEntry,strlen(directoryEntry));
    char *dir;
    int i = 0;
    int inodeIndex = 1;
    if(path[0] == '/')
    {
        dir = strtok(path,"/:");
        while(dir!=NULL)
        {
            inodeIndex = getInodeIndexFromName(dir,inodeIndex);
            printf("string : %s, index : %d\n",dir,inodeIndex);
            dir = strtok(NULL,"/:");
        }

        return 0;
    }

    //perror("not a valid path.");

    testPrintDirectory();
    return 0;
} */

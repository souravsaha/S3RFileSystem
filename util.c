#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "common.h"
#include<fcntl.h>

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
    return 1;
}


char* readDataBlock(WholeFS*fs, FILE *fp, int index)
{   
    int offset = sizeof(int) + fs->sb.dataBlockOffset + (index * DATA_BLOCK_SIZE);
    fseek(fp, offset, SEEK_SET);
    
    char data[128];
    fgets(data,DATA_BLOCK_SIZE,fp);
    return data;
}

int readSingleRedirectDataBlock(int index,int n)
{
    return 0;
}

void printDirectoryContent(char *data,int size)
{
    int offset = 4;
    while(offset < size)
    {
        int currentPosition = offset;
        char c = data[currentPosition++];
        while(c !='\0')
        {
            printf("%c",c);
            c = data[currentPosition++];   
        }
        printf(" ");
        offset += DIRECTORY_ENTRY_LENGTH;
    }
}


int getDataBlockIndex(WholeFS* fs, int inodeIndex, int nDataBlock)
{
    int nEntryInSingleIndirect = DATA_BLOCK_SIZE/sizeof(int);
    int nDirectDataBlock = DIRECT_DATA_BLOCK_NUMBER - 3;
    if(nDataBlock < nDirectDataBlock)
        return fs->ib[inodeIndex].directDBIndex[nDataBlock];
    else if((nDataBlock >= nDirectDataBlock) && (nDataBlock < nDirectDataBlock + nEntryInSingleIndirect))
        return readSingleRedirectDataBlock(fs->ib[inodeIndex].directDBIndex[nDirectDataBlock],nDataBlock - nDirectDataBlock);  
}

void system_ls(WholeFS* fs,int inodeIndex)
{
    Inode inode = fs->ib[inodeIndex];
    int nDataBlock = getNumberOfDataBlockFromSize(inode.fileSize);
    int indexDataBlock, i = 0;
    int size;
    while(i < nDataBlock)
    {
       indexDataBlock =  getDataBlockIndex(fs,inodeIndex,i++);
       char *data = readDataBlock(fs,indexDataBlock);
       printDirectoryContent(data,size);
    }
} 



int getInodeIndexFromName(char *name,int pdIndex)
{
    DataBlock *currentDBBlock  = getDataBlockFromIndex(pdIndex);
    int offset = 0, currentOffset;
    int i, index;
    char dirName[12];
    sscanf(currentDBBlock->content + offset,"%s %d",dirName,&index);
    //printf("Directory name : %s\n",dirName);
    while(offset <128)
    {
        if(strcmp(dirName, name) == 0)
        {
            return index;
        }    
        offset+= 16;
        sscanf(currentDBBlock->content + offset,"%s %d",dirName,&index);
    }

    /* if(offset < 128)
        sscanf(currentDBBlock->content + offset + 12,"%d",&index); */
    
    return 0;

  /*   for(i=0;i<strlen(name);i++)
    {
        currentOffset = offset;
        if(currentDBBlock->content[currentOffset++] != name[i])
        {
            offset += 16;
            if(offset >= 128 )
            {
                perror("dir not found");
                return 0;
            }
        }
    }

    
    }*/
}



void testPrintDirectory()
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
}

int main(int argc, char const *argv[])
{
    /* char path[] = "/abc/def/ghij/xyz";
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

    perror("not a valid path."); */
    
    testPrintDirectory();


    return 0;
}

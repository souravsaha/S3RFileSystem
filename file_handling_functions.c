#include "file_handling_functions.h"
#include "common.h"

char* readNbytesFromOffset(int n,int offset)
{
    FILE* fp = fopen(FILE_NAME,"r");
    fseek(fp, offset, SEEK_SET);

    // n+1 is given as fgets always returns null terminated string so one extra
    char* data = (char*) malloc((n+1)*sizeof(char));
     if(data==NULL)
        assert(0);
    fgets(data,n+1,fp);
    fclose(fp);
    return data;
    
}

int writeNBytesToOffset(char* buffer,int n,int offset)
{
    FILE* fp = fopen(FILE_NAME,"r+");
    fseek(fp, offset, SEEK_SET);
    //size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
    int byteWritten = fwrite( (const void *) buffer, sizeof(char), n,fp );
    if(n!=byteWritten)
        assert(0);
    
    fclose(fp);

}

void writeDataBlockToFile(WholeFS*fs,char* blockBuffer,  int index)
{   
    
    int offset = sizeof(int) + fs->sb.dataBlockOffset + (index * DATA_BLOCK_SIZE);

    writeNBytesToOffset(blockBuffer,DATA_BLOCK_SIZE,offset);
    
}

void writeInodeBlockToFile(WholeFS*fs,char* blockBuffer,  int index)
{   
    
    int offset = sizeof(int) + fs->sb.iNodeOffset + (index * sizeof(Inode));

    writeNBytesToOffset(blockBuffer,sizeof(Inode),offset);
    
}

void writeSuperNodeBlockToFile(WholeFS*fs,char* blockBuffer)
{ 
    int offset = sizeof(int);
    writeNBytesToOffset(blockBuffer,sizeof(SuperBlock),offset);    
}


char* readDataBlockFromFile(WholeFS*fs,  int index)
{   
    
    int offset = sizeof(int) + fs->sb.dataBlockOffset + (index * DATA_BLOCK_SIZE);
    char* data = readNbytesFromOffset(DATA_BLOCK_SIZE,offset);
    return data;
}

char* readInodeBlockFromFile(WholeFS*fs,  int index)
{   
    
    int offset = sizeof(int) + fs->sb.iNodeOffset + (index * sizeof(Inode));
    char* data = readNbytesFromOffset(sizeof(Inode),offset);
    return data;
}

char* readSuperBlockFromFile(WholeFS*fs,  int index)
{   
    
    int offset = sizeof(int);
    char* data = readNbytesFromOffset(sizeof(SuperBlock),offset);
    return data;
}


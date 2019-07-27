#include "common.h"
#include "util.h"
#include "file_handling_functions.h"


char* readNbytesFromOffset(int n,int offset, char *fileSystemName)
{
    FILE* fp = fopen(fileSystemName,"r");
    fseek(fp, offset, SEEK_SET);

    // n+1 is given as fgets always returns null terminated string so one extra
    char* data = (char*) malloc((n+1)*sizeof(char));
     if(data==NULL)
        assert(0);
    fgets(data,n+1,fp);
    printf("Data in syscall :%s\n",data);
    fclose(fp);
    return data;

}

int writeNBytesToOffset(char* buffer,int n,int offset, char *fileSystemName)
{
    FILE* fp = fopen(fileSystemName,"r+");
    fseek(fp, offset, SEEK_SET);
    //size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
    int byteWritten = fwrite( (const void *) buffer, sizeof(char), n,fp );
    //if(n!=byteWritten)
    //    assert(0);

    fclose(fp);
    return byteWritten;
}

int writeDataBlockToFile(WholeFS*fs,char* blockBuffer,int existingFileSize,  int index)
{

    int offset = fs->sb.dataBlockOffset + (index * DATA_BLOCK_SIZE) + (existingFileSize % DATA_BLOCK_SIZE);

    int byteWritten = writeNBytesToOffset(blockBuffer,DATA_BLOCK_SIZE,offset, fs->fileSystemName);
    return byteWritten;
}

void writeInodeBlockToFile(WholeFS*fs,char* blockBuffer,  int index)
{

    int offset = fs->sb.iNodeOffset + (index * sizeof(Inode));

    writeNBytesToOffset(blockBuffer,sizeof(Inode),offset, fs->fileSystemName);

}

void writeSuperNodeBlockToFile(WholeFS*fs,char* blockBuffer)
{
    int offset = sizeof(int);
    writeNBytesToOffset(blockBuffer,sizeof(SuperBlock),offset, fs->fileSystemName);
}


char* readDataBlockFromFile(WholeFS*fs,  int index)
{

    int offset = fs->sb.dataBlockOffset + (index * DATA_BLOCK_SIZE);
    printf("offset %d \n", offset);
    printf("inode list block offset %d \n", fs->sb.iNodeOffset);
    printf("data block offset %d \n", fs->sb.dataBlockOffset);
    printf("sizeof SUPER block offset %d \n", sizeof(SuperBlock));

    char* data = readNbytesFromOffset(DATA_BLOCK_SIZE,offset, fs->fileSystemName);
    printf("Data %s \n", data);
    return data;
}

char* readInodeBlockFromFile(WholeFS*fs,  int index)
{

    int offset = fs->sb.iNodeOffset + (index * sizeof(Inode));
    char* data = readNbytesFromOffset(sizeof(Inode),offset, fs->fileSystemName);
    return data;
}

char* readSuperBlockFromFile(WholeFS*fs)
{

    int offset = sizeof(int);
    char* data = readNbytesFromOffset(sizeof(SuperBlock),offset, fs->fileSystemName);
    return data;
}

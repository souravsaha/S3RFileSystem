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
    //fgets(data,n+1,fp);
    fread(data,sizeof(char), DATA_BLOCK_SIZE, fp);
    //printf("Data in syscall :%s\n",data);
    fclose(fp);
    return data;

}

void writeSuperBlock(WholeFS *fs)
{
    FILE *fp = fopen(fs->fileSystemName, "r+");
    int i;
    // write super block
    fseek(fp, sizeof(int)+1, SEEK_SET);
    fprintf(fp, "%d ", fs->sb.inodeCount);
    fprintf(fp, "%d ", fs->sb.freeInodeCount);
    fprintf(fp, "%d ", fs->sb.iNodeOffset);
    fprintf(fp, "%d ", fs->sb.dataBlockOffset);
    fprintf(fp, "%d ", fs->sb.dataBlockCount);
    fprintf(fp, "%d ", fs->sb.freeDataBlockCount);
    fprintf(fp, "%d ", fs->sb.iNodeSize);
    fprintf(fp, "%d ", fs->sb.dataBlockSize);

    for(i = 0; i < NO_OF_INODES; i++)
        fprintf(fp, "%d ", fs->sb.inodeList[i]);
    
    for(i = 0; i < NO_OF_DATA_BLOCKS; i++)
        fprintf(fp, "%d ", fs->sb.dataBlockList[i]);
    
    

    fclose(fp);
}

// reads super block into memory
void readSuperBlock(WholeFS* fs)
{
    FILE *fp = fopen(fs->fileSystemName, "r");
    int i;
    // write super block
    fseek(fp, sizeof(int), SEEK_SET);
    fscanf(fp, "%d ", &(fs->sb.inodeCount));
    fscanf(fp, "%d ", &(fs->sb.freeInodeCount));
    fscanf(fp, "%d ", &(fs->sb.iNodeOffset));
    fscanf(fp, "%d ", &(fs->sb.dataBlockOffset));
    fscanf(fp, "%d ", &(fs->sb.dataBlockCount));
    fscanf(fp, "%d ", &(fs->sb.freeDataBlockCount));
    fscanf(fp, "%d ", &(fs->sb.iNodeSize));
    fscanf(fp, "%d ", &(fs->sb.dataBlockSize));

    for(i = 0; i < NO_OF_INODES; i++)
        fscanf(fp, "%d ", &(fs->sb.inodeList[i]));
    
    for(i = 0; i < NO_OF_DATA_BLOCKS; i++)
        fscanf(fp, "%d ", &(fs->sb.dataBlockList[i]));
    
    

    /*print */
    printf("inodecount: %d \n", (fs->sb.inodeCount));
    printf("freeInode: %d \n", (fs->sb.freeInodeCount));
    printf("inodeOfsset: %d \n", (fs->sb.iNodeOffset));
    printf("datablockoffset %d \n", (fs->sb.dataBlockOffset));
    printf("datablockCount %d \n", (fs->sb.dataBlockCount));
    printf("freeDataBlockCount %d\n", (fs->sb.freeDataBlockCount));
    printf("inodesize: %d ", (fs->sb.iNodeSize));
    printf("datablocksize: %d ", (fs->sb.dataBlockSize));
    
    fs->pwdInodeNumber = 1;
    strcpy(fs->pwdPath,"/");
    fclose(fp);
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

// write in a data block at some offset
int writeDataBlockToFile(WholeFS*fs,char* blockBuffer,int existingFileSize,  int index)
{

    int offset = fs->sb.dataBlockOffset + (index * DATA_BLOCK_SIZE) + (existingFileSize % DATA_BLOCK_SIZE);

    int byteWritten = writeNBytesToOffset(blockBuffer,DATA_BLOCK_SIZE,offset, fs->fileSystemName);
    return byteWritten;
}

// write entire data block to file
int writeEntireDataBlockToFile(WholeFS*fs,char* blockBuffer,  int index)
{

    int offset = fs->sb.dataBlockOffset + (index * DATA_BLOCK_SIZE) ;

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
    /* printf("offset %d \n", offset);
    printf("inode list block offset %d \n", fs->sb.iNodeOffset);
    printf("data block offset %d \n", fs->sb.dataBlockOffset);
    printf("sizeof SUPER block offset %d \n", sizeof(SuperBlock)); */

    char* data = readNbytesFromOffset(DATA_BLOCK_SIZE,offset, fs->fileSystemName);
    //printf("Data %s \n", data);
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
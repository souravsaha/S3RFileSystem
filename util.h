#ifndef UTIL_H
#define UTIL_H
#include "common.h"
#include "file_handling_functions.h"

DataBlock* getDataBlockFromIndex(int index);
int getNumberOfDataBlockFromSize(int size);
int readSingleRedirectDataBlock(int index,int n);
void printDirectoryContent(char *data,int size);
int getDataBlockIndex(WholeFS* fs, int inodeIndex, int nDataBlock);
int getInodeIndexFromName(WholeFS *fs,char *name,int pdIndex);
void testPrintDirectory();


#endif

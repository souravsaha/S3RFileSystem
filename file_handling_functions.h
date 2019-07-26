#ifndef FUNCTIONS_H
#define FUNCTIONS_H
/*
    This file houses file handling helper functions
 */
#include "common.h"

char* readNbytesFromOffset(int n,int offset);
int writeNBytesToOffset(char* buffer,int n,int offset);
void writeDataBlockToFile(WholeFS*fs,char* blockBuffer,  int index);
char* readDataBlockFromFile(WholeFS*fs,  int index);

#endif
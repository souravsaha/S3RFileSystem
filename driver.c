#include <stdio.h>
#include <stdlib.h>
#include "file_handling_functions.h"


int main(int argc, char const *argv[])
{
    int mounted = 0; 
    int input; 
    int inodeNumber;
    int inode;
    char *pathName = (char*)malloc(500*sizeof(char));
    char *sourcePathName = (char*)malloc(500*sizeof(char));
    char *destinationPathName = (char*)malloc(500*sizeof(char));
    char *newFilename = (char*)malloc(100*sizeof(char));
    char *fileName = (char*)malloc(100*sizeof(char));
    char *commandName = (char*)malloc(100*sizeof(char));
    char *arg1 = (char*)malloc(100*sizeof(char));
    char *arg2 = (char*)malloc(100*sizeof(char));
    int isInitialized = 0;
    initFS(argv[1]);
    WholeFS* fileSystem = readFS(argv[1],&isInitialized);

    if(!isInitialized)
        writeSuperBlock(fileSystem);
    
    printf("\nEnter 1 to mount the filesystem\n");
    scanf("%d", &input);

    if (input != 1)
    {
        printf("\n Filesystem not mounted.\n");
        return 0;
    }
    else
    {
        mounted = 1;
    }
    
    while (mounted == 1)
    {
        /*
        printf("Enter 0 to exit from the fileSystem\n");
        printf("Enter 2 to change directory\n");
        printf("Enter 3 to implement copy\n");
        printf("Enter 4 to implement move\n");
        printf("Enter 5 to implement ls\n");
        printf("Enter 6 to implement put\n");
        printf("Enter 7 to implement get\n");
        printf("Enter 8 to implement rm\n");
        printf("Enter 9 to implement ll\n");
        printf("Enter 10 to implement cat\n");
        printf("Enter 11 to implement touch\n");
        printf("Enter 12 to implement pwd\n");
        printf("Enter 13 to implement unmount\n");
        printf("Enter 14 to implement mkdir\n");
        */
        printf(">> \n");
        printf("cd \t cp \t mv \t ls \t put\n");
        printf("get \t rm \t ll \t cat \t touch\n");
        printf("pwd \t unmount \t mkdir\n");
        scanf("%s", commandName);
        
        if (strcmp(commandName, "cd") == 0)
        {
            printf("Enter the pathname\n");
            scanf("%s", arg1);
            system_cd( fileSystem, arg1);
            //break;
        }
        else if(strcmp(commandName, "cp") == 0)
        {
            printf("Currently under development\n");
            //printf("Enter the pathname to copy from\n");
            //scanf("%s", arg1);
            //printf("Enter the pathname to copy to\n");
            //scanf("%s", arg2);
            //system_cp( fileSystem, arg1, arg2);
            //break;
        }
        else if(strcmp(commandName, "mv") == 0)
        {
            printf("Currently under development\n");
            //printf("Enter the pathname to move from\n");
            //scanf("%s", arg1);
            //printf("Enter the pathname to move to\n");
            //scanf("%s", arg2);
            //system_mv( fileSystem, arg1, arg2);
            //break;
        }
        else if(strcmp(commandName, "ls") == 0)
        {
            inodeNumber = getPwdInodeNumber(fileSystem);
            system_ls( fileSystem, inodeNumber);
            //break;
        }
        else if(strcmp(commandName, "put") == 0)
        {
            printf("Currently under development\n");
            //printf("Enter the pathname\n");
            //scanf("%s", arg1);
            //system_put( fileSystem, arg1);
            //break;
        }
        else if(strcmp(commandName, "get") == 0)
        {
            printf("Currently under development\n");
            //printf("Enter the pathname\n");
            //scanf("%s", arg1);
            //system_get( fileSystem, arg1);
            //break;
        }
        else if(strcmp(commandName, "rm") == 0)
        {
            printf("Currently under development\n");
            //printf("Enter the pathname\n");
            //scanf("%s", arg1);
            //system_rm( fileSystem, arg1);
            //break;
        }
        else if(strcmp(commandName, "ll") == 0)
        {
            printf("Currently under development\n");
            //system_ll(fileSystem);
            //break;
        }
        else if(strcmp(commandName, "cat") == 0)
        {
            printf("Currently under development\n");
            //printf("Enter the filename\n");
            //scanf("%s", arg1);
            //system_cat( fileSystem, arg1);
            //break;
        }
        else if(strcmp(commandName, "touch") == 0)
        {
            //printf("Enter the filename\n");
            scanf("%s", arg1);
            int inode = system_touch( fileSystem, arg1, FILE_MODE);            
            //break;    
        }
        else if(strcmp(commandName, "pwd") == 0)
        {
            printf("Currently under development\n");
            //system_pwd(fileSystem);
            //break;
        }
        else if(strcmp(commandName, "unmount") == 0)
        {
            //system_unmount(fileSystem);
            mounted = 0;
            writeSuperBlock(fileSystem);

            //break;
        }
        else if(strcmp(commandName, "mkdir") == 0)
        {
            //printf("Enter the filename\n");
            scanf("%s", arg1);
            system_mkdir(fileSystem,arg1,FOLDER_MODE);
            //break;
        }
        else
        {
            printf("Command Not found! \n");
        }
    }

    printf("\nFilesystem has been unmounted.\n");
    
    return 0;
}

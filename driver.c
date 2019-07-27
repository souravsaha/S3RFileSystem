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

        scanf("%d", &input);
        
        if(!input)
            break;
        else if ((input < 2) || (input > 13))
        {
            printf("Invalid input. Please enter again\n");
            continue;
        }
        
        switch (input)
        {
        case 2:
            printf("Enter the pathname\n");
            scanf("%s", pathName);
            system_cd( fileSystem, pathName);
            break;
        
        case 3:
            printf("Enter the pathname to copy from\n");
            scanf("%s", sourcePathName);
            printf("Enter the pathname to copy to\n");
            scanf("%s", destinationPathName);
            //system_cp( fileSystem, sourcePathName, destinationPathName);
            break;

        case 4:
            printf("Enter the pathname to move from\n");
            scanf("%s", sourcePathName);
            printf("Enter the pathname to move to\n");
            scanf("%s", destinationPathName);
            //system_mv( fileSystem, sourcePathName, destinationPathName);
            break;

        case 5:
            inodeNumber = getPwdInodeNumber(fileSystem);
            system_ls( fileSystem, inodeNumber);
            break;

        case 6:
            printf("Enter the pathname\n");
            scanf("%s", pathName);
            //system_put( fileSystem, pathName);
            break;

        case 7:
            printf("Enter the pathname\n");
            scanf("%s", pathName);
            //system_get( fileSystem, pathName);
            break;

        case 8:
            printf("Enter the pathname\n");
            scanf("%s", pathName);
            //system_rm( fileSystem, pathName);
            break;

        case 9:
            //system_ll(fileSystem);
            break;

        case 10:
            printf("Enter the filename\n");
            scanf("%s", fileName);
            //system_cat( fileSystem, fileName);
            break;

        case 11:
            printf("Enter the filename\n");
            scanf("%s", fileName);
            int inode = system_touch( fileSystem, fileName);
            writeFS(fileSystem, inode);
            break;

        case 12:
            //system_pwd(fileSystem);
            break;

        case 13:
            //system_unmount(fileSystem);
            mounted = 0;
            break;
        default:
            break;
        }
    }

    printf("\nFilesystem has been unmounted.\n");
    
    return 0;
}


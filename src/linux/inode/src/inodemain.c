/*******************************************************************
 *
 * File         : inodemain.c
 * Author       : Naveen Raj Selvaraj
 * Date created : March 2, 2016
 * Description  : This program reads the inodes from the disk and display
 *                the directories and files as a tree
 *
 *******************************************************************/
#include "inodeinc.h"
#include "inodecli.h"

/*! *************************************************************************
 * Function Name : main 
 *
 * Description   : This is the main() function for the inode program.
 *
 * Input         : argc - total no. of command line arguments
 *                 argv - command line arguments
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID main(INT4 argc, CHAR** argv )
{
#if 1
    if (argc == 1)
    {
        printf("Enter disk name as command line argument. e.g - sudo ./inode /dev/sda1\n");
        return -1;
    }
    CHAR *pDev = argv[1];					//"/dev/sda1";
#else
    CHAR *pDev = "/dev/sdb1";
#endif
    InodeInit(pDev);
    printf("Total inodes: %d, Inodes per group: %d, Size of inode: %d\n",
            sb.s_inodes_count,
            sb.s_inodes_per_group,
            sb.s_inode_size);
    printf("Block Size: %u Bytes \n", gu4BlockSize);

    InodeCliStart();
    InodeInitExit();
}


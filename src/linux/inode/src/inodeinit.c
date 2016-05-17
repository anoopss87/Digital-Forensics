/*******************************************************************
 *
 * File         : inodeinit.c
 * Author       : Naveen Raj Selvaraj
 * Date created : April 20, 2016
 * Description  : This file contains the functions to initialize the 
 *                application
 *
 *******************************************************************/
#include "inodeinc.h"
#include <errno.h>

/*! *************************************************************************
 * Function Name : InodeInit 
 *
 * Description   : This Function is used to Initialize the inode module.
 *
 * Input         : pDev - name of the device to open 
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID InodeInit(CHAR* pDev)
{
    gu4FileDes = 0;
    gu4BlockSize = 0;
    gi4ArrayIndex = -1;

    gu4FileDes = open(pDev, O_RDWR);

    if (gu4FileDes == SYS_CALL_FAILURE) 
    {
        fprintf(stderr, "ERROR: Can't open device: %s ! %s\n", pDev, strerror(errno));
        exit(1);
    }

    /* Read super block to initialize block parameters */
    if (InodeInitReadSuperBlock() == INODE_FAILURE)
    {
        fprintf(stderr, "ERROR: Failed to read super block !\n");
        exit(1);
    }
}

/*! *************************************************************************
 * Function Name : InodeInitReadSuperBlock
 *
 * Description   : This Function is used to read the super block.
 *
 * Input         : None 
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
INT4 InodeInitReadSuperBlock()
{
    INT4 i4RetVal = 0;

    /* Read the super block */
    /* TODO Super block should not be read from inode module.
     * This function call should be replaced with the actual superblock
     * module call when it is ready.
     */
    i4RetVal = InodeUtilReadDataBlock(0, BOOT_SECTOR_OFFSET, &sb, 
            sizeof (struct ext3_super_block));
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to read super block offset: %d %s:%d \n",
                BOOT_SECTOR_OFFSET, __FILE__,
                __LINE__);
        return INODE_FAILURE;
    }

    /* Initialize the parameters */
    /* The block size is 1024*pow (2,s_log_block_size) */
    gu4BlockSize = 1024 << sb.s_log_block_size; /* Block size */
    gu4TotalInodes = sb.s_inodes_count;   /* Total no of inodes in the file system */
    gpInodeArray = malloc(MAX_INODE_ARRAY_SIZE);
}

/*! *************************************************************************
 * Function Name : InodeInitExit
 *
 * Description   : This Function is used to destroy the initialized parameters
 *                 before exiting the program.
 *
 * Input         : None 
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID InodeInitExit()
{
    close(gu4FileDes);
    free(gpInodeArray);
}

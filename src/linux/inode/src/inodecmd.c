/*******************************************************************
 *
 * File         : inodecmd.c
 * Author       : Naveen Raj Selvaraj
 * Date created : March 31, 2016
 * Description  : This file contains the command handler functions
 *
 *******************************************************************/
#include "inodeinc.h"

/*! *************************************************************************
 * Function Name : InodeCmdTreeHandler 
 *
 * Description   : This is the handler function for the command 'tree'.
 *                 This function prints the directory/files tree starting  
 *                 at the given inode.
 *
 * Input         : u4InodeNo - Root inode to start.
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID InodeCmdTreeHandler(UINT4 u4InodeNo)
{
    INT4 i4RetVal = INODE_FAILURE;
    /* Display tree from root directory, if no inode is specified */
    if (u4InodeNo == 0)
    {
        u4InodeNo = ROOT_INODE;
    }
    i4RetVal = InodeUtilPrintTree(u4InodeNo, 1);
    if (i4RetVal == INODE_FAILURE)
    {
        printf(" Invalid directory inode !\n");
    }
}

/*! *************************************************************************
 * Function Name : InodeCmdReadHandler 
 *
 * Description   : This is the handler function for the command 'read'.
 *                 This function reads the block specified and prints the 
 *                 contents in HEX.
 *
 * Input         : u4BlockNo - Block number to read.
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID InodeCmdReadHandler(UINT4 u4BlockNo)
{
    CHAR *pBuffer; 
    CHAR Buffer[MAX_BLOCK_SIZE];
    INT4 i4RetVal;

    memset(Buffer, 0, sizeof(Buffer));
    pBuffer = Buffer; 
    i4RetVal = 0;

    /* Read the Data block */
    i4RetVal = InodeUtilReadDataBlock(u4BlockNo, 0, pBuffer, gu4BlockSize);
    if (i4RetVal == INODE_FAILURE)
    {
        printf(" Could not read block: %d\n", u4BlockNo);
        return;
    }
    printf(" Contents of Data Block: %d (Block Size: %d)\n", u4BlockNo,
            gu4BlockSize);
    printf(" -----------------------------------------------\n");
    InodeUtilDumpDataBlockRaw(Buffer);
    printf(" -----------------------------------------------\n");
}

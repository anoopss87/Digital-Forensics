/*******************************************************************
 *
 * File         : inodedir.c
 * Author       : Naveen Raj Selvaraj
 * Date created : March 10, 2016
 * Description  : This file contains functions to operate on the directory
 *                inode and it's data block
 *
 *******************************************************************/
#include "inodeinc.h"

/*! *************************************************************************
 * Function Name : InodeDirValidateInode 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to validate a directory inode. A
 *                 directory inode is valid if the data block pointer points
 *                 to the actual data block of inode. If the data block pointer 
 *                 is corrupted, then the directory inode is invalid.
 *
 * Input         : u4InodeNo    -   Directory Inode number
 *                 u4BlockNo    -   data block pointed by the inode's first
 *                                  direct block pointer
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, if the inode is a valid directory inode
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeDirValidateInode(UINT4 u4InodeNo, UINT4 u4BlockNo)
{
    struct ext3_dir_entry_2 DirEntry;
    INT4 i4RetVal;

    memset(&DirEntry, 0, sizeof(DirEntry));
    i4RetVal = 0;

    /* Read the first 12 bytes from the Data block */
    i4RetVal = InodeUtilReadDataBlock(u4BlockNo, 0, (CHAR *)&DirEntry,
            DIR_CURR_ENTRY_SIZE);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to read Data Block: %d %s:%d\n", u4BlockNo,
                __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* Check if the current entry's inode number and the actual inode number are
     * the same */
    if (DirEntry.inode != u4InodeNo)
    {
        DEBUG_LOG4("ERROR: Invalid directory data block: %d Inode: %d %s:%d\n",
                u4BlockNo, u4InodeNo, 
                __FILE__, __LINE__);
        return INODE_FAILURE;
    }
    return INODE_SUCCESS; 
}

/*! *************************************************************************
 * Function Name : InodeDirReadRecord 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to read a directory entry from the 
 *                 data block of a directory inode.
 *
 * Input         : pEntries     -   Buffer containing the contents of the data
 *                                  block
 *                 u4StartPos   -   Offset to start reading
 *
 * Output        : pDirEntry    -   the directory entry read from the block
 * 
 * Returns       : INODE_SUCCESS, if the entry is read successfully
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeDirReadRecord(CHAR *pEntries, UINT4 u4StartPos, 
        struct ext3_dir_entry_2 *pDirEntry)
{
    CHAR *pPos = NULL;

    if (pEntries == NULL)
    {
        return INODE_FAILURE;
    }

    pPos = pEntries + u4StartPos;
    memcpy(pDirEntry, pPos, sizeof(UINT8));
    strncpy(pDirEntry->name, pPos + DIR_ENTRY_NAME_OFFSET,
            pDirEntry->name_len);
    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeDirValidateDataBlock 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to check if a data block is a directory
 *                 data block or not. If the data block contains directory
 *                 entries, then it is a valid directory data block. 
 *
 * Input         : pEntries     -   Buffer containing the contents of the data
 *                                  block
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, if the data block is a valid directory data
 *                 block
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeDirValidateDataBlock(CHAR *pEntries)
{
    struct ext3_dir_entry_2 DirEntry;
    UINT1 u1IsValid = FALSE;

    if (pEntries != NULL)
    {
        /* A valid directory data block contains the following entry
         * in the first 12 bytes.
         * -----------------------------------
         * |   0 - 4  | 5  | 6 | 7 |  8 - 11 |
         * |----------|----|---|---|---------|
         * | Inode no | 12 | 1 | 2 | .\0\0\0 |
         * -----------------------------------
         */
        memcpy(&DirEntry, pEntries, DIR_CURR_ENTRY_SIZE);
        if (DirEntry.rec_len == DIR_CURR_ENTRY_SIZE &&
                DirEntry.name_len == strlen(DIR_NAME_CURRENT) &&
                DirEntry.file_type == FILE_TYPE_DIR &&
                (strcmp(DirEntry.name, DIR_NAME_CURRENT) == 0))
        {
            u1IsValid = TRUE;
        }
    }
    if (u1IsValid == FALSE)
    {
        /* The data block could be corrupted or it may belong to a file */
        DEBUG_LOG2("ERROR: Not a Directory Data block %s:%d\n", __FILE__,
                __LINE__);
        return INODE_FAILURE;
    }
    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeDirChangeParentEntry 
 *
 * Author        : Anusha Seshadri
 *
 * Description   : This Function is used to change the parent directory entry of a 
 *                 directory inode.
 *
 * Input         : pDirEntry    -   new parent entry
 *                 u4BlockNo    -   the directory data block where the parent
 *                                  entry needs to be changed
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, if the parent is changed successfully
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeDirChangeParentEntry(struct ext3_dir_entry_2 *pDirEntry, UINT4 u4BlockNo)
{
    INT4 i4RetVal = 0;

    i4RetVal = InodeUtilWriteDataBlock(u4BlockNo, DIR_PARENT_ENTRY_OFFSET,
            pDirEntry, sizeof(UINT4)); 
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG5("ERROR: Failed to write parent entry to Block: %d Start: %d Size: %d %s:%d\n",
                u4BlockNo, DIR_PARENT_ENTRY_OFFSET, sizeof(UINT4), __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeDirAddChildEntry 
 *
 * Author        : Anusha Seshadri
 *
 * Description   : This Function is used to add a child entry to a directory
 *                 inode. The new entry would be appended at the end of the last
 *                 entry.
 *
 * Input         : pDirEntry    -   new child entry
 *                 u4BlockNo    -   the directory data block where the child
 *                                  entry needs to be changed
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, if the child entry is addded successfully
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeDirAddChildEntry(struct ext3_dir_entry_2 *pNewChild, UINT4 u4BlockNo)
{
    struct ext3_dir_entry_2 DirEntry;
    CHAR *pBuffer;
    CHAR buffer[MAX_BLOCK_SIZE]; 
    INT4 i4BytesReadSoFar = 0;
    UINT2 u2Len= 0;
    INT4 i4RetVal = 0;
    UINT4 u4Index = 0;
    UINT4 u4NextPos = 0;

    memset(&DirEntry, 0, sizeof(DirEntry));
    memset(buffer, 0, sizeof(buffer));
    pBuffer = buffer;
    
    /*read datablock into buffer */
    i4RetVal = InodeUtilReadDataBlock(u4BlockNo, 0, pBuffer, gu4BlockSize);
    if(i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to read block: %d %s:%d\n", u4BlockNo, __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* iterate through all directory entries */
    for (u4Index = 0; u4Index < gu4BlockSize; u4Index++)
    {
        memset(&DirEntry, 0, sizeof(DirEntry));

        /* Read the next directory record entry in the data block */
        InodeDirReadRecord(pBuffer, u4NextPos, &DirEntry);

        /* Move the offset to the start of next record */
        u4NextPos += DirEntry.rec_len;

        /* If end of records, break the loop */
        if (u4NextPos == gu4BlockSize)
        {
            break;
        }
        else
        {
            i4BytesReadSoFar = i4BytesReadSoFar + DirEntry.rec_len;
        }
    }

    /* setting the actual record length of the last dir entry */
    DirEntry.rec_len = DIR_REC_LEN(DirEntry);
    memcpy(pBuffer + i4BytesReadSoFar, &DirEntry, DirEntry.rec_len);
    u2Len = DirEntry.rec_len;

    /* adding the new directory entry to the buffer*/
    pNewChild->rec_len = gu4BlockSize - i4BytesReadSoFar - u2Len; 
    memcpy(pBuffer + i4BytesReadSoFar + u2Len, pNewChild, pNewChild->rec_len);

    /* rewrite the block with new data */
    i4RetVal = InodeUtilWriteDataBlock(u4BlockNo, 0, pBuffer, gu4BlockSize);
    if (i4RetVal == SYS_CALL_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to add child entry at Block: %d, %s:%d\n",
                u4BlockNo, __FILE__, __LINE__);
        return INODE_FAILURE;
    }
    return INODE_SUCCESS;
}

#if 0
INT4 InodeDirReadRecord(CHAR *pEntries, UINT4 u4StartPos, 
        struct ext3_dir_entry_2 *pDirEntry)
{
    CHAR *pPos = pEntries + u4StartPos;
    UINT2 u2RecLen;

    u2RecLen = 0;

    memcpy(pDirEntry, pPos, sizeof(UINT8));
    strncpy(pDirEntry->name, pPos + DIR_ENTRY_NAME_OFFSET,
      pDirEntry->name_len);
    if (InodeDirVerifyDirRecord(pEntries, pDirEntry, u4StartPos) == TRUE)
    {
        if (strlen(pDirEntry->name) == pDirEntry->name_len)
        {
            return INODE_SUCCESS;
        }
        else
        {
            printf("ERROR: Directory Data block name field corrupted ! %s:%d\n", __FILE__,
                    __LINE__);
            return INODE_FAILURE; 
        }
    }

    printf("ERROR: Directory Data block corrupted ! %s:%d\n", __FILE__,
            __LINE__);
    return INODE_FAILURE;
}

INT4 InodeDirVerifyDirRecord(CHAR *pEntries, struct ext3_dir_entry_2 *pDirEntry, 
                             UINT2 u4StartPos)
{
    UINT2 u2Len= 0;
    UINT2 u2RecLen= 0;

    if (pDirEntry->inode == 0)
    {
        printf("ERROR: Directory Data block has inode value: 0 %s:%d\n", __FILE__,
                __LINE__);
        return FALSE;
    }

    u2RecLen = pDirEntry->name_len + DIR_ENTRY_NAME_OFFSET;
    u2Len = ceil((float)u2RecLen / 4) * 4;
    if (pDirEntry->rec_len == u2Len)
    {
        /* Valid Record */
        return TRUE; 
    }
    else if (pDirEntry->rec_len == (gu4BlockSize - u4StartPos))   
    {
        /* Last record */
        return TRUE;
    }
    else
    {
        /* If the rec_len does not match with the computed length, then there are
         * two possibilities,
             1. The next directory entry/entries could be deleted previously.
             2. The current entry is corrupted 

         To verify Case 1:
            1. Check the inode of the first deleted entry and make sure it is 0
            2. Goto the next actual entry pointed by rec_len and validate it */

        UINT4 u4DelInode;
        struct ext3_dir_entry_2 NextEntry;
        INT4 i4RetVal = 0;
        
        memset(&NextEntry, 0, sizeof(NextEntry));

        memcpy(pEntries + u2Len, &u4DelInode, sizeof(UINT4));
        if (u4DelInode == 0)
        {
            i4RetVal = InodeDirReadRecord(pEntries,
                    u4StartPos + pDirEntry->rec_len, &NextEntry);
            if (i4RetVal == INODE_SUCCESS)
            {
                return TRUE;
            }
            else
            {
                printf("ERROR: Deleted directory entry corruption %s:%d\n", __FILE__,
                        __LINE__);
                return FALSE;
            }
        }
    }

    printf("ERROR: Directory Data block rec_len has invalid value: %d %s:%d\n",
            pDirEntry->rec_len, __FILE__, __LINE__);
    return FALSE;
}
#endif

/*******************************************************************
 *
 * File         : inodefix.c
 * Author       : Naveen Raj Selvaraj
 * Date created : April 3, 2016
 * Description  : This file contains the functions that perform the 
 *                recovery of the file system inodes.
 *
 *******************************************************************/
#include "inodeinc.h"
#include "inodetime.h"
static VOID InodeFixSetInodeArray(UINT4 u4Index, UINT1 u1Value);
static INT1 InodeFixIsCourruptedInode(UINT4 u4InodeNo);
static INT1 InodeFixCheckDuplicate(UINT4 u4Inode, UINT4 u4BlockNo);

/*! *************************************************************************
 * Function Name : InodeFixExecute 
 *
 * Description   : This Function is used to recover the corrupted directories
 *                 children directories. Please refer the documentation for the
 *                 recovery algorithm.
 *
 * Input         : None 
 *
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, on successful recovery
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeFixExecute()
{
    UINT4 u4Index;
    struct ext3_inode Inode;
    struct ext3_inode ParentInode;
    struct ext3_dir_entry_2 ParentEntry;
    struct ext3_dir_entry_2 NewDirEntry;
    INT4 i4RetVal;
    CHAR Buffer[MAX_BLOCK_SIZE];
    CHAR *pBuffer; 

    memset(gpInodeArray, 0, MAX_INODE_ARRAY_SIZE);
    pBuffer = Buffer;
    START_TIME();
    /* Iterate through all the inodes */
    for (u4Index = ROOT_INODE; u4Index < gu4TotalInodes; u4Index++)
    {
        memset(&Inode, 0, sizeof(Inode));
        memset(Buffer, 0, sizeof(Buffer));
        memset(&ParentEntry, 0, sizeof(ParentEntry));
        memset(&NewDirEntry, 0, sizeof(NewDirEntry));
        memset(&ParentInode, 0, sizeof(ParentInode));

        /* Read the inode */
        i4RetVal = InodeUtilReadInode(u4Index, &Inode);
        if (i4RetVal == INODE_FAILURE)
        {
            DEBUG_LOG3("ERROR: Failed to read Inode: %d %s:%d\n", u4Index,
                    __FILE__, __LINE__);
            return INODE_FAILURE;
        }

        /* Check if the inode is unused */
        if (InodeUtilIsFreeInode(&Inode) == TRUE)
        {
            /* InodeFixSetInodeArray(u4Index, FALSE); */
            continue;
        }

        /* Check if this is a directory inode */
        if (S_ISDIR(Inode.i_mode))
        {
            /* Check if this is a valid directory inode */
            i4RetVal = InodeDirValidateInode(u4Index, Inode.i_block[0]);
            if (i4RetVal == INODE_FAILURE)
            {
                /* Mark this inode as corrupted */
                InodeFixSetInodeArray(u4Index, TRUE);

                if (u4Index == ROOT_INODE)
                {
                    printf(" Root Inode Corrupted\n");
                    InodeFixRootDataBlockPtr(&Inode);
                }
                continue;
            }

            /* Read the Data block */
            i4RetVal = InodeUtilReadDataBlock(Inode.i_block[0], 0, pBuffer, gu4BlockSize);
            if (i4RetVal == INODE_FAILURE)
            {
                DEBUG_LOG3("ERROR: Failed to read Data Block: %d %s:%d\n",
                        Inode.i_block[0],
                        __FILE__, __LINE__);
                return INODE_FAILURE;
            }

            /* Now check if the parent of this directory is
             * corrupted or not */
            i4RetVal = InodeDirReadRecord(pBuffer, DIR_PARENT_ENTRY_OFFSET, &ParentEntry);
            if (i4RetVal == INODE_FAILURE)
            {
                DEBUG_LOG3("ERROR: Failed to read directory entry from block: %d %s:%d\n",
                        Inode.i_block[0], __FILE__,__LINE__);
                return INODE_FAILURE;
            }

            if (InodeFixIsCourruptedInode(ParentEntry.inode) == TRUE)
            {
                /* Parent inode is corrupted. Now, make this directory as a
                 * child of the root directory.
                 * To do this, 
                 *  1. Add a child entry in the root inode for this directory.
                 *  2. Change the parent entry of this directory to root inode.
                 */

                /* ADD Child START */
                /* Read the root inode */
                i4RetVal = InodeUtilReadInode(ROOT_INODE, &ParentInode);
                if (i4RetVal == INODE_FAILURE)
                {
                    DEBUG_LOG3("ERROR: Failed to read Inode: %d %s:%d\n", ParentEntry.inode,
                            __FILE__, __LINE__);
                    return INODE_FAILURE;
                }

                /* If root is the parent if this inode and it is corrupted,
                 * first check, whether the root data block contains an entry
                 * for this inode or not. If it contains do not add, otherwise
                 * add a new entry */
                if (ParentEntry.inode == ROOT_INODE)
                {
                    if (InodeFixCheckDuplicate(u4Index,
                                gu4RootDataBlock) == TRUE)
                    {
                        /* Entry already exists, don't do anything */
                        continue;
                    }
                }

                /* Construct the new child entry */
                NewDirEntry.inode = u4Index;
                NewDirEntry.file_type = FILE_TYPE_DIR;
                sprintf(NewDirEntry.name, "%s%d", REC_NAME_PREFIX, u4Index);
                NewDirEntry.name_len = strlen(NewDirEntry.name);
                NewDirEntry.rec_len = DIR_REC_LEN(NewDirEntry); 

                /* TODO: can a directory take more than one block ? */
                i4RetVal = InodeDirAddChildEntry(&NewDirEntry,
                        ParentInode.i_block[0]);
                if (i4RetVal == INODE_FAILURE)
                {
                    DEBUG_LOG3("ERROR: Failed to add child entry Inode: %d %s:%d\n", 
                            u4Index, __FILE__, __LINE__);
                    return INODE_FAILURE;
                }
                /* ADD Child END */

                /* CHANGE Parent START */
                /* Construct the new parent entry */
                memset(&NewDirEntry, 0, sizeof(NewDirEntry));
                NewDirEntry.inode = ROOT_INODE;
                NewDirEntry.file_type = FILE_TYPE_DIR;
                strncpy(NewDirEntry.name, DIR_NAME_PARENT,
                        strlen(DIR_NAME_PARENT)); 
                NewDirEntry.name_len = strlen(NewDirEntry.name);
                NewDirEntry.rec_len = DIR_PARENT_ENTRY_SIZE;

                /* Add this entry to the current inode */
                i4RetVal = InodeDirChangeParentEntry(&NewDirEntry,
                        Inode.i_block[0]);
                if (i4RetVal == INODE_FAILURE)
                {
                    DEBUG_LOG3("ERROR: Failed to change parent entry Inode: %d %s:%d\n", 
                            u4Index, __FILE__, __LINE__);
                    return INODE_FAILURE;
                }
                /* CHANGE Parent END */
                printf(" Inode: %d recovered, corrupted parent: %d\n", u4Index,
                        ParentEntry.inode);
            }
        }
    }
    END_TIME();
    printf(" Recovery completed ! Time elapsed: %.2f ms\n", ELAPSED_TIME());
    return INODE_FAILURE;
}

/*! *************************************************************************
 * Function Name : InodeFixSetInodeArray 
 *
 * Description   : This Function is mark an inode as corrupted/not corrupted.
 *
 * Input         : u4InodeNo    -   Inode number 
 *                 u1Value      -   TRUE for corrupted, FALSE otherwise
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
static VOID InodeFixSetInodeArray(UINT4 u4InodeNo, UINT1 u1Value)
{
    UINT4 u4InodeArrayIndex;    /* Array index for the inode no */
    UINT4 u4InodeBitIndex;      /* Bit in the array index */
    UINT1 au1Mask[] = { MASK_BIT_0,
                        MASK_BIT_1,
                        MASK_BIT_2,
                        MASK_BIT_3,
                        MASK_BIT_4,
                        MASK_BIT_5,
                        MASK_BIT_6,
                        MASK_BIT_7};

    if (u4InodeNo >= gu4TotalInodes)
    {
        DEBUG_LOG3(" ERROR: Invalid Inode Index: %d %s %d\n", u4InodeNo, __FILE__,
                __LINE__);
        return;
    }

    /* getting the byte array index*/
    u4InodeArrayIndex = u4InodeNo/BYTE; 

    /* getting the offset at that index */
    u4InodeBitIndex = (u4InodeNo % BYTE) ;

    if(u1Value == TRUE)
    {
        /* set the bit corresponding to the Inode to 1 */
        gpInodeArray[u4InodeArrayIndex] |= au1Mask[u4InodeBitIndex];
    }
    else
    {
        /* set the bit corresponding to the Inode to 0 */
        gpInodeArray[u4InodeArrayIndex] &= ~au1Mask[u4InodeBitIndex];
    }
}

/*! *************************************************************************
 * Function Name : InodeFixIsCourruptedInode 
 *
 * Description   : This Function is used to check if an inode is corrupted or
 *                 not
 *
 * Input         : u4InodeNo    -   Inode number
 *
 * Output        : None
 * 
 * Returns       : TRUE, if the inode is corrupted
 *                 FALSE otherwise
 *
 **************************************************************************/
static INT1 InodeFixIsCourruptedInode(UINT4 u4InodeNo)
{
    UINT4 u4InodeArrayIndex;    /* Array index for the inode no */
    UINT4 u4InodeBitIndex;      /* Bit in the array index */
    UINT1 au1Mask[] = { MASK_BIT_0,
                        MASK_BIT_1,
                        MASK_BIT_2,
                        MASK_BIT_3,
                        MASK_BIT_4,
                        MASK_BIT_5,
                        MASK_BIT_6,
                        MASK_BIT_7};

    if (u4InodeNo >= gu4TotalInodes)
    {
        DEBUG_LOG3(" ERROR: Invalid Inode Index: %d %s %d\n", u4InodeNo, __FILE__,
                __LINE__);
        return FALSE;
    }

    /* getting the byte array index*/
    u4InodeArrayIndex = u4InodeNo/BYTE; 

    /* getting the offset at that index */
    u4InodeBitIndex = (u4InodeNo % BYTE);

    /* Check if the bit is set to TRUE or FALSE */
    if ((gpInodeArray[u4InodeArrayIndex] & au1Mask[u4InodeBitIndex]) == FALSE)
    {
        return FALSE;
    }
    return TRUE;
}

/*! *************************************************************************
 * Function Name : InodeFixRootDataBlockPtr 
 *
 * Description   : This Function is used to fix the root inode's data block
 *                 pointer if it is corrupted.
 *
 * Input         : pInode   -   Root Inode structure
 *
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, if the data block pointer is fixed
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeFixRootDataBlockPtr(struct ext3_inode *pInode)
{
    UINT8 u8InodeOffset;
    UINT8 u8DirBlockOffset;
    UINT4 u4DataBlock;
    INT4 i4RetVal;

    u8InodeOffset = 0;
    u8DirBlockOffset = 0;
    i4RetVal = 0;

    /* Get the byte offset of the Inode */
    i4RetVal = InodeUtilGetInodeOffset(ROOT_INODE, &u8InodeOffset);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: failed to obtain inode byte offset Inode no: %d %s:%d\n",
                ROOT_INODE, __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* The first data block is usually assigned to the root directory. Calculate
     * the first data block and set the direct block pointer of root inode to
     * this block number */
    u4DataBlock = (u8InodeOffset + (sb.s_inodes_per_group * sb.s_inode_size)) /
        gu4BlockSize;
    u8DirBlockOffset = u8InodeOffset + DIRECT_BLOCK_OFFSET;

    pInode->i_block[0] = u4DataBlock;

    gu4RootDataBlock = u4DataBlock;

    /* Write to the disk */
    i4RetVal = InodeUtilWriteDataOffset(u8DirBlockOffset, &u4DataBlock, sizeof (UINT4));
    if(i4RetVal == SYS_CALL_FAILURE)
    {
        DEBUG_LOG2( "ERROR: Failed to write root data block ptr %s:%d\n", __FILE__, __LINE__ );
        return INODE_FAILURE;
    }
    printf(" Root Inode data block pointer fixed\n");
    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeFixCheckDuplicate
 *
 * Description   : This Function is used to check if a directory data block
 *                 already contains an entry for the inode specified.
 *
 * Input         : u4Inode      -  Inode number to be checked
 *                 u4BlockNo    -  Block number to be checked for a duplicate
 *                                 entry of u4Inode
 * Output        : None
 * 
 * Returns       : TRUE, if there is a duplicate entry or failure occured
 *                 FALSE otherwise
 *
 **************************************************************************/
static INT1 InodeFixCheckDuplicate(UINT4 u4Inode, UINT4 u4BlockNo)
{
    struct ext3_dir_entry_2 DirEntry;
    CHAR *pBuffer; 
    UINT4 u4Index;
    UINT4 u4NextPos;
    CHAR Buffer[MAX_BLOCK_SIZE];
    INT4 i4RetVal;

    memset(&DirEntry, 0, sizeof(DirEntry));
    memset(Buffer, 0, sizeof(Buffer));
    pBuffer = Buffer; 
    u4Index = 0;
    u4NextPos = 0;
    i4RetVal = 0;

    /* Read the data block 
     * TODO: optimization could be done by
     * keeping the root data block in buffer */
    /* Read the Data block */
    i4RetVal = InodeUtilReadDataBlock(u4BlockNo, 0, pBuffer, gu4BlockSize);
    if (i4RetVal == INODE_FAILURE)
    {   
        DEBUG_LOG3("ERROR: Failed to read Data Block: %d %s:%d\n", u4BlockNo,
                __FILE__, __LINE__);
        /* Prevent write by returning TRUE */
        return TRUE;
    }  
    /* Read all the directory entries one at a time */
    for (u4Index = 0; u4Index < gu4BlockSize; u4Index++)
    {
        memset(&DirEntry, 0, sizeof(DirEntry));

        /* Read the next directory record entry in the data block */
        InodeDirReadRecord(pBuffer, u4NextPos, &DirEntry);

        if (DirEntry.inode == u4Inode)
        {
            /* Entry already exists */
            return TRUE;
        }
        /* Move the offset to the start of next record */
        u4NextPos += DirEntry.rec_len;

        /* If end of records, break the loop */
        if (u4NextPos == gu4BlockSize)
        {
            break;
        }
    }
    /* Entry does not exist already */
    return FALSE;
}

/*******************************************************************
 *
 * File         : inodeutil.c
 * Author       : Naveen Raj Selvaraj
 * Date created : March 2, 2016
 * Description  : This file contians the utility functions used to print inode,
 *                data block, corrupt/uncorrupt inode etc.
 *
 *******************************************************************/
#include "inodeinc.h"

/*! *************************************************************************
 * Function Name : InodeUtilPrintInode 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to print the contents of an inode
 *
 * Input         : u4InodeNo    -   Inode number
 *
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, when successfully printed
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilPrintInode(UINT4 u4InodeNo)
{
    struct ext3_inode Inode;
    INT4 i4RetVal;

    memset(&Inode, 0, sizeof(Inode));
    i4RetVal = 0;

    i4RetVal = InodeUtilReadInode(u4InodeNo, &Inode);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to read Inode: %d %s:%d\n", u4InodeNo, __FILE__,
                __LINE__);
        return INODE_FAILURE;
    }
#if 1
    if (InodeUtilIsFreeInode(&Inode) == TRUE)
    {
        printf(" Inode unused\n");
        return INODE_SUCCESS;
    }
#endif
    InodeUtilDumpInode(&Inode);
    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilPrintTree 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to print the tree/subtree of the file system
 *                 objects starting at a specified directory inode in depth
 *                 first order.
 *
 * Input         : u4InodeNo    -   Inode number
 *                 u4Level      -   number inidicating the depth of the file
 *                                  system object in the tree (this is used to
 *                                  format the output. e.g: root inode for the
 *                                  tree should be called with u4Level = 1)
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, when successfully printed
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilPrintTree(UINT4 u4InodeNo, UINT4 u4Level)
{
    struct ext3_inode Inode;
    INT4 i4RetVal;

    memset(&Inode, 0, sizeof(Inode));
    i4RetVal = 0;

    i4RetVal = InodeUtilReadInode(u4InodeNo, &Inode);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to read Inode: %d %s:%d\n", u4InodeNo, __FILE__,
                __LINE__);
        return INODE_FAILURE;
    }

    /* validate the directory inode and return failure if validation failed */
    i4RetVal = InodeDirValidateInode(u4InodeNo, Inode.i_block[0]);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Not a valid directory Inode: %d %s:%d\n", u4InodeNo, __FILE__,
                __LINE__);
        return INODE_FAILURE;
    }

    i4RetVal = InodeUtilPrintChildren(Inode.i_block[0], u4Level);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to print directory entries for Inode: %d %s:%d\n", u4InodeNo, __FILE__,
                __LINE__);
        return INODE_FAILURE;
    }
    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilPrintChildren
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to print the children of a directory
 *                 inode as a part of printing the tree. If the child is a directory
 *                 inode, then it calls the InodeUtilPrintTree function to print
 *                 the subtree of that child.
 *
 * Input         : u4BlockNo    -   Data block of a directory inode
 *                 u4Level      -   number inidicating the depth of the file
 *                                  system object in the tree.(used to
 *                                  format the output)
 *
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, when successfully printed
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilPrintChildren(UINT4 u4BlockNo, UINT4 u4Level)
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

    /* Read the Data block */
    i4RetVal = InodeUtilReadDataBlock(u4BlockNo, 0, pBuffer, gu4BlockSize);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to read Data Block: %d %s:%d\n", u4BlockNo,
                __FILE__, __LINE__);
        return INODE_FAILURE;
    }

#if 0
    /* Check if this data block belongs to a directory inode, if not throw error */
    if (InodeDirValidateDataBlock(pBuffer) == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Not a directory data block: %d %s:%d\n", u4BlockNo,
                __FILE__, __LINE__);
        return INODE_FAILURE;
    }
#endif

    /* Read all the directory entries one at a time */
    for (u4Index = 0; u4Index < gu4BlockSize; u4Index++)
    {
        memset(&DirEntry, 0, sizeof(DirEntry));

        /* Parse a directory record entry in the data block */
        i4RetVal = InodeDirReadRecord(pBuffer, u4NextPos, &DirEntry);
        if (i4RetVal == INODE_FAILURE)
        {
            DEBUG_LOG3("ERROR: Failed to read directory entry from block: %d %s:%d\n",
                    u4BlockNo, __FILE__,__LINE__);
            return INODE_FAILURE;
        }

        /* Print the entry */
        InodeUtilDumpDirEntry(&DirEntry, FILE_TYPE_FILES, u4Level);

        /* If the entry is a directory, print the subtree of this directory */
        if (DirEntry.file_type == 2 && 
            (strcmp(DirEntry.name, DIR_NAME_PARENT) != 0) &&
            (strcmp(DirEntry.name, DIR_NAME_CURRENT) != 0))
        {
            InodeUtilPrintTree(DirEntry.inode, u4Level + 1);
        }

        /* Move the offset to the start of next record */
        u4NextPos += DirEntry.rec_len;

        /* If end of records, break the loop */
        if (u4NextPos == gu4BlockSize)
        {
            break;
        }
    }
}

/*! *************************************************************************
 * Function Name : InodeUtilPrintDirDataBlock 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to print the directory entries present
 *                 in a directory data block
 *
 * Input         : u4BlockNo    -   directory data block number
 *
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, when successfully printed 
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
/* NOTE: This functionality can be done by modifying the InodeUtilPrintChildren
 * and InodeUtilDumpDirEntry functions. 
 * To keep the code clean, the below function was implemented */
INT4 InodeUtilPrintDirDataBlock(UINT4 u4BlockNo)
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

    /* Read the Data block */
    i4RetVal = InodeUtilReadDataBlock(u4BlockNo, 0, pBuffer, gu4BlockSize);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to read Data Block: %d %s:%d\n", u4BlockNo,
                __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* Check if this data block belongs to a directory inode, if not throw error */
    if (InodeDirValidateDataBlock(pBuffer) == INODE_FAILURE)
    {
        printf(" Not a directory data block\n");
        return INODE_FAILURE;
    }

    printf("------------------------------------------------------------------------\n");
    printf("|  Inode no.   | rec len | name len | type |              name         |\n");
    printf("------------------------------------------------------------------------\n");
    /* Read all the directory entries one at a time */
    for (u4Index = 0; u4Index < gu4BlockSize; u4Index++)
    {
        memset(&DirEntry, 0, sizeof(DirEntry));

        /* Parse a directory record entry in the data block */
        i4RetVal = InodeDirReadRecord(pBuffer, u4NextPos, &DirEntry);
        if (i4RetVal == INODE_FAILURE)
        {
            DEBUG_LOG3("ERROR: Failed to read directory entry from block: %d %s:%d\n",
                    u4BlockNo, __FILE__,__LINE__);
            return INODE_FAILURE;
        }

        /* Print the entry */
        printf("| %12d | %7d | %8d | %4d | %25s |\n", DirEntry.inode,
                                                     DirEntry.rec_len,
                                                     DirEntry.name_len,
                                                     DirEntry.file_type,
                                                     DirEntry.name);

        /* Move the offset to the start of next record */
        u4NextPos += DirEntry.rec_len;

        /* If end of records, break the loop */
        if (u4NextPos == gu4BlockSize)
        {
            break;
        }
    }
    printf("------------------------------------------------------------------------\n");
}

/*! *************************************************************************
 * Function Name : InodeUtilReadDataBlock 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to read a file system block 
 *
 * Input         : u4BlockNo    -   Block number to read
 *                 u4Size       -   size of data to read in bytes
 *
 * Output        : pBuffer      -   Buffer containg the contents of the block
 * 
 * Returns       : INODE_SUCCESS, if the read succeeds
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilReadDataBlock(UINT4 u4BlockNo, UINT2 u2StartPos, VOID *pBuffer, UINT4 u4Size)
{
    UINT8 u8Offset;
    INT4 i4RetVal;

    u8Offset = ((UINT8)gu4BlockSize * u4BlockNo) + u2StartPos;
    i4RetVal = 0;

    /* Read the contents of the data block */
    if (lseek64(gu4FileDes, u8Offset, SEEK_SET) == SYS_CALL_FAILURE)
    {
        DEBUG_LOG2("ERROR: lseek error %s:%d\n", __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* Read the block */
    i4RetVal = read(gu4FileDes, pBuffer, u4Size); 
    if (i4RetVal == SYS_CALL_FAILURE)
    {
        DEBUG_LOG2("ERROR: read error %s:%d\n", __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilReadDataOffset
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to read data from any location 
 *
 * Input         : u8Offset     -   Byte offset to read
 *                 u4Size       -   size of data to read in bytes
 *
 * Output        : pBuffer      -   Buffer containg the contents of the block
 * 
 * Returns       : INODE_SUCCESS, if the read succeeds
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilReadDataOffset(UINT8 u8Offset, VOID *pBuffer, UINT4 u4Size)
{
    UINT4 u4BlockNo;
    UINT2 u2Start;
    INT4 i4RetVal;

    GET_BLOCK_OFFSET_FROM_BYTE_OFFSET(u4BlockNo, u2Start, u8Offset);
    /* write to the block */
    i4RetVal = InodeUtilReadDataBlock(u4BlockNo, u2Start, pBuffer, u4Size); 
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG5("ERROR: read failed Block: %d Start: %d Size: %d %s:%d\n",
                u4BlockNo, u2Start, u4Size, __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilWriteDataBlock 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to write data to a file system block 
 *
 * Input         : u4BlockNo    -   Block number to write
 *                 u2StartPos   -   byte offset inside the block to start 
 *                 u4Size       -   size of data to read in bytes
 *                 pBuffer      -   Buffer containg the data to write
 *
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, if the write succeeds
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilWriteDataBlock(UINT4 u4BlockNo, UINT2 u2StartPos, VOID *pBuffer, UINT4 u4Size)
{
    UINT8 u8Offset;
    INT4 i4RetVal;

    /* NOTE: validation not performed on u2StartPos and u4Size. If they are
     * larger than the block size, the write would span across multiple blocks
     * without any problem */
    u8Offset = ((UINT8)gu4BlockSize * u4BlockNo) + u2StartPos;
    i4RetVal = 0;

    /* seek to the offset */
    if (lseek64(gu4FileDes, u8Offset, SEEK_SET) == SYS_CALL_FAILURE)
    {
        DEBUG_LOG5("ERROR: lseek error Block: %d Start: %d Size: %d %s:%d\n",
                u4BlockNo, u2StartPos, u4Size, __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* write to the block */
    i4RetVal = write(gu4FileDes, pBuffer, u4Size); 
    if (i4RetVal == SYS_CALL_FAILURE)
    {
        DEBUG_LOG5("ERROR: write error Block: %d Start: %d Size: %d %s:%d\n",
                u4BlockNo, u2StartPos, u4Size, __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilWriteDataOffset
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to write data at a particualar offset
 *
 * Input         : u8Offset     -   Byte offset to write
 *                 u4Size       -   size of data to read in bytes
 *                 pBuffer      -   Buffer containg the data to write
 *
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS, if the write succeeds
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilWriteDataOffset(UINT8 u8Offset, VOID *pBuffer, UINT4 u4Size)
{
    UINT4 u4BlockNo;
    UINT2 u2Start;
    INT4 i4RetVal;

    GET_BLOCK_OFFSET_FROM_BYTE_OFFSET(u4BlockNo, u2Start, u8Offset);
    /* write to the block */
    i4RetVal = InodeUtilWriteDataBlock(u4BlockNo, u2Start, pBuffer, u4Size); 
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG5("ERROR: write error Block: %d Start: %d Size: %d %s:%d\n",
                u4BlockNo, u2Start, u4Size, __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilReadInode 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to read an inode
 *
 * Input         : u4InodeNo    -   Inode number to read
 *
 * Output        : pNewInode    -   pointer to the structure containing the
 *                                  inode contents
 * 
 * Returns       : INODE_SUCCESS, on successful read 
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilReadInode(UINT4 u4InodeNo, struct ext3_inode *pNewInode)
{
    struct ext3_inode Inode;
    struct ext3_group_desc GroupDes;
    UINT8 u8Offset;
    INT4 i4RetVal;

    memset(&Inode, 0, sizeof(Inode));
    memset(&GroupDes, 0, sizeof(GroupDes));
    i4RetVal = 0;

    /* Obtain the Inode offset in bytes */
    i4RetVal = InodeUtilGetInodeOffset(u4InodeNo, &u8Offset);
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: failed to obtain inode byte offset Inode no: %d %s:%d\n",
                u4InodeNo, __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* Read the inode */
    i4RetVal = InodeUtilReadDataOffset(u8Offset, &Inode, sizeof (struct ext3_inode));
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG3("ERROR: Failed to read Inode: %d %s:%d\n", u4InodeNo, __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* Copy the inode to the output pointer */
    memcpy(pNewInode, &Inode, sizeof(Inode));

   return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilDumpDirEntry 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to print a directory entry.
 *
 * Input         : pDirEntry    -   pointer to the directory entry to print
 *                 Filter       -   FILE_TYPE_DIRS - print only directories 
 *                                  FILE_TYPE_FILES - print directories and
 *                                                   files 
 *                                  FILE_TYPE_ALL - print everything including
 *                                                  . and .. entries
 *                 u4Level      -   depth of this entry in the tree (used to
 *                                  format the output)
 * Output        : None
 * 
 * Returns       : None 
 *
 **************************************************************************/
VOID InodeUtilDumpDirEntry(struct ext3_dir_entry_2 *pDirEntry, tFileFilter Filter, UINT4 u4Level)
{
    UINT1 u1Flag = FALSE;
    CHAR Trail =  (pDirEntry->file_type == 2) ? '/' : ' ';

    switch (Filter)
    {
        case FILE_TYPE_DIRS:
            /* Print only the directories */
            if (pDirEntry->file_type == FILE_TYPE_DIR && 
                strcmp(pDirEntry->name, DIR_NAME_CURRENT) != 0 &&
                strcmp(pDirEntry->name, DIR_NAME_PARENT) != 0)
            {
                u1Flag = TRUE;
            }
            break;

        case FILE_TYPE_FILES:
            /* Print all dirs and files except hidden dirs */
            if ((strcmp(pDirEntry->name, DIR_NAME_CURRENT) != 0 &&
                        (strcmp(pDirEntry->name, DIR_NAME_PARENT) != 0)))
            {
                u1Flag = TRUE;
            }
            break;

        case FILE_TYPE_ALL:
        default:
            u1Flag = TRUE;
    }
    if (u1Flag == TRUE)
    {
        UINT4 space;
        for (space = u4Level; space > 0; space--)
        {
            if (space == 1)
                printf("  |--");
            else
                printf("  ");
        }
#if 0
        printf("Inode Num: %d rec len: %d name len: %u file type: %u file name: %s\n",
                pDirEntry->inode, pDirEntry->rec_len, pDirEntry->name_len,
                pDirEntry->file_type,
                pDirEntry->name);
#endif
        printf("%s%c (%d)\n",
                pDirEntry->name, Trail, pDirEntry->inode);
    }
}

/*! *************************************************************************
 * Function Name : InodeUtilDumpInode 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to print the ext3_inode structure
 *
 * Input         : pInode   -   pointer to the inode structure 
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID InodeUtilDumpInode(struct ext3_inode *pInode)
{
    UINT4 u4Index = 0;
    printf(" i_mode = %hu\n", pInode->i_mode);
    printf(" i_uid = %hu\n", pInode->i_uid);
    printf(" i_size = %u\n",pInode->i_size);
    printf(" i_atime = %u\n",pInode->i_atime);
    printf(" i_ctime = %u\n",pInode->i_ctime);
    printf(" i_mtime = %u\n",pInode->i_mtime);
    printf(" i_dtime = %u\n",pInode->i_dtime);
    printf(" i_gid = %hu\n",pInode->i_gid);
    printf(" i_links_count = %hu\n",pInode->i_links_count);
    printf(" i_blocks = %u\n",pInode->i_blocks);

    for (u4Index = 0; u4Index < EXT3_N_BLOCKS; u4Index++)
    {
        printf("    i_block[%d] = %u\n", u4Index, pInode->i_block[u4Index]);
    }
    printf(" i_flags = %u\n",pInode->i_flags);
    printf(" i_generation = %u\n",pInode->i_generation);
    printf(" i_file_acl = %u\n",pInode->i_file_acl);
    printf(" i_dir_acl = %u\n",pInode->i_dir_acl);
    printf(" i_faddr = %u\n",pInode->i_faddr);
    printf(" i_extra_isize = %hu\n",pInode->i_extra_isize);
    printf(" i_pad1 = %hu\n",pInode->i_pad1);
}

/*! *************************************************************************
 * Function Name : InodeUtilDumpDataBlockRaw 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to print the content of a block in HEX
 *
 * Input         : pBuffer  -   buffer containing the data block contents
 *
 * Output        : None
 * 
 * Returns       : None 
 *
 **************************************************************************/
VOID InodeUtilDumpDataBlockRaw(CHAR *pBuffer)
{
    UINT4 u4Index1 = 0;
    UINT4 u4Index2 = 0;
    for(u4Index1 = 0; u4Index1 < gu4BlockSize; u4Index1++)
    {   
        fprintf(stdout, " %02x", pBuffer[u4Index1]);
        u4Index2 += 1;
        if (u4Index2 == 16) 
        { 
            fprintf(stdout, "\n");
            u4Index2 = 0;
        }   
    }   
    /* fprintf(stdout, "\n\n"); */
}


/*! *************************************************************************
 * Function Name : InodeUtilGetInodeOffset 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to convert the given inode number into
 *                 byte offset
 *
 * Input         : u4InodeNo    -   Inode number
 *
 * Output        : pu8Offset    -   byte offset 
 * 
 * Returns       : INODE_SUCCESS, if the byte offset can be calculated 
 *                 INODE_FAILURE otherwise
 *
 **************************************************************************/
INT4 InodeUtilGetInodeOffset(UINT4 u4InodeNo, UINT8 *pu8Offset)
{
    struct ext3_inode Inode;
    struct ext3_group_desc GroupDes;
    UINT4 u4GroupNo;
    UINT4 u4LocalInodeIndex;
    INT4 i4RetVal;
    UINT8 u8GbdOffset;

    memset(&Inode, 0, sizeof(Inode));
    memset(&GroupDes, 0, sizeof(GroupDes));
    i4RetVal = 0;
    u8GbdOffset = 0;

    /* Find the block group no. corresponding to this inode */
    u4GroupNo = (u4InodeNo - 1) / sb.s_inodes_per_group; 

    /* Find the local inode index */
    u4LocalInodeIndex = (u4InodeNo - 1) % sb.s_inodes_per_group;

    /* 
       1. Seek and Read the block group descriptor for u4GroupNo
       2. Identify the inode table location from the group descriptor
    */
    u8GbdOffset = gu4BlockSize + u4GroupNo * sizeof(struct ext3_group_desc);
    i4RetVal = InodeUtilReadDataOffset(u8GbdOffset, &GroupDes, sizeof(struct ext3_group_desc));
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG2("ERROR: Failed to read Block group descriptor table %s:%d\n", __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    /* Inode table block no. is identified for this group no.
       Seek to this block and get offset */
    *pu8Offset = ((UINT8) GroupDes.bg_inode_table * gu4BlockSize) + (u4LocalInodeIndex *
           sb.s_inode_size); 

    /* printf("Inode no %d has offset %u\n", u4InodeNo, u8Offset); */
    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilCheckCorruptedInode
 *
 * Author        : Anusha Seshadri 
 *
 * Description   : This Function is used during manual corruption to check
 *                 whether the inode is already corrupted manually or not.
 *
 * Input         : u4InodeNo    -   Inode number
 *
 * Output        : None
 * 
 * Returns       : array index in the corrupted array, if the inode is corrupted 
 *                 INODE_FAILURE if it is not corrupted already
 *
 **************************************************************************/
INT4 InodeUtilCheckCorruptedInode(UINT4 u4InodeNo)
{   
    INT4 i4Len;
    UINT4 u4Index = 0;

    i4Len = sizeof(gu4CorruptedArray)/sizeof(gu4CorruptedArray[0]);

    for(u4Index=0; u4Index<i4Len; u4Index++)
    {
        if(gu4CorruptedArray[u4Index] == u4InodeNo)
        {
            return u4Index;
        }
    }
    return INODE_FAILURE;
}

/*! *************************************************************************
 * Function Name : InodeUtilCorruptAndRecover 
 *
 * Author        : Anusha Seshadri 
 *
 * Description   : This Function is used to manually corrupt and fix an inode.
 *
 * Input         : u4InodeNo    -   Inode to corrupt/fix
 *                 u4Cmd    -  INODE_CORRUPT  to corrupt
 *                             INODE_FIX to fix
 * Output        : None
 * 
 * Returns       : INODE_SUCCESS 
 *                 INODE_FAILURE
 *
 **************************************************************************/
INT4 InodeUtilCorruptAndRecover(UINT4 u4InodeNo, UINT4 u4Cmd)
{
    /*
       1. Get inode offset
       2. Read Inode
       3. Goto i_block[0] and flip bits say 1, 3 & 5
     */
    UINT4 u4iBlock0;
    UINT8 u8Offset = 0;
    INT4 i4RetVal;
    UINT4 u4Index = 0;

    InodeUtilGetInodeOffset(u4InodeNo, &u8Offset); /* offset for inode; */

    u8Offset = u8Offset + DIRECT_BLOCK_OFFSET;     /* offset for i_block[0] */

    i4RetVal = InodeUtilReadDataOffset(u8Offset, &u4iBlock0,
            sizeof(UINT4));
    if (i4RetVal == INODE_FAILURE)
    {
        DEBUG_LOG2("ERROR: Failed to read Block group descriptor table %s:%d\n", __FILE__, __LINE__);
        return INODE_FAILURE;
    }

    u4Index = InodeUtilCheckCorruptedInode(u4InodeNo);
    if((u4Index != INODE_FAILURE) &&
            (u4Cmd == INODE_CORRUPT))/* corruption intended, but already corrupted */
    {

        printf(" Inode already corrupted\n");
    }
    else if((u4Index == INODE_FAILURE) &&
            (u4Cmd == INODE_FIX))/* fix intended, but not corrupted already */
    {
        printf(" Nothing to fix\n");
    }
    else
    {
        /* flip bits on u4iBlock0 */
        u4iBlock0 = u4iBlock0 ^ CORR_FIX_MASK;  /* flipping bits 1,3,5 of LSB ie., 
                                          masking 00010101 to any of the address */
        /* write changes */
        i4RetVal = InodeUtilWriteDataOffset(u8Offset, &u4iBlock0, sizeof (UINT4));
        if(i4RetVal == SYS_CALL_FAILURE)
        {
            DEBUG_LOG2("ERROR: Failed to write back original data  %s:%d\n", __FILE__, __LINE__);
            return INODE_FAILURE;
        }

        if (u4Cmd == INODE_CORRUPT)
        {
            gu4CorruptedArray[++gi4ArrayIndex] = u4InodeNo;
        }
        else
        {
            gu4CorruptedArray[u4Index] = 0;
        }
    }
    return INODE_SUCCESS;
}

/*! *************************************************************************
 * Function Name : InodeUtilIsFreeInode 
 *
 * Author        : Naveen Raj Selvaraj
 *
 * Description   : This Function is used to check if an inode is all zero
 *
 * Input         : pInode    -   pointer to the inode structure
 *
 * Output        : None
 * 
 * Returns       : TRUE, if the inode is free
 *                 FALSE otherwise
 *
 **************************************************************************/
INT1 InodeUtilIsFreeInode(struct ext3_inode *pInode)
{
    /* XXX WARNING: This function used memcmp to compare struct.
       This is not a suggested way to compare structs, since padding bytes added
       to the struct are non-zero. Assuming that the 'struct ext3_inode' already
       contains 128 bytes (therefore no padding bytes would be added), the
       function was implemented this way. The better way is to compare every
       element of the 'struct ext3_inode' to 0. */
        
    struct ext3_inode DummyInode;
    memset(&DummyInode, 0, sizeof(DummyInode));

    if (memcmp(pInode, &DummyInode, sizeof(DummyInode)) == 0)
    {
        return TRUE;
    }
    return FALSE;
}

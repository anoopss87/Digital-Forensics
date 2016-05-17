/********************************************************************
 *
 * File         : inodeproto.h
 * Author       : Naveen Raj Selvaraj
 * Date created : March 18, 2016
 * Description  : This file contains all function prototypes 
 *
 *******************************************************************/

/*****************************************************************************/
/*                              inodeutil.c                                  */
/*****************************************************************************/
INT4 InodeUtilPrintChildren(UINT4 u4BlockNo, UINT4 u4Level);
INT4 InodeUtilPrintTree(UINT4 u4InodeNo, UINT4 u4Level);
VOID InodeUtilDumpDirEntry(struct ext3_dir_entry_2 *pDirEntry, tFileFilter Filter,
        UINT4 u4Level);
VOID InodeUtilDumpInode(struct ext3_inode *pInode);
VOID InodeUtilDumpDataBlockRaw(CHAR *pBuffer);
INT4 InodeUtilReadInode(UINT4 u4Inode, struct ext3_inode *pNewInode);
INT4 InodeUtilReadDataOffset(UINT8 u8Offset, VOID *pBuffer, UINT4 u4Size);
INT4 InodeUtilReadDataBlock(UINT4 u4BlockNo, UINT2 u2StartPos, VOID *pBuffer, UINT4 u4Size);
INT4 InodeUtilWriteDataOffset(UINT8 u8Offset, VOID *pBuffer, UINT4 u4Size);
INT4 InodeUtilWriteDataBlock(UINT4 u4BlockNo, UINT2 u2StartPos, VOID *pBuffer,
        UINT4 u4Size);
INT4 InodeUtilPrintDirDataBlock(UINT4 u4BlockNo);
INT4 InodeUtilPrintInode(UINT4 u4InodeNo);
INT4 InodeUtilGetInodeOffset(UINT4 u4InodeNo, UINT8 *u8Offset);
INT4 InodeUtilCheckCorruptedInode(UINT4 u4InodeNo);
INT4 InodeUtilCorruptAndRecover(UINT4 u4InodeNo, UINT4 u4Cmd);
INT1 InodeUtilIsFreeInode(struct ext3_inode *pInode);

/*****************************************************************************/
/*                              inodedir.c                                  */
/*****************************************************************************/
INT4 InodeDirReadRecord(CHAR *pEntries, UINT4 u4StartPos,
        struct ext3_dir_entry_2 *pDirEntry);

INT4 InodeDirVerifyDirRecord(CHAR *pEntries, struct ext3_dir_entry_2 *pDirEntry, 
                             UINT2 u4StartPos);
INT4 InodeDirValidateInode(UINT4 u4InodeNo, UINT4 u4BlockNo);
INT4 InodeDirValidateDataBlock(CHAR *pEntries);
INT4 InodeDirChangeParentEntry(struct ext3_dir_entry_2 *pDirEntry, UINT4 u4BlockNo);
INT4 InodeDirAddChildEntry(struct ext3_dir_entry_2 *pDirEntry, UINT4 u4BlockNo);

/*****************************************************************************/
/*                              inodefix.c                                  */
/*****************************************************************************/
INT4 InodeFixExecute();
INT4 InodeFixRootDataBlockPtr(struct ext3_inode *pInode);

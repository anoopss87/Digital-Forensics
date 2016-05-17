/********************************************************************
 *
 * File         : inodedef.h
 * Author       : Naveen Raj Selvaraj
 * Date created : March 17, 2016
 * Description  : This file contains all constants related         
 *                to inode functions
 *
 *******************************************************************/
#define BOOT_SECTOR_OFFSET 1024

#define DIR_NAME_PARENT ".."
#define DIR_NAME_CURRENT "."

#define DIR_CURR_ENTRY_OFFSET 0
#define DIR_PARENT_ENTRY_OFFSET 12

#define DIR_CURR_ENTRY_SIZE 12
#define DIR_PARENT_ENTRY_SIZE 12

#define DIR_ENTRY_NAME_OFFSET sizeof(UINT8)

#define INODE_SUCCESS 1
#define INODE_FAILURE -1

#define ROOT_INODE 2

#define REC_NAME_PREFIX "Recovery_"
#define MAX_BLOCK_SIZE 4096 /* should be declared in cmndef.h with correct value
                               */

#define DIR_CREATE(NewDirEntry, Inode, Name, FileType) NewDirEntry.inode = Inode; \
                                NewDirEntry.file_type = FileType; \
                strncpy(NewDirEntry.name, Name, strlen(Name)); \
                NewDirEntry.name_len = strlen(NewDirEntry.name); \
                NewDirEntry.rec_len = ceil((float)(DirEntry.name_len + DIR_ENTRY_NAME_OFFSET) / 4) * 4; 

#define DIR_REC_LEN(DirEntry) (ceil((float)(DirEntry.name_len + \
                            DIR_ENTRY_NAME_OFFSET) / 4) * 4); 

#define MAX_CORR_INODES 100

#define DIRECT_BLOCK_OFFSET 0x28  /* offset for i_block[0] from inode offset */

#define CORR_FIX_MASK 0x15  /* mask to corrupt and recover data : corrupting 1,3,5 of LSB */

#define BYTE 8

#define MAX_INODE_ARRAY_SIZE (ceil((float)gu4TotalInodes / BYTE))

/* Mask to set or clear bits in a byte */
#define MASK_BIT_0 0x80 /* MSB */
#define MASK_BIT_1 0x40
#define MASK_BIT_2 0x20
#define MASK_BIT_3 0x10
#define MASK_BIT_4 0x08
#define MASK_BIT_5 0x04
#define MASK_BIT_6 0x02
#define MASK_BIT_7 0x01 /* LSB */

#define GET_BLOCK_OFFSET_FROM_BYTE_OFFSET(Block, Position, ByteOffset) \
                            Block = (UINT4)(ByteOffset / gu4BlockSize); \
                            Position = (UINT2)(ByteOffset % gu4BlockSize);

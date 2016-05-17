/* Digital Forensics - Spring 2016 
 * 
 * GBD.h - structures and functions are declared in this file.
 * @author - Anoop S Somashekar
*/

#include <sys/stat.h>
#include <errno.h>
#include <linux/hdreg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

typedef signed char _s8;
typedef unsigned char _u8;
typedef unsigned char byte;

typedef signed short _s16;
typedef unsigned short _u16;

typedef signed int _s32;
typedef unsigned int _u32;

typedef long long _s64;
typedef unsigned long long _u64;

#define SUPER_BLOCK_OFFSET 1024
#define SUPER_BLOCK_SIZE 1024
#define BLOCK_SIZE_VALUE_OFFSET 24
#define NUM_OF_BLOCKS_IN_A_GROUP_REL_OFFSET 24
#define NUM_OF_INODES_IN_A_GROUP_REL_OFFSET 4
#define MIN_BLOCK_SIZE 1024
#define BLOCK_GROUP_DESC_SIZE 32
#define EXT3_N_BLOCKS 15
#define DEFAULT_EXT3_INODE_SIZE 256
#define BLOCK_GROUP_SIZE 32
#define HEX_DUMP_LINE_SIZE 16
#define MAX_FILENAME_LENGTH 25
#define DIRECT_BLOCKS_COUNT 12
#define BLOCK_GROUP_INFO_MAX_SIZE 60
#define BLOCK_GROUP_FIELD_MAX_SIZE 12
#define SINGLE_INDIRECT_BLOCKS_COUNT 1024
#define DOUBLE_INDIRECT_BLOCKS_COUNT 1024*1024
#define BITS_IN_A_BYTE 8

#define USED_BLOCK 1
#define NULL_BLOCK 2
#define ADDR_BLOCK 3
#define TEXT_BLOCK 4
#define UNUSED_BLOCK 5
#define DIR_BLOCK 6

#define LAST_ASCII_HEX 0x7E

//global variables
int gBlockSize;
int gBlockGroupCount;
struct ext3_group_desc *gGrpDescTable;
//unsigned char *colorCodeBitmap;

/*
 * Structure of a blocks group descriptor
*/
struct ext3_group_desc
{
	_u32	bg_block_bitmap;		/* Blocks bitmap block */
	_u32	bg_inode_bitmap;		/* Inodes bitmap block */
	_u32	bg_inode_table;		/* Inodes table block */
	_u16	bg_free_blocks_count;	/* Free blocks count */
	_u16	bg_free_inodes_count;	/* Free inodes count */
	_u16	bg_used_dirs_count;	/* Directories count */
	_u16	bg_pad;
	_u32	bg_reserved[3];
};

struct ext3_dir_entry_2 
{
	__le32 inode;		/* Inode number */
	__le16 rec_len;	/* Directory entry length */
	__u8 name_len; /* Name length */
	__u8 file_type;	/* File Type */
	char name[255]; /* File name */
};


struct ext3_inode {
	__le16	i_mode;		/* File mode */
	__le16	i_uid;		/* Low 16 bits of Owner Uid */
	__le32	i_size;		/* Size in bytes */
	__le32	i_atime;	/* Access time */
	__le32	i_ctime;	/* Creation time */
	__le32	i_mtime;	/* Modification time */
	__le32	i_dtime;	/* Deletion Time */
	__le16	i_gid;		/* Low 16 bits of Group Id */
	__le16	i_links_count;	/* Links count */
	__le32	i_blocks;	/* Blocks count */
	__le32	i_flags;	/* File flags */
	union {
		struct {
			__u32  l_i_reserved1;
		} linux1;
		struct {
			__u32  h_i_translator;
		} hurd1;
		struct {
			__u32  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	__le32	i_block[EXT3_N_BLOCKS];/* Pointers to blocks */
	__le32	i_generation;	/* File version (for NFS) */
	__le32	i_file_acl;	/* File ACL */
	__le32	i_dir_acl;	/* Directory ACL */
	__le32	i_faddr;	/* Fragment address */
	union {
		struct {
			__u8	l_i_frag;	/* Fragment number */
			__u8	l_i_fsize;	/* Fragment size */
			__u16	i_pad1;
			__le16	l_i_uid_high;	/* these 2 fields    */
			__le16	l_i_gid_high;	/* were reserved2[0] */
			__u32	l_i_reserved2;
		} linux2;
		struct {
			__u8	h_i_frag;	/* Fragment number */
			__u8	h_i_fsize;	/* Fragment size */
			__u16	h_i_mode_high;
			__u16	h_i_uid_high;
			__u16	h_i_gid_high;
			__u32	h_i_author;
		} hurd2;
		struct {
			__u8	m_i_frag;	/* Fragment number */
			__u8	m_i_fsize;	/* Fragment size */
			__u16	m_pad1;
			__u32	m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */
	__le16	i_extra_isize;
	__le16	i_pad1;
};

/* Function Declarations */
char* bgdCompareGrpDesc(int blkGrpNum1, int blkGrpNum2, char driveName[]);
struct ext3_group_desc * bgdGetGrpDescTable(char driveName[], int blockGroupNo, int fileWrite);
void bgdReadSingleIndirectBlocks(int blockNumber, FILE *fDataOutput, int fp, int fileSizeInBlocks, FILE *fBlockAddr, FILE *fBin);
void bgdReadDoubleIndirectBlocks(int blockNumber, FILE *fDataOutput, int fp, int fileSizeInBlocks, FILE *fBlockAddr, FILE *fBin);
void bgdReadTripleIndirectBlocks(int blockNumber, FILE *fDataOutput, int fp, int fileSizeInBlocks, FILE *fBlockAddr, FILE *fBin);
void bgdReadFromInode(int inode, char dName[]);
char *bgdGetBlockGroupInfo(int blockGroupNum);

int bgdMax(int a, int b);
int bgdGetBlockSize(char driveName[]);
int bgdGetNumberofBlockGroups(char driveName[]);
int bgdIsPowerOf3_5_7(int number);
long long bgdGetGroupDescStartOffset(int index);
unsigned char* bgdReadBlockBitmap(char driveName[], int blockNo);
void bgdUpdateNullBlock(int fd, int blockGroupNo, char *colorCodeBitmap);
void bgdUpdateAddrBlock(int fd, int blockGroupNo, char *colorCodeBitmap);
void bgdUpdateTextBlock(int fd, int blockGroupNo, char *colorCodeBitmap);
unsigned long getBlockBitMapAddr(int blockGroupNo);

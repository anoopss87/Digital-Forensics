#include "cmninc.h"
#include "test.h"
#define BYTE_CHUNK 		2048
#define BLKSZ_1K		1024	// block size 1024
#define BLKSZ_2K		2048	// block size 2048
#define BLKSZ_4K		4096	// block size 4096
#define LESS_ONE		1023	// 1K - 1
#define BLKSZ_BYTES_PFX		0x00	// prefix bytes for block size bytes in superblock
#define BLKSZ_BYTES_1K		0x00	// bytes representing block size 1024
#define BLKSZ_BYTES_2K		0x01	// bytes representing block size 2048
#define BLKSZ_BYTES_4K		0x02	// bytes representing block size 4096
#define MAGIC_NUMBER		0xef53
#define SB_OFFSET		1024	// superblock offset from beginning of filesystem
#define MAG_NUM_OFFSET		1080	// magic number offset from beginning of filesystem
#define BLKS_GRP_OFFSET		1056   	// blocks per group
#define BLK_SZ_OFFSET		1048	// block size offset from beginning of filesystem
#define CORRUPT_BYTE		0xff	// used for magic number corrupt test
#define SB_ARR_SZ		5	// number of superblocks in array
#define MAG_NUM_BYTE		56	// used for magic number corrupt test
/* global variables */
INT4 gBlockSize;
INT4 gReservedGDTBlocks;
INT4 gInodeTableSize;
INT4 gNumBlockGroups;
INT4 gBlocksPerGroup;
INT8 gFilesystemSize;
INT8 gTotalBlocks;
tSuperblock gPrimarySuperblock;
tSuperblock gFiveSuperblockArray[5];

void sbCorruptSuperblock()
{
	printf("/Corrupting...\n");
	FILE *pFile;
	pFile = fopen("/dev/sdb1", "rb+");
	UCHAR ucCorruptByte = CORRUPT_BYTE;
	INT2 i2RetVal1, i2RetVal2;
	i2RetVal1 = fseek(pFile, MAG_NUM_OFFSET, SEEK_SET);
	if(i2RetVal1 != 0)
	{
		if (ferror(pFile))
		{
			perror("fseek()");
			fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
			exit(EXIT_FAILURE);
		}
	}
	i2RetVal2 = fwrite(&ucCorruptByte, 1, 1, pFile);
	if (i2RetVal2 < 1)
	{
		if (ferror(pFile))
		{
			perror("fwrite()");
			fprintf(stderr,"fwrite() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
			exit(EXIT_FAILURE);
		}
	}
	fclose(pFile);
}

void sbFixSuperblock()
{
	printf("/Fixing...\n");
	FILE *pFile;
	pFile = fopen("/dev/sdb1", "rb+");
	UCHAR *pucMagicNumBytes = MAGIC_NUMBER;
	INT2 i2RetVal1, i2RetVal2;
	i2RetVal1 = fseek(pFile, MAG_NUM_OFFSET, SEEK_SET);
	if(i2RetVal1 != 0)
	{
		if (ferror(pFile))
		{
			perror("fseek()");
			fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
			exit(EXIT_FAILURE);
		}
	}
	i2RetVal2 = fwrite(&pucMagicNumBytes, 2, 1, pFile);
	if (i2RetVal2 < 1)
	{
		if (ferror(pFile))
		{
			perror("fwrite()");
			fprintf(stderr,"fwrite() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
			exit(EXIT_FAILURE);
		}
	}
	fclose(pFile);
}

#ifdef UNUSED
/*
 * This is experimental code intended to afford the user controlled
 * corruption of filesystem. The code only corrupts a single byte which
 * could either make the superblock unreadable (as in the case of a
 * corrupt magic number) or it would cause the superblock to return
 * garbage data. Possible revisions to the code could include allowing
 * user to specify which superblock field to corrupt.
 */
void sbCorruptSuperblock()
{
	INT2 i2RetVal1;
	CHAR acUserInput[10];
	printf("\nEnter filesystem to corrupt: ");
	scanf(" %s", &acUserInput); // scan in user selection
	FILE *pFile;
	pFile = fopen(&acUserInput, "rb+");

	if (pFile == NULL)
	{
		printf("Error opening filesystem: %s\n", strerror(errno));
		printf("Try again\n");
		fix_test();
	}
	UCHAR ucCorruptedByte = CORRUPT_BYTE;
	i2RetVal1 = fseek(pFile, MAG_NUM_OFFSET, SEEK_SET);   	// offsets have been defined above
	if(i2RetVal1 != 0)
	{
		if (ferror(pFile))
		{
			perror("fseek()");
			fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
			exit(EXIT_FAILURE);
		}
	}
	fputc(ucCorruptedByte, pFile);
	printf("filesystem corrupted!\n");
	fclose(pFile);
}

void sbFixSuperblock()
{
	INT2 i2RetVal1;
	CHAR acUserInput[10];
	printf("\nEnter filesystem to fix: ");
	scanf(" %s", &acUserInput); // scan in user selection
	FILE *pFile;
	pFile = fopen(&acUserInput, "rb+");

	if (pFile == NULL)
	{
		printf("Error opening filesystem: %s\n", strerror(errno));
		printf("Try again\n");
		fix_test();
	}
	UCHAR ucMagicNumByte = 0x53; // this left undefined bc it is variable based
							// based on type of corruption
	i2RetVal1 = fseek(pFile, MAG_NUM_OFFSET, SEEK_SET);
	if(i2RetVal1 != 0)
	{
		if (ferror(pFile))
		{
			perror("fseek()");
			fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
			exit(EXIT_FAILURE);
		}
	}
	fputc(ucMagicNumByte, pFile);
	printf("filesystem fixed!\n");
	fclose(pFile);
}
#endif

void sbSwap (INT8 *i8Var1, INT8 *i8Var2)
{
	INT8 i8Temp = *i8Var1;
	*i8Var1 = *i8Var2;
	*i8Var2 = i8Temp;
}

void sbPrintSuperblocks(tSuperblock *pSuperblock);
int  sbGetBlockSize(FILE *pFile);
void *sbFindValidSuperblock(tSuperblock *pSuperblock, INT2 *ai2SuperblockArray);
int  sbCheckValidSuperblock(tSuperblock *pSuperblock);
void sbValidateSuperblocks(FILE *pFile);
void sbGetPrimarySuperblock(FILE *pFile);
void sbAutoRetrieve(FILE *pFile);
void sbManualRetrieve(FILE *pFile);
void sbRepairSuperblocks(tSuperblock *pSuperblockBad, tSuperblock *pSuperblockGood);

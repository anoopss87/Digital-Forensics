#include "superblock.h"
/*
 * the "ai8SuperBlockGrpNums" array represent exponential values of 3, 5, 7
 * which can be used to calculate superblock offsets. This version
 * would allow retrieval of all superblocks in a 1TB filesystem. In
 * the final version of the program, this need not be a constant; actual
 * values could be computed at runtime.
 */
INT8 ai8SuperBlockGrpNums[43] = {
	1, 3, 5, 7, 9, 25, 27, 49, 81, 125, 243, 343,
	625, 729, 2187, 2401, 3125, 6561, 15625, 16807,
	19683, 59049, 78125, 117649, 177147, 390625, 531441,
	823543, 1594323, 1953125, 5764801, 4782969, 9765625,
	14398907, 40353607, 43046721, 48828125, 129140163,
	244140625, 282475249, 387420489, 1162261467, 1220703125
};

int init(FILE *pFile)
{
	gBlockSize = sbGetBlockSize(pFile);
	if(gBlockSize == -1)
	{
		printf("block size error\n");
		return 0;
	}
	gBlocksPerGroup = gBlockSize * 8;
	printf("Block size: %d %d\n", gBlockSize, gBlocksPerGroup);
	sbGetPrimarySuperblock(pFile);
	if(sbCheckValidSuperblock(&gPrimarySuperblock) == 1) {
		printf("Valid filesystem\n");
		return 1;
	} else {
		printf("Not a valid filesystem\n");
		return 0;
	}
//	sbValidateSuperblocks(pFile);
}

/*
 * This function takes an array of superblocks and returns an int array
 * where "1" in the int array means the superblock in the corresponding
 * position in the superblock array is "likely" valid. A superblock is
 * deemed "likely" valid if its magic number, blocks per group, and block size
 * fields match expected values.
 */
void *sbFindValidSuperblock(tSuperblock *superblock, INT2 *ai2SuperblockArray){
	ai2SuperblockArray[0] = 0;
	INT2 i2Index1;

	for(i2Index1 = 0; i2Index1 < SB_ARR_SZ; i2Index1++) {
		// compare magic number
		if(superblock[i2Index1].s_magic == MAGIC_NUMBER) {
			// compare blocks per group
			if(superblock[i2Index1].s_blocks_per_group == gBlocksPerGroup) {
				// compare block size
				if ((BLKSZ_1K << superblock[i2Index1].s_log_block_size) == gBlockSize)
					ai2SuperblockArray[i2Index1] = 1;
			}
		} else
			ai2SuperblockArray[i2Index1] = 0;
	}

	// print array representing valid superblocks
	INT2 i2Index2;
	printf("sb_array: = { ");
	for (i2Index2=0; i2Index2 < SB_ARR_SZ; i2Index2++) {
		printf(" %d", ai2SuperblockArray[i2Index2]);
	}
	printf(" }\n");

	return ai2SuperblockArray;
}
/*
 * Master Boot Record may have incorrect filesystem id. This function
 * essentially peforms sbFindValidSuperblock on the primary superblock
 * to determine if filesystem is in fact ext
 */
sbCheckValidSuperblock(tSuperblock *pSuperblock) {
	if (pSuperblock->s_magic == MAGIC_NUMBER) {
		if(pSuperblock->s_blocks_per_group == gBlocksPerGroup) {
			if((BLKSZ_1K << pSuperblock->s_log_block_size) == gBlockSize)
				return 1;
		}
	} else
		return 0;
}

/*
 * this function can be used for testing purposes. It allows user to
 * corrupt and repair superblocks as well choose whether or not to
 * randomly or manually select superblocks to compare.
 */
#ifdef NOT_USED
void sbValidateSuperblocks(FILE *pFile) {
	CHAR UserInput;
	INT2 i2Index1;
	// test menu
	printf("Superblock Validation\n");
	printf("Enter option to compare superblocks: \n");
	printf("\t1: get primary superblock\n");
	printf("\t2: auto-retrieve\n");
	printf("\t3: manual retrieve\n");
	printf("\t4: corrupt filesystem\n");
	printf("\t5: fix filesystem\n");
	printf("\t6: print meta data info\n");
	printf("\t7: exit\n");

// option for selecting superblocks
	i2Index1 = scanf(" %c", &UserInput); // scan in user selection
	if(i2Index1 != 1) {
		printf("Error (scanf) %s\n", strerror(errno));
	}
	switch (UserInput)	{
		case '1' :
			sbGetPrimarySuperblock(pFile);
			sbValidateSuperblocks(pFile);
			break;
		case '2' :
			sbAutoRetrieve(pFile);
			break;
		case '3' :
			sbManualRetrieve(pFile);
			break;
		case '4' :
			sbCorruptSuperblock(); // corrupts primary block magic number
			break;
		case '5' :
			sbFixSuperblock();
			break;
		case '6' :
			printf("Total block groups: %d\n", gNumBlockGroups);
			printf("Reserved GDT Blocks: %d\n", gReservedGDTBlocks);
			printf("Inode table size: %d\n", gInodeTableSize);
			break;
		case '7' :
			printf("\tGoodbye\n");
			break;
		default :
			printf("Invalid option. Try again\n");
			sbValidateSuperblocks(pFile); // loop until user chooses valid option
	}
}
#endif
/*
 * This function will retrieve the primary superblock
 */

void sbGetPrimarySuperblock(FILE *pFile) {
	tSuperblock primarySB;
	INT2 i2RetVal1, i2RetVal2;
	i2RetVal1 = fseek(pFile, SB_OFFSET, SEEK_SET);
	if(i2RetVal1 != 0) {
		if (ferror(pFile)) {
			perror("fseek()");
			fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
			exit(EXIT_FAILURE);
		}
	}
	i2RetVal2 = fread(&primarySB, sizeof(tSuperblock), 1, pFile);
	if(i2RetVal2 != 1)
		if(ferror(pFile))
			printf("error reading filesystem\n");
	rewind(pFile);
	// set global for total number of block groups
	gNumBlockGroups = primarySB.s_blocks_count/gBlocksPerGroup;
	printf("numBlockGroups: %d\n", gNumBlockGroups);
	// set global for the size of the inode table (in blocks)
	gInodeTableSize = primarySB.s_inodes_per_group/(gBlockSize/primarySB.s_inode_size);
	printf("inodeTableSz: %d\n", gInodeTableSize);
	// set global for number of blocks reserved for group descriptors
	gReservedGDTBlocks = primarySB.s_reserved_gdt_blocks;
	printf("reserved block group descriptor blocks: %d\n", gReservedGDTBlocks);
	// set global for total blocks
	gTotalBlocks = primarySB.s_blocks_count;
	printf("total blocks: %d\n", gTotalBlocks);
	// set global for file system size
	gFilesystemSize = gTotalBlocks * gBlockSize;
	printf("filesystem size: %lu\n", gFilesystemSize);
	// display some superblock fields, used for testing
#ifdef TEST_CODE
	printf("Inode size: %d\n", sb->s_inode_size);
	printf("Block group number of superblock: %lld\n", sb->s_block_group_nr);
	printf("Magic Number: 0x%04X\n", sb->s_magic);
	printf("Blocks: %d\n", sb->s_blocks_count);
	printf("Blocks_per_Group: %u\n", sb->s_blocks_per_group);
	printf("Inodes: %d\n", sb->s_inodes_count);
	printf("Inodes per group %d\n", sb->s_inodes_per_group);
	printf("Reserved blocks for GDT: %d\n", sb->s_reserved_gdt_blocks);
	printf("Inode size: %d\n", sb->s_inode_size);
#endif
	// set global for primary superblock
	gPrimarySuperblock = primarySB;
}

int getBlocksPerGroup() {
	return gBlocksPerGroup;
}

long getPartitionSize() {
	return gFilesystemSize;
}

long getInodeTableSize() {
	return gInodeTableSize;
}

long getReservedGDTBlocks() {
	return gReservedGDTBlocks;
}

int getSingleBlockSize() {
	return gBlockSize;
}

/*
 * this function randomly chooses superblocks to compare
 */
void sbAutoRetrieve(FILE *pFile) {
	tSuperblock atSuperblocks[SB_ARR_SZ];
	INT8 ai8SuperblockOffsets[SB_ARR_SZ];
	ai8SuperblockOffsets[0] = 0;
	INT2 i2Index1, i2Index2, i2Index3, i2Index4, i2Index5, i2Index6, i2Index7;
	INT2 i2RetVal1, i2RetVal2;
	/* random number generator */
	time_t t;
	srand((unsigned) time(&t));
	/* shuffle ai8SuperBlockGrpNums */
	for(i2Index1 = 7; i2Index1 > 1; i2Index1--) {
		i2Index2 = rand()%(i2Index1 + 1);
		sbSwap(&ai8SuperBlockGrpNums[i2Index1], &ai8SuperBlockGrpNums[i2Index2]);
	}

	for(i2Index3 = 1; i2Index3 < 5; i2Index3++) {
		if(gBlockSize == BLKSZ_1K)
			ai8SuperblockOffsets[i2Index3] = (pow(gBlockSize, 2) * 8 * ai8SuperBlockGrpNums[i2Index3 - 1]) + 1;
		else
			ai8SuperblockOffsets[i2Index3] = pow(gBlockSize, 2) * 8 * ai8SuperBlockGrpNums[i2Index3-1];
	}
	for (i2Index4 = 0; i2Index4 < SB_ARR_SZ; i2Index4++) {
		if (i2Index4 == 0) { /* the first superblock (block0) is always at offset 1024 */
			i2RetVal1 = fseek(pFile, ai8SuperblockOffsets[i2Index4] + SB_OFFSET, SEEK_SET);
			if(i2RetVal1 != 0) {
				if (ferror(pFile))	{
					perror("fseek()");
					fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
					exit(EXIT_FAILURE);
				}
				i2RetVal2 = fread(&atSuperblocks[i2Index4], sizeof(tSuperblock), 1, pFile);
				if(i2RetVal2 != 1)
					if(ferror(pFile))
						printf("error reading filesystem\n");
				rewind(pFile);
			}
		} else { /* this finds location of all other superblocks */
			if (gBlockSize == BLKSZ_1K) {
				i2RetVal1 = fseek(pFile, ai8SuperblockOffsets[i2Index4] + LESS_ONE, SEEK_SET);
				if(i2RetVal1 != 0) {
					if(ferror(pFile)) {
						perror("fseek()");
						fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
						exit(EXIT_FAILURE);
					}
				}
			} else {
				i2RetVal1 = fseek(pFile, ai8SuperblockOffsets[i2Index4], SEEK_SET);
				if(i2RetVal1 != 0) {
					if(ferror(pFile)) {
						perror("fseek()");
						fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
						exit(EXIT_FAILURE);
					}
				}
			}
			i2RetVal2 = fread(&atSuperblocks[i2Index4], sizeof(tSuperblock), 1, pFile);
			if(i2RetVal2 != 1)
				if(ferror(pFile))
					printf("error reading filesystem\n");
			rewind(pFile);
		}
	}
	/*
	 * Save array of 5 randomly chosen superblocks. Possible uses of
	 * the superblocks include repairing corrupted superblocks and using
	 * superblock from some block group n as the primary superblock.
	 *
	 */
	for(i2Index5 = 0; i2Index5 < SB_ARR_SZ; i2Index5++)
	{
		gFiveSuperblockArray[i2Index5] = atSuperblocks[i2Index5];
	}
	// print retrieved superblocks
	sbPrintSuperblocks(atSuperblocks);
	INT2 *ai2ValidSuperblocks = malloc(sizeof(INT2) * SB_ARR_SZ);
	ai2ValidSuperblocks = sbFindValidSuperblock(atSuperblocks, ai2ValidSuperblocks);
	for(i2Index6 = 0; i2Index6 < SB_ARR_SZ; i2Index6++)
	{
		if(ai2ValidSuperblocks[i2Index6] == 0)
		{
			for(i2Index7 = 0; i2Index7 < SB_ARR_SZ; i2Index7++)
			{
				if(ai2ValidSuperblocks[i2Index7] == 1)
				{
					sbRepairSuperblocks (&atSuperblocks[i2Index6], &atSuperblocks[i2Index7]);
					break;
				}
				else if (i2Index7 == SB_ARR_SZ)
					printf("No valid superblocks...\n");
			}
		}
		else
			continue;
	}
	free(ai2ValidSuperblocks);
}
/*
 * give user option to choose which superblocks to compare
 */
void sbManualRetrieve(FILE *pFile) {
	tSuperblock atSuperblocks[SB_ARR_SZ];
	INT8 ai8SuperblockOffsets[SB_ARR_SZ];
	ai8SuperblockOffsets[0] = 0;
	INT8 i8BlockGroupNumber[4];
	INT2 i2Index1, i2Index2, i2Index3, i2Index4, i2Index5;
	INT2 i2RetVal1, i2RetVal2;
	/*
	 * Here I would like to list the possible exponential values from which
	 * a user may manually select a block group. In order to display the correct
	 * values the program would first determine the size of the filesystem as well
	 * as the blocksize. Subsequent iterations should consider the utility of listing
	 * possible block group numbers as opposed to leaving it up to the user
	 */
	printf("Enter 4 superblocks: ");
	scanf("%lld %lld %lld %lld", &i8BlockGroupNumber[0], &i8BlockGroupNumber[1], &i8BlockGroupNumber[2], &i8BlockGroupNumber[3]);
	for(i2Index1 = 1; i2Index1 < 5; i2Index1++) {
		if(gBlockSize == BLKSZ_1K)
			ai8SuperblockOffsets[i2Index1] = (pow(gBlockSize, 2) * 8 * i8BlockGroupNumber[i2Index1 - 1]) + 1;
		else
			ai8SuperblockOffsets[i2Index1] = pow(gBlockSize, 2) * 8 * i8BlockGroupNumber[i2Index1 - 1];
	}
	printf("\n");
	for(i2Index2 = 0; i2Index2 < SB_ARR_SZ; i2Index2++) {
		if(i2Index2 == 0) /* the first superblock (block0) is always at offset 1024 */ {
			i2RetVal1 = fseek(pFile, ai8SuperblockOffsets[i2Index2] + SB_OFFSET, SEEK_SET);
			if(i2RetVal1 != 0) {
				if(ferror(pFile)) {
					perror("fseek()");
					fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
					exit(EXIT_FAILURE);
				}
			}
			i2RetVal2 = fread(&atSuperblocks[i2Index2], sizeof(tSuperblock), 1, pFile);
			if(i2RetVal2 != 1)
				if(ferror(pFile))
					printf("error reading filesystem\n");
			rewind(pFile);
		} else { /* this finds location of all other superblocks */
			if(gBlockSize == BLKSZ_1K) {
				i2RetVal1 = fseek(pFile, ai8SuperblockOffsets[i2Index2] + LESS_ONE, SEEK_SET);
				if(i2RetVal1 != 0) {
					if(ferror(pFile)) {
						perror("fseek()");
						fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
						exit(EXIT_FAILURE);
					}
				}
			} else {
				i2RetVal1 = fseek(pFile, ai8SuperblockOffsets[i2Index2], SEEK_SET);
				if(i2RetVal1 != 0) {
					if(ferror(pFile)) {
						perror("fseek()");
						fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
						exit(EXIT_FAILURE);
					}
				}
			}
			i2RetVal2 = fread(&atSuperblocks[i2Index2], sizeof(tSuperblock), 1, pFile);
			if(i2RetVal2 != 1)
				if(ferror(pFile))
					printf("error reading filesystem\n");
			rewind(pFile);
		}
	}
	for(i2Index3 = 0; i2Index3 < SB_ARR_SZ; i2Index3++) {
		gFiveSuperblockArray[i2Index3] = atSuperblocks[i2Index3];
	}
	// print superblocks
	sbPrintSuperblocks(atSuperblocks);
	INT2 *ai2ValidSuperblocks = malloc(sizeof(int) * SB_ARR_SZ);
	ai2ValidSuperblocks = sbFindValidSuperblock(atSuperblocks, ai2ValidSuperblocks);
	for(i2Index4 = 0; i2Index4 < SB_ARR_SZ; i2Index4++) {
		if(ai2ValidSuperblocks[i2Index4] == 0) {
			for(i2Index5 = 0; i2Index5 < SB_ARR_SZ; i2Index5++) {
				if(ai2ValidSuperblocks[i2Index5] == 1) {
					sbRepairSuperblocks (&atSuperblocks[i2Index4], &atSuperblocks[i2Index5]);
					break;
				} else if(i2Index5 == SB_ARR_SZ) {
					printf("No valid superblocks...\n");
				}
			}
		} else
			continue;
	}
	free(ai2ValidSuperblocks);
}
/*
 * this function was used to test repairing a corrupt magic number. This function
 * repairs the single byte corrupted by corrupt().
 */
void sbRepairSuperblocks(tSuperblock *pSuperblockBad, tSuperblock *pSuperblockGood) {
	INT2 i2BlockGrpNum;
	INT2 i2RetVal1, i2RetVal2;
	INT8 i8ByteOffset = 0;
	FILE *pFile;
	pFile = fopen("/dev/sdb1", "rb+");
	UCHAR *pMagicNumber = MAGIC_NUMBER;

	if(pSuperblockBad->s_magic != pSuperblockGood->s_magic) {
		printf("repairing magic number now...\n");
		i2BlockGrpNum = pSuperblockBad->s_block_group_nr;
		if(i2BlockGrpNum == 0) {
			i2RetVal1 = fseek(pFile, MAG_NUM_OFFSET, SEEK_SET);
			if(i2RetVal1 != 0) {
				if(ferror(pFile)) {
					perror("fseek()");
					fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
					exit(EXIT_FAILURE);
				}
			}
			i2RetVal2 = fwrite(&pMagicNumber, 2, 1, pFile);
			if(i2RetVal2 < 1) {
				if(ferror(pFile)) {
					perror("fwrite()");
					fprintf(stderr,"fwrite() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
					exit(EXIT_FAILURE);
				}
			}
			fclose(pFile);
		} else {
			i8ByteOffset = (pSuperblockBad->s_block_group_nr * gBlocksPerGroup * gBlockSize) + MAG_NUM_BYTE;
			i2RetVal1 = fseek(pFile, i8ByteOffset, SEEK_SET);
			if(i2RetVal1 != 0) {
				if(ferror(pFile)) {
					perror("fseek()");
					fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
					exit(EXIT_FAILURE);
				}
			}
			i2RetVal2 = fwrite(&pMagicNumber, 2, 1, pFile);
			if(i2RetVal2 < 1) {
				if(ferror(pFile))
				{
					perror("fwrite()");
					fprintf(stderr,"fwrite() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
					exit(EXIT_FAILURE);
				}
			}
			fclose(pFile);
		}
	} else
		printf("it's all good\n");
}
/*
 * this function retrieves the block size and, in doing so,
 * partially validates the superblock.
 */
sbGetBlockSize(FILE *pFile) {
	UCHAR aucBlockSizeBytes[4];
	INT2 i2BlockSize;
	INT2 i2RetVal1, i2RetVal2;
	i2RetVal1 = fseek(pFile, BLK_SZ_OFFSET, SEEK_SET);
	if(i2RetVal1 != 0) {
		if(ferror(pFile)) {
			perror("fseek()");
			fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
			exit(EXIT_FAILURE);
		}
	}
	i2RetVal2 = fread(aucBlockSizeBytes, 1, 2, pFile);
	if(i2RetVal2 != 1)
		if(ferror(pFile))
			printf("error reading filesystem\n");
	if(aucBlockSizeBytes[0] == BLKSZ_BYTES_1K && ((aucBlockSizeBytes[1] ==  	// 1K block size
			aucBlockSizeBytes[2]) == (aucBlockSizeBytes[3] == BLKSZ_BYTES_PFX))) {
		i2BlockSize = BLKSZ_1K;
		return i2BlockSize;
	}
	if(aucBlockSizeBytes[0] == BLKSZ_BYTES_2K && ((aucBlockSizeBytes[1] == 		// 2K block size
			aucBlockSizeBytes[2]) == (aucBlockSizeBytes[3] == BLKSZ_BYTES_PFX))) {
		i2BlockSize = BLKSZ_2K;
		return i2BlockSize;
	}
	if(aucBlockSizeBytes[0] == BLKSZ_BYTES_4K && ((aucBlockSizeBytes[1] == 		// 4K block size
			aucBlockSizeBytes[2]) == (aucBlockSizeBytes[3] == BLKSZ_BYTES_PFX))) {
		i2BlockSize = BLKSZ_4K;
		return i2BlockSize;
	} else {
		i2BlockSize = -1;
		return i2BlockSize;
	}
}
/*
 * function to print superblock info from array
 * of five superblocks
 */
void sbPrintSuperblocks(tSuperblock *pSuperblock) {
	INT2 i2Index1, i2Index2;
	for(i2Index1 = 0; i2Index1 < SB_ARR_SZ; i2Index1++) {
		printf("SUPERBLOCK\n");
		printf("UUID: ");
		for(i2Index2 = 0; i2Index2 < sizeof(pSuperblock->s_uuid); i2Index2++) {
			printf("%02x", pSuperblock->s_uuid[i2Index2]);
		}
		printf("\n");
		printf("Inode size: %d\n", pSuperblock->s_inode_size);
		printf("Block group number of superblock: %lld\n", pSuperblock->s_block_group_nr);
		printf("Magic Number: 0x%04X\n", pSuperblock->s_magic);
		printf("Blocks: %d\n", pSuperblock->s_blocks_count);
		printf("Blocks_per_Group: %u\n", pSuperblock->s_blocks_per_group);
		printf("Inodes: %d\n", pSuperblock->s_inodes_count);
		printf("Inodes per group %d\n", pSuperblock->s_inodes_per_group);
		printf("Reserved blocks for GDT: %d\n", pSuperblock->s_reserved_gdt_blocks);
		printf("Inode size: %d\n", pSuperblock->s_inode_size);
		printf("\n");
	}
}

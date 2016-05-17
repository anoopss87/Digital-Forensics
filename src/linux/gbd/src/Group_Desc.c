/* Digital Forensics - Spring 2016 
 * 
 * GBD.c - writes the group descriptor table data into the output file(hex dump as well as key:value).
 * compares group descriptor table of 2 different block group number.
 * @author - Anoop S Somashekar
*/

#include "Group_Desc.h"
extern struct ext3_group_desc *gGrpDescTable;

/* returns the maximum element from 2 numbers */ 
int bgdMax(int a, int b)
{
    if(a > b)
      return a;
    else
      return b;  
}

/*Group descriptor will be duplicated at 0,1 and powers of 3, 5, 7
  For e.g it is duplicated at block group number 0, 1, 3, 5, 7, 9, 25, 27, 49, 81, 125 etc....*/
long long bgdGetGroupDescStartOffset(int index)
{
    /* blockSize * 8 gives the number of blocks in a block group index is the block group number
     * from which we need the block group descriptor table since first block of any block group
     * is super block, 1 is added to get the starting offset for group desc table (index * blockSize * 8 + 1)
     * is the block number. Since we need byte offset to read the data, it is multiplied by blocksize.*/
    if(index == 0) {
        if(gBlockSize == 1024) {
            return 2*gBlockSize;
        } else {
            return gBlockSize;
        }
    }
    long long value = (index * gBlockSize * BITS_IN_A_BYTE) + 1;
    value *= gBlockSize;
    return value;
}

/* returns the block size for the given drive name */
int bgdGetBlockSize(char driveName[])
{
    int fd = open(driveName,O_RDONLY);
    if(fd < 0)
    {
        fputs("memory error",stderr); 
        exit (2);
    }

    int offset = SUPER_BLOCK_OFFSET + BLOCK_SIZE_VALUE_OFFSET;
    int blockSize;
    unsigned char buffer[4];	
    lseek(fd,offset,SEEK_CUR);
    read (fd,buffer,sizeof(int));
    memcpy(&blockSize,buffer, sizeof(int));
    blockSize = MIN_BLOCK_SIZE << blockSize;
    return blockSize;    	           	
}

/* returns the total number of block groups in the group descriptor table */
int bgdGetNumberofBlockGroups(char driveName[])
{
    int fd = open(driveName,O_RDONLY);
    if(fd < 0)
    {
        fputs("memory error",stderr);
        exit (2);
    }

    int offset = SUPER_BLOCK_OFFSET;
    int totalNumOfInodes, totalNumOfBlocks, numOfBlocksInGroup, numOfInodesInGroup;
    unsigned char buffer[4];

    /* read 4 bytes data(int) i.e. total number of inodes */
    lseek(fd,offset,SEEK_CUR);
    read (fd,buffer,sizeof(int));
    memcpy(&totalNumOfInodes,buffer, sizeof(int));

   /* read 4 bytes of int data i.e. total number of blocks */
    read (fd,buffer,sizeof(int));
    memcpy(&totalNumOfBlocks,buffer, sizeof(int));
    
    /* read 4 bytes of data(int) i.e. number of blocks in a group at offset 32 */
    lseek(fd,NUM_OF_BLOCKS_IN_A_GROUP_REL_OFFSET,SEEK_CUR);
    read (fd,buffer,sizeof(int));
    memcpy(&numOfBlocksInGroup,buffer, sizeof(int));
    
    /* read 4 bytes of data(int) i.e. number of indoes in a group at a relative offset 40 */   
    lseek(fd,NUM_OF_INODES_IN_A_GROUP_REL_OFFSET,SEEK_CUR);
    read (fd,buffer,sizeof(int));
    memcpy(&numOfInodesInGroup,buffer, sizeof(int));
    
    int numOfBlockGroups1 = totalNumOfInodes / numOfInodesInGroup;
    int numOfBlockGroups2 = totalNumOfBlocks / numOfBlocksInGroup;
    return bgdMax(numOfBlockGroups1, numOfBlockGroups2);
}

/*Function returns 1 if the number is power of 3 or 5 or 7. 
  else returns 0. */
int bgdIsPowerOf3_5_7(int number)
{
    int num = number;
    if(num == 0)
        return 1;

    /* If the number is power of 3 */
    while(num % 9 == 0)
    {
        num /= 9;
    }
    
    if (num == 1 || num == 3)
        return 1;

    num = number;
    /* If the number is power of 5 */
    while(num % 5 == 0)
    {
        num /= 5;
    }

    if(num == 1)
        return 1;

    num = number;
    /* If the number is power of 7 */
    while(num % 7 == 0)
    {
        num /= 7;
    }

    if(num == 1)
        return 1;

    /* If the number is neither power of 3 or 5 or 7 then return 0. */
    return 0;
}
   
/*returns empty string if group descriptor table at blkGrpNum1 and blkGrpNum2 are identical.
 else returns the diff string*/
char* bgdCompareGrpDesc(int blkGrpNum1, int blkGrpNum2, char driveName[])
{
    int fd1 = open(driveName, O_RDONLY);
    int fd2 = open(driveName, O_RDONLY);

    if(!bgdIsPowerOf3_5_7(blkGrpNum1) || !bgdIsPowerOf3_5_7(blkGrpNum2))
    {
        return "Invalid block group number";
    }

    /* get the starting offset of the group descriptor table
     * for the block group numbers blkGrpNum1 and blkGrpNum2 */  
    long long offset1 = bgdGetGroupDescStartOffset(blkGrpNum1);
    long long offset2 = bgdGetGroupDescStartOffset(blkGrpNum2);

    lseek64(fd1,offset1,SEEK_SET);
    lseek64(fd2,offset2,SEEK_SET);

    int bg_iterator = 0;
    struct ext3_group_desc * gdesc1 = (struct ext3_group_desc *)malloc(sizeof(struct ext3_group_desc));
    struct ext3_group_desc * gdesc2 = (struct ext3_group_desc *)malloc(sizeof(struct ext3_group_desc));

    char *buff1 = (char *)malloc(sizeof(struct ext3_group_desc));
    char *buff2 = (char *)malloc(sizeof(struct ext3_group_desc));
    char *compResult = (char *)malloc(sizeof(struct ext3_group_desc) * gBlockGroupCount * 100);
    memset(compResult, 0, sizeof(struct ext3_group_desc) * gBlockGroupCount * 100);
    while(bg_iterator < gBlockGroupCount)
    {
        read(fd1,buff1,sizeof(struct ext3_group_desc));
        memcpy((void *)gdesc1,(void *)buff1,sizeof(struct ext3_group_desc));

        read(fd2,buff2,sizeof(struct ext3_group_desc));
        memcpy((void *)gdesc2,(void *)buff2,sizeof(struct ext3_group_desc));

        char temp[100];
        if(gdesc1->bg_block_bitmap != gdesc2->bg_block_bitmap)
        {
            memset(temp, 0, 100);
            sprintf(temp, "Block bitmap[%d]  ->  %ld | %ld\n", bg_iterator, gdesc1->bg_block_bitmap, gdesc2->bg_block_bitmap);
            strcat(compResult, temp);  
        }
        if(gdesc1->bg_inode_bitmap != gdesc2->bg_inode_bitmap)
        {
            memset(temp, 0, 100);
            sprintf(temp, "Inode bitmap[%d]  ->  %ld | %ld\n", bg_iterator, gdesc1->bg_inode_bitmap, gdesc2->bg_inode_bitmap);
            strcat(compResult, temp);
        }
        if(gdesc1->bg_inode_table != gdesc2->bg_inode_table)
        {
            memset(temp, 0, 100);
            sprintf(temp, "Inode table[%d]   ->  %ld | %ld\n", bg_iterator, gdesc1->bg_inode_table, gdesc2->bg_inode_table);
            strcat(compResult, temp);
        }
        if(gdesc1->bg_free_blocks_count != gdesc2->bg_free_blocks_count)
        {
            memset(temp, 0, 100);
            sprintf(temp, "Free blocks[%d]   ->  %d | %d\n", bg_iterator, gdesc1->bg_free_blocks_count, gdesc2->bg_free_blocks_count);
            strcat(compResult, temp);
        }
        if(gdesc1->bg_free_inodes_count != gdesc2->bg_free_inodes_count)
        {
            memset(temp, 0, 100);
            sprintf(temp, "Free inodes[%d]   ->  %d | %d\n", bg_iterator, gdesc1->bg_free_inodes_count, gdesc2->bg_free_inodes_count);
            strcat(compResult, temp);
        }
        if(gdesc1->bg_used_dirs_count != gdesc2->bg_used_dirs_count)
        {
            memset(temp, 0, 100);
            sprintf(temp, "Used dirs[%d]     ->  %d | %d\n", bg_iterator, gdesc1->bg_used_dirs_count, gdesc2->bg_used_dirs_count);
            strcat(compResult, temp);
        }
        bg_iterator++;
    }
    free(buff1);
    free(buff2);
    free(gdesc1);
    free(gdesc2);
    return compResult;
}

/* init function which will be called at the start to update block size
 * block group count and group descriptor table */
int bgdInit(char *dName)
{
    gBlockSize = bgdGetBlockSize(dName);
    gBlockGroupCount = bgdGetNumberofBlockGroups(dName);

    gGrpDescTable = bgdGetGrpDescTable(dName, 0, 0);
    return gBlockSize;
}

/* iterate through all the blocks of the blockGroupNo
 * update the bitmap to indicate the null blocks among 
 * unused blocks */
void bgdUpdateNullBlocks(int fd, int blockGroupNo, char *colorCodeBitmap)
{
    int maxBlocks = gBlockSize * BITS_IN_A_BYTE;
    int i;
    int count = 0;
    for(i = 0;i < maxBlocks;++i)
    {
        if(!colorCodeBitmap[i])
        {
            int blockNum = i + (blockGroupNo * gBlockSize * BITS_IN_A_BYTE);
            long long offset = blockNum * gBlockSize;
            lseek64(fd, offset, SEEK_SET);
            unsigned char content[4] = {0};
            read(fd, content, 4);

            /* if the string length of the first four bytes is zero
             * it can be concluded that the data block is empty */ 
            if(strlen(content) == 0)
            {
                colorCodeBitmap[i] = (int)NULL_BLOCK;
                count++;
            }
        }
    }
    printf("Total number of null blocks(unreserved) are %d\n", count);
}

/* iterate through all the blocks of the blockGroupNo
 * update the bitmap to indicate the address blocks 
 * among unused blocks*/
void bgdUpdateAddrBlocks(int fd, int blockGroupNo, char *colorCodeBitmap)
{
    int maxBlocks = gBlockSize * BITS_IN_A_BYTE;
    int i;
    int count = 0;
    for(i = 0;i < maxBlocks;++i)
    {
        if(!colorCodeBitmap[i])
        {
            int blockNum = i + (blockGroupNo * gBlockSize * BITS_IN_A_BYTE);
            long long offset = blockNum * gBlockSize;
            lseek64(fd, offset, SEEK_SET);
            char *data = (char *)malloc(gBlockSize);
            memset((void *)data, 0, gBlockSize);
            read(fd, data, gBlockSize);
            int addr1, addr2;

            /* read first eight bytes of data into an integer variable */  
            memcpy(&addr1, data, sizeof(int));
            data += sizeof(int);
            memcpy(&addr2, data, sizeof(int));

            /* if the difference between two integers at the first 8 bytes
             * is 1 then it can be concluded that it's an address block */
            if(abs(addr1 - addr2) == 1)
            {
                colorCodeBitmap[i] = (int)ADDR_BLOCK;
                count++;
                //printf("Block address is at %d\n", blockNum);
            }
            else
            {
                /* If the integer value of the first 8 bytes are not consecutive
                 * then try next 8 bytes else conclude that it's not an address block */ 
                data += sizeof(int);
                memcpy(&addr1, data, sizeof(int));
                data += sizeof(int);
                memcpy(&addr2, data, sizeof(int));
                if(abs(addr1 - addr2) == 1)
                {
                    colorCodeBitmap[i] = (int)ADDR_BLOCK;
                    count++;
                    //printf("Address Block is at %d\n", blockNum);
                }   
            }
        }
    }
    printf("Total number of address blocks(unreserved) are %d\n", count);
}

/* iterate through all the blocks of the blockGroupNo
 * update the bitmap to indicate the text blocks among
 * unused blocks*/
void bgdUpdateTextBlocks(int fd, int blockGroupNo, char *colorCodeBitmap)
{
    int maxBlocks = gBlockSize * BITS_IN_A_BYTE;
    int i;
    int count = 0;
    int dirCount = 0;
    for(i = 0;i < maxBlocks;++i)
    {
        if(!colorCodeBitmap[i])
        {
            int blockNum = i + (blockGroupNo * gBlockSize * BITS_IN_A_BYTE);
            long long offset = blockNum * gBlockSize;
            lseek64(fd, offset, SEEK_SET);
            char *data = (char *)malloc(gBlockSize);
            memset((void *)data, 0, gBlockSize);
            read(fd, data, gBlockSize);
            int k;
            int found = 0;
            
            /* read the block data into directory structure */
            struct ext3_dir_entry_2 DirEntry;
            memset(&DirEntry, 0, sizeof(DirEntry));
            memcpy(&DirEntry, data, sizeof(DirEntry));
            
            /* First entry of any directory block is ".". After reading block data
             * into a directory structure, if it is "." and length is 1 then 
             * it can be concluded that the data block is a directory block */ 
            if((strcmp(DirEntry.name, ".")) == 0 && (DirEntry.name_len == 1))
            {
                found =1;
                colorCodeBitmap[i] = (int)DIR_BLOCK;
                dirCount++;
                continue;				
            }
            else
            {
                for(k =0; k<gBlockSize; ++k)
                {
                    /* If all the bytes in the data blocks are in
                     * the ASCII range then it can be concluded that
                     * the data block is a text block */
                    if(data[k] > LAST_ASCII_HEX)
                    {
                        found = 1;                    
                        break;
                    }
                /*else if(data[k] < 0x20)
                {
                    if(data[k] != 0x09 && data[k] != 0x0A)
                    {
                        found = 1;
                        break;
                    }
                }*/
                }
            }
            if(found == 0)
            {
                colorCodeBitmap[i] = (int)TEXT_BLOCK;
                //printf("Text Block is at %d\n", blockNum);
                count++;
            }
        }
    }
    printf("Total number of text blocks(unreserved) are %d\n", count);
    printf("Total number of directory blocks(unreserved) are %d\n", dirCount);
}

/* given a drive name and the block group number, this function 
 * first reads the block bitmap and copies the corresponding bitmap
 * data and then updates null blocks, text blocks, address blocks,
 * directory blocks among the unused blocks i.e. the one with bitmap
 * entry as zero */
unsigned char* bgdReadBlockBitmap(char driveName[], int blockGroupNo)
{
    unsigned long blockBitMapBlockNum = getBlockBitMapAddr(blockGroupNo);
    long long offset = blockBitMapBlockNum * gBlockSize;
    unsigned char* colorCodeBitmap = (unsigned char *)malloc(gBlockSize * BITS_IN_A_BYTE);
    memset((void *)colorCodeBitmap, 0, gBlockSize * BITS_IN_A_BYTE);
    int fd = open(driveName,O_RDONLY);
    if(fd < 0)
    {
        fputs("drive read failed",stderr);
        exit(2);
    }

    lseek64(fd, offset, SEEK_SET);
    unsigned char *data = (unsigned char *)malloc(gBlockSize);
    memset((void *)data, 0, gBlockSize);
    read(fd, data, gBlockSize);
    int i, counter;
    counter = 0;

    for(i = 0;i<gBlockSize;++i)
    {
        int index = i * BITS_IN_A_BYTE;
        if(data[i] & 0x01)
        {
            counter++;
            colorCodeBitmap[index] = USED_BLOCK;
        }
        if(data[i] & 0x02)
        {
            counter++;
            colorCodeBitmap[index + 1] = USED_BLOCK;
        }
        if(data[i] & 0x04)
        {
            counter++;
            colorCodeBitmap[index + 2] = USED_BLOCK;
        }
        if(data[i] & 0x08)
        {
            counter++;
            colorCodeBitmap[index + 3] = USED_BLOCK;
        }
        if(data[i] & 0x10)
        {
            counter++;
            colorCodeBitmap[index + 4] = USED_BLOCK;
        }
        if(data[i] & 0x20)
        {
            counter++;
            colorCodeBitmap[index + 5] = USED_BLOCK;
        }
        if(data[i] & 0x40)
        {
            counter++;
            colorCodeBitmap[index + 6] = USED_BLOCK;
        }
        if(data[i] & 0x80)
        {
            counter++;
            colorCodeBitmap[index + 7] = USED_BLOCK;
        }
    }
    printf("Total number of used blocks are %d\n", counter);

    bgdUpdateNullBlocks(fd, blockGroupNo, colorCodeBitmap);
    bgdUpdateAddrBlocks(fd, blockGroupNo, colorCodeBitmap);
    bgdUpdateTextBlocks(fd, blockGroupNo, colorCodeBitmap);

    counter = 0;
    int maxBlocks = gBlockSize * BITS_IN_A_BYTE;
    for(i = 0;i<maxBlocks;++i)
    {
        if(!colorCodeBitmap[i])
            counter++;
    }
    printf("Total number of unknown blocks(unreserved) are %d\n", counter);
    return colorCodeBitmap;
}

/* given a block group number, this function returns the
 * block bitmap address by looking at the group descriptor
 * table which was built at the start */   
unsigned long getBlockBitMapAddr(int blockGroupNo)
{
    return gGrpDescTable[blockGroupNo].bg_block_bitmap;
}

/* given a block group number and drive name, reads group descriptor table and output
 * its contents to an output file named output.txt and hexdump.txt. Init function uses
 * this module with block group number as zero to store the group descriptor table into
 * the global variable gGrpDescTable for future usage. */
struct ext3_group_desc * bgdGetGrpDescTable(char driveName[], int blockGroupNo, int fileWrite)
{
    long long offset =0;
    FILE* output_file;
    FILE* hex_dump;
    int index;
    char *buff = (char *)malloc(sizeof(struct ext3_group_desc));
    struct ext3_group_desc * gdesc = (struct ext3_group_desc *)malloc(sizeof(struct ext3_group_desc));
    
    /* structure to store all block group descriptors */ 
    struct ext3_group_desc *gDescTable = (struct ext3_group_desc *)malloc(gBlockGroupCount * sizeof(struct ext3_group_desc));    

    /* The different fields in the block group descriptor */
    unsigned char block_group_descriptor[8][30] = {"Blocks bitmap block","Inodes bitmap block","Inode table block","Free blocks count","Free inodes count","Used dirs count"/*,"bg_pad","bg_reserved"*/};
    int fd = open(driveName,O_RDONLY);
    if(fd < 0)
    {
        fputs("memory error",stderr);
        exit (2);
    }

    /* block group desc table size is one block group desc size(32) * total number of block groups */
    int blockGroupTblSize = gBlockGroupCount * BLOCK_GROUP_DESC_SIZE;
    unsigned char buffer[blockGroupTblSize];
    offset = bgdGetGroupDescStartOffset(blockGroupNo);

    /* Go to the first block group descriptor in the second block */	
    lseek64(fd,offset,SEEK_SET);
	
    /* create output file if required */
    if(fileWrite)
    {
        char fileName[MAX_FILENAME_LENGTH];
        sprintf(fileName, "output%d.txt", blockGroupNo);
        output_file = fopen(fileName,"write");
        //open the file for hex dump
        memset(fileName, 0, MAX_FILENAME_LENGTH);
        sprintf(fileName, "hexdump%d.txt", blockGroupNo);
        hex_dump = fopen(fileName, "write");
    }

    /* read block group desc table into buffer which will be used for hex dump */
    int retVal = read(fd, buffer, blockGroupTblSize);
    if(retVal  <= 0)
    {
        fprintf(stderr, "unable to read disk, retVal = %d\n", retVal);
        return;
    }

    /* hex dump i.e 16 bytes in each line */
    if(fileWrite)
    {
        int byteCount = 0;
        for(index=0;index<blockGroupTblSize;index++)
        {
            fprintf(hex_dump, "%02x ", buffer[index]);
            byteCount++;
            if(byteCount == HEX_DUMP_LINE_SIZE)
            {
                fprintf(hex_dump, "\n");
                byteCount = 0;
            }
        }
    }

    if(fileWrite)
    {
        /* print the group descriptor field descriptions in the output file */
        for(index=0;index<8;++index)
        {
            fprintf(output_file,"%s    ", block_group_descriptor[index]);
        }	

        fprintf(output_file,"\n");
    }
	lseek64(fd,offset,SEEK_SET);
    //iterate through all the group descriptors in the group descriptor table
    int bg_iterator = 0;
    while(bg_iterator < gBlockGroupCount)
    {
        /* read each group descriptor and write it to output file. */
        read(fd,buff,sizeof(struct ext3_group_desc));
        memcpy((void *)&gDescTable[bg_iterator], (void *)buff, sizeof(struct ext3_group_desc));

        if(fileWrite)
        {
            memcpy((void *)gdesc,(void *)buff,sizeof(struct ext3_group_desc));
            fprintf(output_file, "%15ld|", gdesc->bg_block_bitmap);
            fprintf(output_file, "%18ld|", gdesc->bg_inode_bitmap);
            fprintf(output_file, "%20ld|", gdesc->bg_inode_table);
            fprintf(output_file, "%22d|", gdesc->bg_free_blocks_count);
            fprintf(output_file, "%18d|", gdesc->bg_free_inodes_count);
            fprintf(output_file, "%16d\n", gdesc->bg_used_dirs_count);
        }
        bg_iterator++;
    }
    if(fileWrite)
    {
        close(output_file);
        close(hex_dump);
    }
    free(gdesc);
    free(buff);
    return gDescTable;
}

/* Reads a single indirect block located at blockNumber and
 * read each 4 bytes into an integer variable which is an address
 * pointing to a data block and print the content of data block to
 * the file fDataOutput.Also, copy the content of single indirect
 * block of addresses to the file fBlockAddr */
void bgdReadSingleIndirectBlocks(int blockNumber, FILE *fDataOutput, int fp, int fileSizeInBlocks, FILE *fBlockAddr, FILE *fBin)
{
    char *data = (char *)malloc(gBlockSize);
    long long offset = blockNumber * gBlockSize;
    printf("Block addresses block number is %d and the offset is %lld\n", blockNumber, offset);
    lseek64(fp, offset, SEEK_SET);
    memset((void *)data, 0, gBlockSize);
    read(fp, data, gBlockSize);
    printf("%s\n", data);
    int addr;
    int counter = 1;
    int maxCount = fileSizeInBlocks - DIRECT_BLOCKS_COUNT;
    if(maxCount > SINGLE_INDIRECT_BLOCKS_COUNT)
        maxCount = SINGLE_INDIRECT_BLOCKS_COUNT;

    char *content = (char *)malloc(gBlockSize);
    while(counter <= maxCount)
    {
        memcpy(&addr, (void *)data, sizeof(int));
        if(addr > 0)
        {
            memset((void *)content, 0, gBlockSize);
            long long byteOffset = addr * gBlockSize;
            lseek64(fp, byteOffset, SEEK_SET);
            read(fp, content, gBlockSize);
            //printf("%s", content);
            fprintf(fDataOutput, "%s", content);
            fwrite(&content, sizeof(content), 1, fBin);
            fprintf(fBlockAddr, "%7d ", addr);
            if(counter % 16 == 0)
                fprintf(fBlockAddr, "\n");
        }
        else
        {
            break;
        }
        data += sizeof(int);
        counter++;
    }
    printf(".......Finished Indirect Block.........and the counter is %d\n", counter);
    fprintf(fBlockAddr, "\n\n");
    //free(data);
    //free(content);
}

/* Reads the double indirect block located at blockNumber and
 * read 4 bytes into an integer variable and call bgdReadSingleIndirectBlocks
 * by passing the integer variable as a blockNumber till the end of double 
 * indirect block */
void bgdReadDoubleIndirectBlocks(int blockNumber, FILE *fDataOutput, int fp, int fileSizeInBlocks, FILE *fBlockAddr, FILE *fBin)
{
    char *data = (char *)malloc(gBlockSize);
    long long offset = blockNumber * gBlockSize;
    lseek64(fp, offset, SEEK_SET);
    memset((void *)data, 0, gBlockSize);
    read(fp, data, gBlockSize);
    int addr;
    int counter = 0;
    int maxCount = fileSizeInBlocks - DIRECT_BLOCKS_COUNT - SINGLE_INDIRECT_BLOCKS_COUNT;
    if(maxCount > DOUBLE_INDIRECT_BLOCKS_COUNT)
        maxCount = DOUBLE_INDIRECT_BLOCKS_COUNT;

    printf("... Double Indirect Block and max count is %d\n", maxCount);

    while(counter < maxCount)
    {
        memcpy(&addr, (void *)data, sizeof(int));
        if(addr > 0)
        {
            bgdReadSingleIndirectBlocks(addr, fDataOutput, fp, fileSizeInBlocks, fBlockAddr, fBin);
            printf("Finished single indirect block %d in double indirect block\n", counter+1);
        }
        else 
            break;
        data += sizeof(int);
        counter++;
    }
    //free(data);
}

/* Reads a triple indirect block located at blockNumber and call
 * bgdReadDoubleIndirectBlocks by reading each four bytes into an 
 * integer variable */
void bgdReadTripleIndirectBlocks(int blockNumber, FILE *fDataOutput, int fp, int fileSizeInBlocks, FILE *fBlockAddr, FILE *fBin)
{
    char *data = (char *)malloc(gBlockSize);
    long long offset = blockNumber * gBlockSize;
    lseek64(fp, offset, SEEK_SET);
    memset((void *)data, 0, gBlockSize);
    read(fp, data, gBlockSize);
    int addr;
    int counter = 0;
    int maxCount = fileSizeInBlocks - DIRECT_BLOCKS_COUNT - SINGLE_INDIRECT_BLOCKS_COUNT - DOUBLE_INDIRECT_BLOCKS_COUNT;
    /*if(maxCount > TRIPLE_INDIRECT_BLOCKS_COUNT)
        maxCount = TRIPLE_INDIRECT_BLOCKS_COUNT;*/

    printf("... Triple Indirect Block and max count is %d\n", maxCount);

    while(counter < maxCount)
    {
        memcpy(&addr, (void *)data, sizeof(int));
        if(addr > 0)
        {
            bgdReadDoubleIndirectBlocks(addr, fDataOutput, fp, fileSizeInBlocks, fBlockAddr, fBin);
            printf("Finished double indirect block %d in triple indirect block\n", counter+1);
        }
        else 
            break;
        data += sizeof(int);
        counter++;
    }
    //free(data);
}

/* Reads all the data blocks for the given inode number 
 * by reading i_block entry of the inode table which
 * contains 12 direct block addresses, 1 single indirect block
 * address, 1 double indirect address and 1 triple indirect address.*/
void bgdReadFromInode(int inode, char dName[])
{
    char *buffer = (char *)malloc(DEFAULT_EXT3_INODE_SIZE);
    struct ext3_inode *inode_tab = (struct ext3_inode *)malloc(DEFAULT_EXT3_INODE_SIZE);
    int blockGroupNo, inodeTableIndex;

    /* get the block group number where the file resides */
    blockGroupNo = inode / gBlockSize;

    /* get the index of the inode table */
    inodeTableIndex = (inode % gBlockSize) - 1;

    /* read inode table starting block number from group descriptor table */
    long inodeTableBlockNum = gGrpDescTable[blockGroupNo].bg_inode_table;

    /* calculate the offset in bytes for the file seek */
    long long offset = (inodeTableBlockNum * gBlockSize) + (inodeTableIndex * DEFAULT_EXT3_INODE_SIZE);
    int fd = open(dName, O_RDONLY);
    lseek64(fd,offset,SEEK_SET);
    read(fd, buffer, DEFAULT_EXT3_INODE_SIZE);
    memcpy((void *)inode_tab, (void *)buffer, DEFAULT_EXT3_INODE_SIZE);
    int fileSizeInBlocks;
    printf("File size is %d\n", inode_tab->i_size);
    if(inode_tab->i_size % gBlockSize == 0)
        fileSizeInBlocks = inode_tab->i_size / gBlockSize;
    else
        fileSizeInBlocks = (inode_tab->i_size / gBlockSize) + 1;
    
    char *data = (char *)malloc(gBlockSize);
    int index = 0;
    int fp = open(dName, O_RDONLY);
    int blockNumber;

    char fileName[MAX_FILENAME_LENGTH];
    sprintf(fileName, "inode%d.txt", inode);
    FILE *file_data = fopen(fileName,"write");
    memset((void *)fileName, 0, MAX_FILENAME_LENGTH);
    sprintf(fileName, "baddr_inode%d.txt", inode);
    FILE *block_addr = fopen(fileName, "write");

    memset((void *)fileName, 0, MAX_FILENAME_LENGTH);
    sprintf(fileName, "inode%d.bin", inode);
    FILE *file_bin = fopen(fileName, "ab");

    printf("File Size in Blocks is %d\n", fileSizeInBlocks);
    lseek(fp, 0, SEEK_SET);

    /* read the direct blocks */
    if(fileSizeInBlocks <= DIRECT_BLOCKS_COUNT || 
       fileSizeInBlocks > DIRECT_BLOCKS_COUNT)
    {
        int maxCount = fileSizeInBlocks;
        if(maxCount > DIRECT_BLOCKS_COUNT)
            maxCount = DIRECT_BLOCKS_COUNT;

        fprintf(block_addr, "The direct block addresses are:\n");

        while(index < maxCount)
        {
            offset = 0;
            memset((void *)data, 0, gBlockSize);
            blockNumber = 0;
            blockNumber = inode_tab->i_block[index++];        
            offset = blockNumber * gBlockSize;
            printf("Block number %d ..... and Offset %lld\n", blockNumber, offset);
            lseek64(fp, offset, SEEK_SET);
            read(fp, data, gBlockSize);
            //printf("%s", data);
            fwrite(&data, sizeof(data), 1, file_bin);
            fprintf(file_data, "%s", data);
            fprintf(block_addr, "%d ", blockNumber);
        }
        fprintf(block_addr, "\n\n");        
    }

    /* read single indirect block */
    if(fileSizeInBlocks > DIRECT_BLOCKS_COUNT &&
       (fileSizeInBlocks <= SINGLE_INDIRECT_BLOCKS_COUNT ||
       fileSizeInBlocks > SINGLE_INDIRECT_BLOCKS_COUNT))
    {
        blockNumber = inode_tab->i_block[index++];
        fprintf(block_addr, "The Single Indirect block addresses are:\n");
        bgdReadSingleIndirectBlocks(blockNumber, file_data, fp, fileSizeInBlocks, block_addr, file_bin);
        fprintf(block_addr, "\n\n");
    }

    /* read double indirect block */
    if(fileSizeInBlocks > SINGLE_INDIRECT_BLOCKS_COUNT &&
           (fileSizeInBlocks <= DOUBLE_INDIRECT_BLOCKS_COUNT ||
            fileSizeInBlocks > DOUBLE_INDIRECT_BLOCKS_COUNT))
    {
        blockNumber = inode_tab->i_block[index++];
        fprintf(block_addr, "The Double Indirect block addresses are:\n");
        bgdReadDoubleIndirectBlocks(blockNumber, file_data, fp, fileSizeInBlocks, block_addr, file_bin);
        fprintf(block_addr, "\n\n");
    }

    /* read triple indirect block */
    if(fileSizeInBlocks > DOUBLE_INDIRECT_BLOCKS_COUNT)
    {
        blockNumber = inode_tab->i_block[index++];
        fprintf(block_addr, "The Triple Indirect block addresses are:\n");
        bgdReadTripleIndirectBlocks(blockNumber, file_data, fp, fileSizeInBlocks, block_addr, file_bin);
    }
    close(file_data);
    close(block_addr);
    close(file_bin);
    free(buffer);
    free(inode_tab);
    //free(data);
}

/* This function returns the block group descriptor of the 
 * given blockGroupNum by reading global block group descriptor
 * table with | as the delimiter. */
char *bgdGetBlockGroupInfo(int blockGroupNum)
{
//    if(!bgdIsPowerOf3_5_7(blockGroupNum))
//    {
//        return "";
//    }
    char *bgdInfo = (char *)malloc(BLOCK_GROUP_INFO_MAX_SIZE);
    memset((void *)bgdInfo, 0, BLOCK_GROUP_INFO_MAX_SIZE); 
    char target[BLOCK_GROUP_FIELD_MAX_SIZE] = {0};
    sprintf(target, "%ld", gGrpDescTable[blockGroupNum].bg_block_bitmap);
    strcpy(bgdInfo, target);
    strcat(bgdInfo, " ");
    memset((void *)target, 0, BLOCK_GROUP_FIELD_MAX_SIZE);
    sprintf(target, "%ld", gGrpDescTable[blockGroupNum].bg_inode_bitmap);
    strcat(bgdInfo, target);
    strcat(bgdInfo, " ");
    memset((void *)target, 0, BLOCK_GROUP_FIELD_MAX_SIZE);
    sprintf(target, "%ld", gGrpDescTable[blockGroupNum].bg_inode_table);
    strcat(bgdInfo, target);
    strcat(bgdInfo, " ");
    memset((void *)target, 0, BLOCK_GROUP_FIELD_MAX_SIZE);
    sprintf(target, "%d", gGrpDescTable[blockGroupNum].bg_free_inodes_count);
    strcat(bgdInfo, target);
    strcat(bgdInfo, " ");
    memset((void *)target, 0, BLOCK_GROUP_FIELD_MAX_SIZE);
    sprintf(target, "%d", gGrpDescTable[blockGroupNum].bg_free_blocks_count);
    strcat(bgdInfo, target);
    strcat(bgdInfo, " ");
    memset((void *)target, 0, BLOCK_GROUP_FIELD_MAX_SIZE);
    sprintf(target, "%d", gGrpDescTable[blockGroupNum].bg_used_dirs_count);
    strcat(bgdInfo, target);
    return bgdInfo;
}

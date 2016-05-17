/*
Program to print the block group descriptor table of a drive

Input: device path (note: the program needs super user privillages to access the drives)
Output: An output.txt file that contains all fileds of the block group descriptor
*/


#include <sys/stat.h>
#include <errno.h>
#include <linux/hdreg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

#define SIZE_OF_GROUP_DESCRIPTOR 32
#define BLOCK_SIZE 4096
#define BLOCKS_IN_GROUP 32768


int main(int argc, char* argv[])
{
	int offset =0;
	int current_block=0;
	int fp;
	FILE* output_file;
	unsigned char buffer[32];
	int result;
	int i,j;

	//The different fields in the block group descriptor
	unsigned char block_group_descriptor[8][30] = {"bg_block_bitmap: 1","bg_node_bitmap: 2","bg_node_table: 3","bg_free_block_count: 4","bg_free_inodes_count: 5","bg_used_dirs_count: 6","bg_pad: 7","bg_reserved: 8"};
	
	//Open the drive specified in the argument
	fp = open(argv[1],O_RDONLY);
	if( fp<0)
	{
		printf("\n%d",strerror(errno));
				
		fputs("memory error",stderr); 
		exit (2);
	}
	

	offset=BLOCK_SIZE*(1);
	//Go to the first block group descriptor in the second block	
	lseek(fp,offset,SEEK_CUR);
	
	//Read the entire 32 Bytes of info in the block group descriptor
	result = read (fp,buffer,32);
	close(fp);
	
	
	output_file=fopen("output.txt","write");
	if(result<=0)
	{
		printf("\nERROR:%d",result);
		fputs("\nERROR:NO DATA\n",stderr);
		exit(2);
	}
	
	//Print the column information	
	fprintf(output_file,"Description: Column Number\n");
	for(j=0;j<8;j++)
	{
		fprintf(output_file,"%s\n",block_group_descriptor[j]);	

	}
	fprintf(output_file,"\n");
	
	//Print all the fields in the block group descriptor
	for(i=0;i<SIZE_OF_GROUP_DESCRIPTOR;i++)
	{
		//Differenciate the fields according their sizes
		if(i==4|i==8|i==12|i==14|i==16|i==18|i==20)
		{
			fprintf(output_file,"  |  ");
		}	
		fprintf(output_file,"%x",buffer[i]);
		fprintf(output_file," ");
	}
	close(output_file);
}

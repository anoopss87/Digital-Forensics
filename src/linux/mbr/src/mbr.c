#include "mbrinc.h"
#include <string.h>

#ifndef GRAPHICAL
extern struct hd_geometry geometry;
#endif
#define SIZE_OF_MBR 512
#define BLK_SIZE 4096
#define MAGIC1 510
#define MAGIC2 511

#define MAGIC_VAL1 0x55
#define MAGIC_VAL2 0xAA
#define START_PARTITION_DATA 446
#define MAXNUM_OF_PARTS 4

void   printMBR( int fd ) {
   unsigned char b[BLK_SIZE];
   int retVal;
   int i, j;
   int countr;
   unsigned short s;
   unsigned long partSize, l;

#ifndef GRAPHICAL
   if (!ioctl(fd, HDIO_GETGEO, &geometry)) {
                printf(" heads = %d\n", geometry.heads);
                printf(" sectors = %d\n", geometry.sectors);
                printf(" cylinders = %d\n", geometry.cylinders);
   }
#endif

   retVal = read(fd, b , SIZE_OF_MBR);
   if ( retVal <=0 ) {
      fprintf(stderr, "unable to read disk, retVal = %d\n", 				retVal );
      return;
   }
   if ( (b[MAGIC1] != MAGIC_VAL1) && (b[MAGIC2] != MAGIC_VAL2) )
   {
      fprintf(stderr, "MBR corrupted ... \n");
      return;
   }

   /* Print the MBR 16 bytes per line */
   j=0;
   for(i=0;i<512;i++)
   {
      fprintf(stdout, "%2x ",b[i]);
      j++;
      if ( j == 16 ) {
         fprintf(stdout, "\n");
         j = 0;
      }
   }
   fprintf(stdout, "\n\n" );
   countr = START_PARTITION_DATA;
   for ( i = 0; i < MAXNUM_OF_PARTS; i++ ) {
      partSize = b[countr+15];
      partSize = partSize << 8;
      partSize = partSize | b[countr+14];
      partSize = partSize << 8;
      partSize = partSize | b[countr+13];
      partSize = partSize << 8;
      partSize = partSize | b[countr+12];
      if ( partSize > 0 ) {
    
         fprintf(stdout, "Boot indicator = %x\n", b[countr] );
         fprintf(stdout, "starting CHS head = %x\n", b[countr+1] );
         fprintf(stdout, "starting CHS sector = %x\n", b[countr+2] & 0x2f );
         s = b[countr+2] & 0xc0 ;
         s = s << 2 ;
         s = s | b[countr+3];
         fprintf(stdout, "starting CHS Cylinder = %x\n", s );
         fprintf(stdout, "Partition type = %x\n", b[countr+4] );
         fprintf(stdout, "Ending CHS head = %x\n", b[countr+5] );
         fprintf(stdout, "Ending CHS sector = %x\n", b[countr+6] & 0x2f );
         s = b[countr+6] & 0xc0 ;
         s = s << 2 ;
         s = s | b[countr+7];
         fprintf(stdout, "Ending CHS Cylinder = %x\n", s );
         l = b[countr+11];
         l = l << 8;
         l = l | b[countr+10];
         l = l << 8;
         l = l | b[countr+9];
         l = l << 8;
         l = l | b[countr+8];
         fprintf(stdout, "Starting Sector = %ld\n", l );
         fprintf(stdout, "partition Size = %ld\n", partSize );
         fprintf(stdout, "\n\n" );
      }
      countr += 16; // next partition
   }

}

int loadMBRArray(char array[5][200], char* disk) {
	int fd = open(disk, O_RDONLY);
	unsigned char b[BLK_SIZE];
	int retVal;
	int i, j;
	int countr;
	unsigned short s;
	unsigned long partSize, l;
	int partitionCount = 0;

	retVal = read(fd, b , SIZE_OF_MBR);
	if ( retVal <=0 ) {
		fprintf(stderr, "unable to read disk, retVal = %d\n", retVal );
		return -1;
	}
	if ( (b[MAGIC1] != MAGIC_VAL1) && (b[MAGIC2] != MAGIC_VAL2) )
	{
		fprintf(stderr, "MBR corrupted ... \n");
		return -1;
	}

	/* Print the MBR 16 bytes per line */
//	j=0;
//	for(i=0;i<512;i++)
//	{
//		fprintf(stdout, "%2x ",b[i]);
//		j++;
//		if ( j == 16 ) {
//			fprintf(stdout, "\n");
//			j = 0;
//		}
//	}
//	fprintf(stdout, "\n\n" );
	countr = START_PARTITION_DATA;
	for ( i = 0; i < MAXNUM_OF_PARTS; i++ ) {
		partSize = b[countr+15];
		partSize = partSize << 8;
		partSize = partSize | b[countr+14];
		partSize = partSize << 8;
		partSize = partSize | b[countr+13];
		partSize = partSize << 8;
		partSize = partSize | b[countr+12];
		if ( partSize > 0 ) {
			partitionCount += 1;
			char stri[100]="";
//			fprintf(stdout, "Boot indicator = %x\n", b[countr] );
//			fprintf(stdout, "starting CHS head = %x\n", b[countr+1] );
//			fprintf(stdout, "starting CHS sector = %x\n", b[countr+2] & 0x3f );
			s = b[countr+2] & 0xc0 ;
			s = s << 2 ;
			s = s | b[countr+3];
			sprintf(stri, "%d %d %d %d ", b[countr], b[countr+1], b[countr+2]&0x3f, s);
			strcpy(array[i+1], stri);
//			fprintf(stdout, "starting CHS Cylinder = %x\n", s );
//			fprintf(stdout, "Partition type = %x\n", b[countr+4] );
//			fprintf(stdout, "Ending CHS head = %x\n", b[countr+5] );
//			fprintf(stdout, "Ending CHS sector = %x\n", b[countr+6] & 0x3f );
			s = b[countr+6] & 0xc0 ;
			s = s << 2 ;
			s = s | b[countr+7];
//			fprintf(stdout, "Ending CHS Cylinder = %x\n", s );
			l = b[countr+11];
			l = l << 8;
			l = l | b[countr+10];
			l = l << 8;
			l = l | b[countr+9];
			l = l << 8;
			l = l | b[countr+8];
//			fprintf(stdout, "Starting Sector = %ld\n", l );
//			fprintf(stdout, "partition Size = %ld\n", partSize );
			sprintf(stri, "%x %d %d %d %lu %lu", b[countr+4], b[countr+5], b[countr+6]&0x3f, s, l, partSize);
//			fprintf(stdout, "\n\n" );
			strcat(array[i+1], stri);
		}
		countr += 16; // next partition
	}
	sprintf(array[0], "%d", partitionCount);
	return 0;
}

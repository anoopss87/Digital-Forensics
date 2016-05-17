#include "mbrinc.h"

struct hd_geometry geometry;

void getMetaData( int fd ) {
	printMBR( fd );
}

void main(int argc, char** argv) {
	FILE *hd;
	int fd;

	if(argc != 2) {
		fprintf(stderr, "USAGE: %s /dev/sdx\n", argv[0]);
		return;
	}
	fd = open(argv[1], O_RDONLY);
	if ( fd <= 0 )
	{
		fprintf(stderr, "File not opened = %x %s\n", hd , strerror(errno));
		return;
	}
	getMetaData( fd );
	close( fd );
}

#include <stdio.h> 
#include <errno.h>
#include <string.h>
#include <stdlib.h>

extern int init(char *disk);

int main(int argc, char **argv) 
{ 
	if (argc != 2) 
	{
		printf("Error: %s\n", strerror(errno));
		return -1;
	}
	FILE *fp = fopen(argv[1], "rb");
	if (fp < 0)
	{
		printf("Error %s\n", strerror(errno));
		return -1;
	}
	init(fp);
	fclose(fp);
	return 0;
}

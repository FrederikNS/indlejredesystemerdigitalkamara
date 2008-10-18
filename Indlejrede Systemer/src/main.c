#include <stdio.h>
#include "bmp.h"
#define DEBUG

int main (int argc, char *argv[]) {

	#ifdef DEBUG
	printf("Start of main.\n");
	#endif

	printf("Image filtering.\n");
	char File[]="./example24.bmp";
	char File2[]="./result.bmp";
	bmp_ctgc(File,File2);

	printf("Decompression.\n");
	char File3[]="./example8rle.bmp";
	char File4[]="./result_uncompressed.bmp";
	bmp_decompress(File3,File4);

	printf("Compression.\n");
	char File5[]="./resultx_-1.bmp";
	char File6[]="./result_compressed.bmp";
	bmp_compress(File5,File6);

	#ifdef DEBUG
	printf("End of main.\n");
	#endif

    return 0;
}

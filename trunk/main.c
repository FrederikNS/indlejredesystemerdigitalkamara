#include <stdio.h>
#include "bmp.h"

int main (int argc, char *argv[]) {
	char File[]="./example24.bmp";
	char File2[]="./result.bmp";
	bmp_ctgc(File,File2);
	
    return 0;
}

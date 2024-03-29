#include "types.h"
#include "compression.h"
#include "compression_test.h"
#include <stdlib.h>
#include <stdio.h>
//#define DEBUG

//Tests compression and decompression.
int test_compression() {
	printf("Testing compression system. 0 for successful testing, anything else failure.\n");
	printf("Test compression: %d\n", test_compress());
	printf("Test decompression: %d\n", test_decompress());
	return 0;
}

//Tests compression.
int test_compress() {

	/*   Test 1: Handle 3*5 image with different values.   */

	//Allocate memory.
	IMAGE *com_image = (IMAGE*) malloc(sizeof(IMAGE));
	IMAGE *decom_image = (IMAGE*) malloc(sizeof(IMAGE));
	if (!com_image || !decom_image) {
		printf("ERROR: Failed to allocate memory to test decompressed image.\n");
		return 1;
	}
	decom_image->Width = 5;
	decom_image->Height = 3;

	decom_image->Pixels[0] = 128;
	decom_image->Pixels[1] = 128;
	decom_image->Pixels[2] = 128;
	decom_image->Pixels[3] = 128;
	decom_image->Pixels[4] = 128;

	decom_image->Pixels[5] = 128;
	decom_image->Pixels[6] = 220;
	decom_image->Pixels[7] = 230;
	decom_image->Pixels[8] = 150;
	decom_image->Pixels[9] = 100;

	decom_image->Pixels[10] = 150;
	decom_image->Pixels[11] = 150;
	decom_image->Pixels[12] = 150;
	decom_image->Pixels[13] = 120;
	decom_image->Pixels[14] = 120;

	//Output image should be: 5 128 0 0 1 128 1 220 1 230 1 150 1 100 0 0 3 150 2 120 0 1
	BYTE desired_image[22];
	desired_image[0] = 5;
	desired_image[1] = 128;
	desired_image[2] = 0;
	desired_image[3] = 0;
	desired_image[4] = 1;
	desired_image[5] = 128;
	desired_image[6] = 1;
	desired_image[7] = 220;
	desired_image[8] = 1;
	desired_image[9] = 230;
	desired_image[10] = 1;
	desired_image[11] = 150;
	desired_image[12] = 1;
	desired_image[13] = 100;
	desired_image[14] = 0;
	desired_image[15] = 0;
	desired_image[16] = 3;
	desired_image[17] = 150;
	desired_image[18] = 2;
	desired_image[19] = 120;
	desired_image[20] = 0;
	desired_image[21] = 1;

	//Check.
	compress(decom_image, com_image);
	int i;
	for (i = 0; i < 22; i++) {
		if (desired_image[i] != com_image->Pixels[i]) {
			//Fail.
			printf("Value of compressed image at index %d is %d\n", i, com_image->Pixels[i]);
			free(com_image);
			free(decom_image);
			return 1;
		}
	}

	/*   Test 2: Handle sequence of the same color of length >255.   */

	decom_image->Width = 300;
	decom_image->Height = 1;

	for (i = 0; i < 300; i++) {
		decom_image->Pixels[i] = 128;
	}

	//Output image should be: 255 128 45 128 0 1
	desired_image[0] = 255;
	desired_image[1] = 128;
	desired_image[2] = 45;
	desired_image[3] = 128;
	desired_image[4] = 0;
	desired_image[5] = 1;

	//Check.
	compress(decom_image, com_image);
	for (i = 0; i < 6; i++) {
		if (desired_image[i] != com_image->Pixels[i]) {
			//Fail.
			printf("Value of compressed image at index %d is %d\n", i, com_image->Pixels[i]);
			free(com_image);
			free(decom_image);
			return 1;
		}
	}

	free(com_image);
	free(decom_image);

	return 0;
}

//Tests decompression.
int test_decompress() {

	//Test 1: Handle simple line.
	//Test 2: Handle different values.
	//Test 3: Handle line shift.

	//Allocate memory.
	IMAGE *decom_image = (IMAGE*) malloc(sizeof(IMAGE));
	IMAGE *com_image = (IMAGE*) malloc(sizeof(IMAGE));
	if (!com_image || !decom_image) {
		printf("ERROR: Failed to allocate memory to test decompressed image.\n");
		return 1;
	}
	com_image->Width = 24;
	com_image->Height = 1;

	#ifdef DEBUG
	printf("Control reaches part before test 1.\n");
	#endif

	/*		Test 1		*/
	//Create a sample with 16 pixels with color 128.
	//Sample looks like this: 16 128 0 1
	//And should be decompressed like this: 128 128 128 128 128 128 128 128...
	com_image->Pixels[0] = 0x10;
	com_image->Pixels[1] = 0x80;
	com_image->Pixels[2] = 0;
	com_image->Pixels[3] = 1;//End of bitmap.

	decompress(com_image, decom_image);

	int i = 0;
	for (i = 0; i < 16; i++) {
		if (decom_image->Pixels[i] != 0x80) {
			//Fail.
			free(com_image);
			free(decom_image);
			return 1;
		}
	}

	#ifdef DEBUG
	printf("Control reaches part before test 2.\n");
	#endif

	/*		Test 2		*/
	//Create a sample with 2 pixels with color 128, 3 pixels with color 255,
	//and 4 pixels with color 128.
	//Sample looks like this: 2 128 3 255 4 128 0 1
	//And should be decompressed like this: 128 128 255 255 255 128 128 128 128
	com_image->Pixels[0] = 0x02;
	com_image->Pixels[1] = 0x80;
	com_image->Pixels[2] = 0x03;
	com_image->Pixels[3] = 0xFF;
	com_image->Pixels[4] = 0x04;
	com_image->Pixels[5] = 0x80;
	com_image->Pixels[6] = 0;
	com_image->Pixels[7] = 1;//End of bitmap.

	decompress(com_image, decom_image);

	for (i = 0; i < 2; i++) {
		if (decom_image->Pixels[i] != 0x80) {
			//Fail.
			printf("Value of decompressed image at %d is %d\n", i, decom_image->Pixels[i]);
			free(com_image);
			free(decom_image);
			return 1;
		}
	}
	for (i = 2; i < 5; i++) {
		if (decom_image->Pixels[i] != 0xFF) {
			//Fail.
			printf("Value of decompressed image at %d is %d\n", i, decom_image->Pixels[i]);
			free(com_image);
			free(decom_image);
			return 1;
		}
	}
	for (i = 5; i < 9; i++) {
		if (decom_image->Pixels[i] != 0x80) {
			//Fail.
			printf("Value of decompressed image at %d is %d\n", i, decom_image->Pixels[i]);
			free(com_image);
			free(decom_image);
			return 1;
		}
	}

	#ifdef DEBUG
	printf("Control reaches part before test 3.\n");
	#endif

	/*		Test 3		*/
	//Create a sample with 3 pixels with color 128, a line break,
	//and 5 pixels with color 128.
	//Sample looks like this: 3 128 0 0 5 128 0 1
	//And should be decompressed like this: 128 128 128 128 128 128 128 128
	com_image->Pixels[0] = 0x03;
	com_image->Pixels[1] = 0x80;
	com_image->Pixels[2] = 0x00;//Line break.
	com_image->Pixels[3] = 0x00;
	com_image->Pixels[4] = 0x05;
	com_image->Pixels[5] = 0x80;
	com_image->Pixels[6] = 0;
	com_image->Pixels[7] = 1;//End of bitmap.

	decompress(com_image, decom_image);

	for (i = 0; i < 7; i++) {
		if (decom_image->Pixels[i] != 0x80) {
			//Fail.
			printf("Value of decompressed image at %d is %d\n", i, decom_image->Pixels[i]);
			free(com_image);
			free(decom_image);
			return 1;
		}
	}

	#ifdef DEBUG
	printf("Control reaches part before cleaning up.\n");
	#endif

	//Clean up.
	free(com_image);
	free(decom_image);

	return 0;
}

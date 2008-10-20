#include "types.h"
#include "compression.h"
#include <stdlib.h>
#include <stdio.h>
//#define DEBUG

WORD size_compressed(IMAGE *image) {

	//Analysing memory need.
	int i, a;
	int count = 0;
	int current_value;
	WORD size = 0;
	for (a = 0; a < image->Height; a++) {
		current_value = image->Pixels[a*image->Width];
		for (i = 1; i < image->Width; i++) {
			//If it is the same value as before.
			if (current_value == image->Pixels[a*image->Width+i]) {
				count++;
				//If the value is the same for more than 255 bytes, another 2 bytes
				//are needed to record the rest.
				if (count > 254) {
					size += 2;
					count = 0;
				}
			}
			else {
				//A new value means 2 more bytes.
				size += 2;
				count = 0;
				current_value = image->Pixels[a*image->Width+i];
			}
		}
	}
	//Apply memory for each end of line and a single end of bitmap,
	//and for the last pixel(s) in each line.
	size += image->Height*4;

	return size;
}

int compress(IMAGE *de_image, IMAGE *co_image) {

	int a, i;
	int count = 0;
	BYTE current_value;

	int index = 0;
	for (a = 0; a < de_image->Height; a++) {
		current_value = de_image->Pixels[a*de_image->Width];
		for (i = 1; i < de_image->Width; i++) {
			//If it is the same value as before.
			if (current_value == de_image->Pixels[a*de_image->Width+i]) {
				count++;
				//If the value is the same for more than 255 bytes, another 2 bytes
				//are needed to record the rest.
				if (count > 254) {
					co_image->Pixels[index] = count;
					co_image->Pixels[index+1] = current_value;
					index += 2;
					count = 0;
				}
			}
			else {
				//A new value means 2 more bytes.
				co_image->Pixels[index] = count+1;
				co_image->Pixels[index+1] = current_value;
				index += 2;
				count = 0;
				current_value = de_image->Pixels[a*de_image->Width+i];
			}
		}
		//Last pixel(s).
		co_image->Pixels[index] = count+1;
		co_image->Pixels[index+1] = current_value;
		index += 2;
		//End of line.
		co_image->Pixels[index] = 0;
		co_image->Pixels[index+1] = 0;
		index += 2;
		count = 0;
	}

	//The last end of line should be replaced by a end of bitmap.
	co_image->Pixels[index-2] = 0;
	co_image->Pixels[index-1] = 1;

	return 0;
}

int decompress(IMAGE * compressed_image, IMAGE * decompressed_image) {

	#ifdef DEBUG
	printf("Control reaches part before decompression.\n");
	#endif

	int index = 0;//index is the index in the compressed image.
	int index2 = 0;//index2 is the index in the decompressed image.
	int i;
	//Prerequest; no delta escapes.
	//As long as the end of bitmap (0x00, 0x01) hasn't been reached.
	while (compressed_image->Pixels[index] != 0x00 || compressed_image->Pixels[index+1] != 0x01) {
		//If the current index isn't an "end of line".
		if (!(compressed_image->Pixels[index] == 0x00 && compressed_image->Pixels[index+1] == 0x00)) {
			for (i = 0; i < compressed_image->Pixels[index]; i++) {
				//Prerequest; the color map index equals gray level.
				//May not be correct, but will work with own images.
				decompressed_image->Pixels[index2+i] = compressed_image->Pixels[index+1];
			}
			index2 += compressed_image->Pixels[index];
			index += 2;
		}
		else {
			//Do practically nothing when handling lines.
			index += 2;
		}
	}

	#ifdef DEBUG
	printf("Control reaches part after decompression.\n");
	#endif

	return 0;
}

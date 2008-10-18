#include "types.h"
#include "compression.h"
#include <stdlib.h>
#include <stdio.h>
//#define DEBUG

int compress(BYTE * decompressed_image, BYTE ** compressed_image, int image_width, int image_height, int * memory_needed) {

	//Analysing memory need.
	int i, a;
	int count = 0;
	int current_value;
	int size = 0;
	for (a = 0; a < image_height; a++) {
		current_value = decompressed_image[a*image_width];
		for (i = 1; i < image_width; i++) {
			//If it is the same value as before.
			if (current_value == decompressed_image[a*image_width+i]) {
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
				current_value = decompressed_image[a*image_width+i];
			}
		}
	}
	//Apply memory for each end of line and a single end of bitmap,
	//and for the last pixel(s) in each line.
	size += image_height*4;

	//Allocate memory.
	*memory_needed = size;
	BYTE* new_image = (BYTE*) malloc(sizeof(char)*size);
	if (!new_image) {
		#ifdef DEBUG
		printf("ERROR: Failed to allocate memory to decompressed image.\n");
		#endif
		return 1;
	}
	*compressed_image = new_image;

	int index = 0;
	count = 0;
	for (a = 0; a < image_height; a++) {
		current_value = decompressed_image[a*image_width];
		for (i = 1; i < image_width; i++) {
			//If it is the same value as before.
			if (current_value == decompressed_image[a*image_width+i]) {
				count++;
				//If the value is the same for more than 255 bytes, another 2 bytes
				//are needed to record the rest.
				if (count > 254) {
					(*compressed_image)[index] = count;
					(*compressed_image)[index+1] = current_value;
					index += 2;
					count = 0;
				}
			}
			else {
				//A new value means 2 more bytes.
				(*compressed_image)[index] = count+1;
				(*compressed_image)[index+1] = current_value;
				index += 2;
				size += 2;
				count = 0;
				current_value = decompressed_image[a*image_width+i];
			}
		}
		//Last pixel(s).
		(*compressed_image)[index] = count+1;
		(*compressed_image)[index+1] = current_value;
		index += 2;
		//End of line.
		(*compressed_image)[index] = 0;
		(*compressed_image)[index+1] = 0;
		index += 2;
		count = 0;
	}

	//The last end of line should be replaced by a end of bitmap.
	(*compressed_image)[index-2] = 0;
	(*compressed_image)[index-1] = 1;

	return 0;
}

int decompress(BYTE * compressed_image, BYTE ** decompressed_image, int image_width, int image_height) {

	//Allocate memory.
	BYTE* new_image = (BYTE*) malloc(sizeof(char)*image_width*image_height);
	if (!new_image) {
		#ifdef DEBUG
		printf("ERROR: Failed to allocate memory to decompressed image.\n");
		#endif
		return 1;
	}
	*decompressed_image = new_image;

	#ifdef DEBUG
	printf("Control reaches part before decompression.\n");
	#endif

	int index = 0;//index is the index in the compressed image.
	int index2 = 0;//index2 is the index in the decompressed image.
	int i;
	//Prerequest; no delta escapes.
	//As long as the end of bitmap (0x00, 0x01) hasn't been reached.
	while (compressed_image[index] != 0x00 || compressed_image[index+1] != 0x01) {
		//If the current index isn't an "end of line".
		if (!(compressed_image[index] == 0x00 && compressed_image[index+1] == 0x00)) {
			for (i = 0; i < compressed_image[index]; i++) {
				//Prerequest; the color map index equals gray level.
				//May not be correct, but will work with own images.
				(*decompressed_image)[index2+i] = compressed_image[index+1];
			}
			index2 += compressed_image[index];
			index += 2;
		}
		else {
			//Prerequest; the image width in bytes is a multiple of 4.
			//There, do practically nothing when handling lines.
			index += 2;
		}
	}

	#ifdef DEBUG
	printf("Control reaches part after decompression.\n");
	#endif

	return 0;
}

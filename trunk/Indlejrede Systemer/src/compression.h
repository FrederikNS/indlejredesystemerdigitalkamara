#include "types.h"

#ifndef COMPRESSION_H_
#define COMPRESSION_H_

//Decompress a bmp image encoded with run length encoding.
int decompress(BYTE * compressed_image, BYTE ** decompressed_image, int image_width, int image_height);

//Compress a bmp image with run length encoding.
int compress(BYTE * decompressed_image, BYTE ** compressed_image, int image_width, int image_height, int * memory_needed);

#endif /* COMPRESSION_H_ */

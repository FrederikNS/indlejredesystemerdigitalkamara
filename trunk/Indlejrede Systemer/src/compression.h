#include "types.h"

#ifndef COMPRESSION_H_
#define COMPRESSION_H_

//Decompress a bmp image encoded with run length encoding.
int decompress(IMAGE *de_image, IMAGE *co_image);

//Compress a bmp image with run length encoding.
int compress(IMAGE *de_image, IMAGE *co_image);

//Get size of image if it was compressed.
WORD size_compressed(IMAGE *image);

#endif /* COMPRESSION_H_ */

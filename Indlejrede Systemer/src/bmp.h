
/********************************************************************************
 * BMP Header
 ********************************************************************************/

#ifndef __BMP_H
#define __BMP_H

#pragma pack(push, 1)

typedef struct {
	HALFWORD 	BfType;  		    /* Must be 0x4D42 				  */
	WORD	 	BfSize;				/* Size of the file in bytes 	  */
	HALFWORD	BfReserved1;		/* Should be 0 					  */
	HALFWORD	BfReserved2;		/* Should be 0 					  */
	WORD		BfOffBits;			/* Offset of image data in file   */
} BITMAPFILEHEADER;

typedef struct {
	WORD		BiSize;				/* Size of this structure 		  */
	WORD		BiWidth;			/* Width of the image in bytes    */
	WORD		BiHeight;			/* Height of the image in bytes   */
	HALFWORD	BiPlanes;			/* Should be 1 					  */
	HALFWORD	BiBitCount;			/* Bit count (..) 				  */
	WORD		BiCompression;		/* Compression used 			  */
	WORD		BiSizeImage;		/* Size of the image in bytes 	  */
	WORD		BiXPelsPerMeter; 	/* Pixels per meter, X			  */
	WORD		BiYPelsPerMeter;	/* Pixels per meter, Y 			  */
	WORD		BiClrUsed;			/* number of colors used 		  */
	WORD		BiClrImportant;		/* number of important colors 	  */
} BITMAPINFOHEADER;

#pragma pack(pop)

int bmp_open(char *File, IMAGE curr_image);

int bmp_gprof(char *File);
int bmp_ctgc(char *File, char *File2);
int bmp_test(char *File);
int bmp_compress(char *File, char *File2);
int bmp_decompress(char *File, char *File2);

/* Open and read the Header of a BMP file */
int bmp_info_reader(char *File, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih);

/* Open and read the image data of a BMP file */
int bmp_image_reader(char *File, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih, IMAGE *image_data);

/* Builds a palette for a 256 grayscale BMP */
int bmp_palette_creator(BYTE *pal);

/* Converts the bmp image data to grayscale */
int bmp_colour_to_grayscale(BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih, BYTE *pixold, BYTE *pixnew);

int bmp_header_builder(IMAGE *curr_image, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih);

int bmp_save_image(char *File, IMAGE *curr_image);

/* Store image to BMP file */
int bmp_save(char *File, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih, BYTE *pixnew, BYTE *pal);

/* Decompress bmp image data */
int decompress(BYTE * compressed_image, BYTE ** decompressed_image, int image_width, int image_height);

#endif /* __BMP_H */

#include <stdio.h>
#include <stdlib.h>
#include "filtering.h"
#include "filtering_test.h"
#include "compression.h"
#include "compression_test.h"
#include "bmp.h"

int bmp_ctgc(char *File, char *File2){
	BITMAPFILEHEADER *bmfh = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
	BYTE *pixold = 0;
	BYTE *pixnew = 0;
	BYTE *pal = 0;
	if(bmfh && bmih){
		bmp_info_reader(File, bmfh, bmih);
		pixold = (BYTE *) malloc(sizeof(BYTE)*bmih->BiSizeImage);
		pixnew = (BYTE *) malloc(sizeof(BYTE)*(bmih->BiSizeImage/3));
		if(!pixold||!pixnew){
			printf("Premature return");
			return 1;
		}
		bmp_image_reader(File, bmfh, bmih, pixold);
		pal = (BYTE *) malloc(sizeof(BYTE)*1024);
		if(!pal){
			printf("Premature return");
			return 1;
		}
		bmp_palette_creator(pal);
		bmp_colour_to_grayscale(bmfh, bmih, pixold, pixnew);


		int image_width = 512, image_height = 512;

		unsigned char *pixfilter = 0;
		filter_image(pixnew, &pixfilter, 1, 3, image_width, image_height);

		//Test compression.
		if (test_compress()) {
			//Fail.
			printf("Testing of compression failed.\n");
			return 1;
		}
		if (test_decompress()) {
			//Fail.
			printf("Testing of decompression failed.\n");
			return 1;
		}

		/*int i;
		for (i = 0; i < 100; i++) {
			filter_image(pixnew, &pixfilter, , 3, image_width, image_height);
			free(pixfilter);
		}*/

		//Test filtering.
		//test_filtering(pixnew, image_width, image_height);

		bmp_save(File2, bmfh, bmih, pixfilter, pal);

		//free(pixfilter);
	}
	free(bmfh);
	free(bmih);
	free(pixold);
	free(pixnew);
	free(pal);
	return 0;
}

int bmp_decompress(char *File, char *File2){
	BITMAPFILEHEADER *bmfh = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
	BYTE *pixold = 0;
	BYTE *pal = 0;
	if(bmfh && bmih){
		bmp_info_reader(File, bmfh, bmih);
		pixold = (BYTE *) malloc(sizeof(BYTE)*bmih->BiSizeImage);
		if(!pixold){
			printf("Premature return");
			return 1;
		}
		bmp_image_reader(File, bmfh, bmih, pixold);
		pal = (BYTE *) malloc(sizeof(BYTE)*1024);
		if(!pal){
			printf("Premature return");
			return 1;
		}
		bmp_palette_creator(pal);

		BYTE * pixdecompressed = 0;
		decompress(pixold, &pixdecompressed, bmih->BiWidth, bmih->BiHeight);

		bmih->BiCompression = 0;
		bmih->BiSizeImage = (bmih->BiHeight)*(bmih->BiWidth);

		bmp_save(File2, bmfh, bmih, pixdecompressed, pal);

	}
	free(bmfh);
	free(bmih);
	free(pixold);
	free(pal);
	return 0;
}

int bmp_compress(char *File, char *File2){
	BITMAPFILEHEADER *bmfh = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
	BYTE *pixold = 0;
	BYTE *pal = 0;
	if(bmfh && bmih){
		bmp_info_reader(File, bmfh, bmih);
		pixold = (BYTE *) malloc(sizeof(BYTE)*bmih->BiSizeImage);
		if(!pixold){
			printf("Premature return");
			return 1;
		}
		bmp_image_reader(File, bmfh, bmih, pixold);
		pal = (BYTE *) malloc(sizeof(BYTE)*1024);
		if(!pal){
			printf("Premature return");
			return 1;
		}
		bmp_palette_creator(pal);

		BYTE * pixdecompressed = 0;
		int size = 0;
		compress(pixold, &pixdecompressed, bmih->BiWidth, bmih->BiHeight, &size);

		bmih->BiCompression = 1;
		bmih->BiSizeImage = size;

		bmp_save(File2, bmfh, bmih, pixdecompressed, pal);

	}
	free(bmfh);
	free(bmih);
	free(pixold);
	free(pal);
	return 0;
}

int bmp_info_reader(char *File, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih) {
	FILE* fp;

	if((fp = fopen(File, "rb")) == NULL) {
		perror("Could not open file");
		return 1;
	}

	fread(bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(bmih, sizeof(BITMAPINFOHEADER), 1, fp);

	fclose(fp);
	return 0;
}

int bmp_image_reader(char *File, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih, BYTE *pixold) {
	FILE* fp;

	if((fp = fopen(File, "rb")) == NULL) {
		perror("Could not open file");
		return 1;
	}

	fseek(fp, bmfh->BfOffBits, SEEK_SET);

	fread(pixold, sizeof(BYTE), bmih->BiSizeImage, fp);

	fclose(fp);
	return 0;
}

int bmp_palette_creator(BYTE *pal) {
	int i;
	int j;

	for(i=0;i<=255;i++) {
		for(j=0;j<3;j++){
			pal[i*4+j]=i;
		}
		pal[i*4+4]=0x00;
	}
	return 0;
}

int bmp_colour_to_grayscale(BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih, BYTE *pixold, BYTE *pixnew) {
	int a;

	for (a = 0; a +2< bmih->BiSizeImage; a+=3) {
		pixnew[a/3] = (int) (0.11*(double)(pixold[a]) + 0.59*(double)(pixold[a+1]) + 0.3*(double)(pixold[a+2]));
	}

	bmfh->BfOffBits = bmfh->BfOffBits+256*4;
	bmih->BiSizeImage /= 3;
	bmih->BiBitCount = 8;
	bmfh->BfSize = bmfh->BfOffBits+bmih->BiSizeImage;

	return 0;
}

int bmp_save(char *File, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih, BYTE *pixnew, BYTE *pal) {
	FILE* fp = fopen(File, "wb");

	if(fp == NULL) {
		perror("Could not open file");
		return 1;
	}

	fwrite(bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(bmih, sizeof(BITMAPINFOHEADER), 1, fp);

	fwrite(pal, sizeof(BYTE), 1024, fp);

	fwrite(pixnew, sizeof(BYTE), bmih->BiSizeImage, fp);

	fclose(fp);
	return 0;
}

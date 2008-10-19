#include <stdio.h>
#include <stdlib.h>
#include "filtering.h"
#include "filtering_test.h"
#include "compression.h"
#include "compression_test.h"
#include "bmp.h"
#include "types.h"

int bmp_open(char *File, IMAGE *current_image) {
	BITMAPFILEHEADER *bmfh = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
	
	if(bmp_info_reader(File, bmfh, bmih) != 0 || bmp_image_reader(File, bmfh, bmih, current_image) != 0)
		return 1;
	return 0;
}

//This function tests filtering and compression.
//When compiling for release, comment this function out,
//the gprof function, and comment out the header files with "test"
//in their names, thereby easily excluding unnecessary source code.
/*int bmp_test(char *File) {
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
 
 //Test filtering.
 test_filtering(pixnew, image_width, image_height);
 
 //Test compression.
 test_compression();
 }
 free(bmfh);
 free(bmih);
 free(pixold);
 free(pixnew);
 free(pal);
 return 0;
 }*/

//Runs time-consuming functions a lot of times
//so gprof can get a good estimate of them.
//They are all run with worst-case input,
//since the worst-case input is the most hindering to the user,
//and the most relevant.
/*int bmp_gprof(char *File) {
 BITMAPFILEHEADER *bmfh = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
 BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
 IMAGE *old_image;
 IMAGE *new_image;
 BYTE *pal = 0;
 if(bmfh && bmih){
 bmp_info_reader(File, bmfh, bmih);
 
 old_image = (IMAGE *) malloc(sizeof(IMAGE));
 new_image = (IMAGE *) malloc(sizeof(IMAGE));
 
 bmp_image_reader(File, bmfh, bmih, old_image);
 pal = (BYTE *) malloc(sizeof(BYTE)*1024);
 
 bmp_palette_creator(pal);
 bmp_colour_to_grayscale(bmfh, bmih, pixold, pixnew);
 
 int image_width = 512, image_height = 512;
 
 //Run filters.
 unsigned char *pixfilter = 0;
 int i;
 //Laplacian.
 for (i = 0; i < 100; i++) {
 filter_image(pixnew, &pixfilter, 0, 3);
 free(pixfilter);
 }
 //Steepness.
 for (i = 0; i < 100; i++) {
 filter_image(pixnew, &pixfilter, 1, 3, image_width, image_height);
 free(pixfilter);
 }
 //Mean (3X3).
 /*for (i = 0; i < 100; i++) {
 filter_image(pixnew, &pixfilter, 2, 3, image_width, image_height);
 free(pixfilter);
 }*//*
  //Mean (5X5).
  for (i = 0; i < 100; i++) {
  filter_image(pixnew, &pixfilter, 2, 5, image_width, image_height);
  free(pixfilter);
  }
  
  BYTE *pixcom = 0;
  BYTE *pixdecom = 0;
  //Compression/decompression.
  int size = 0;
  for (i = 0; i < 100; i++) {
  compress(pixnew, &pixcom, image_width, image_height, &size);
  decompress(pixcom, &pixdecom, image_width, image_height);
  free(pixcom);
  free(pixdecom);
  }
  }
  free(bmfh);
  free(bmih);
  free(pixold);
  free(pixnew);
  free(pal);
  return 0;
  }*/

/*int bmp_ctgc(char *File, char *File2){
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
 filter_image(pixnew, &pixfilter, 1, 3);
 
 return 0;
 }
 }*/

/*int bmp_decompress(char *File, char *File2){
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
 }*/

/*int bmp_compress(char *File, char *File2){
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
 }*/

int bmp_info_reader(char *File, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih) {
	FILE* fp;
	
	if((fp = fopen(File, "rb")) == NULL) {
		perror("Could not open file");
		return 1;
	}
	
	fread(bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(bmih, sizeof(BITMAPINFOHEADER), 1, fp);
	
	fclose(fp);
	
	printf("Survived info reader\n");
	return 0;
}

int bmp_image_reader(char *File, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih, IMAGE *image_data) {
	FILE* fp;
	
	if((fp = fopen(File, "rb")) == NULL) {
		perror("Could not open file");
		return 1;
	}
	
	fseek(fp, bmfh->BfOffBits, SEEK_SET);
	fread(image_data->Pixels, sizeof(char), bmih->BiSizeImage, fp);
	fclose(fp);
	
	image_data->Width=bmih->BiWidth;
	image_data->Height=bmih->BiHeight;
	
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

int bmp_header_builder(IMAGE *curr_image, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih){
	bmfh->BfType = 0x4D42;
	bmfh->BfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*4+curr_image->Width*curr_image->Height;
	bmfh->BfReserved1 = 0;
	bmfh->BfReserved2 = 0;
	bmfh->BfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*4;
	
	bmih->BiSize = sizeof(BITMAPINFOHEADER);
	bmih->BiWidth = curr_image->Width;
	bmih->BiHeight = curr_image->Height;
	bmih->BiPlanes = 1;
	bmih->BiBitCount = 8;
	bmih->BiCompression = 0;
	bmih->BiSizeImage = curr_image->Width*curr_image->Height;
	bmih->BiXPelsPerMeter = 2834;
	bmih->BiYPelsPerMeter = 2834;
	bmih->BiClrUsed = 0;
	bmih->BiClrImportant = 0;
	return 0;
}

int bmp_save_image(char *File, IMAGE *curr_image) {
	BITMAPFILEHEADER *bmfh = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
	BYTE *pal = (BYTE *) malloc(sizeof(BYTE)*256*4);
	
	bmp_header_builder(curr_image, bmfh, bmih);
	
	bmp_palette_creator(pal);
	
	bmp_save(File, bmfh, bmih, curr_image->Pixels, pal);
	
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
	
	fwrite(pal, sizeof(char), 256*4, fp);
	
	fwrite(pixnew, sizeof(char), bmih->BiSizeImage, fp);
	
	fclose(fp);
	return 0;
}

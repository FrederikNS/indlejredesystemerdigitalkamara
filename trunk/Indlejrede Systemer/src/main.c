#include <stdio.h>
#include <stdlib.h>
#include "ccd.h"
#include "lcd.h"
#include "filtering.h"
#include "types.h"

#define DEBUG

#define BUFFER_LENGTH 255

#define MAX_WIDTH 512
#define MAX_HEIGHT 512

typedef struct {
	WORD  Height;
	WORD  Width;
	BYTE  Pixels[MAX_WIDTH * MAX_HEIGHT];	
} IMAGE;

int main (int argc, char *argv[]) {
	char command[BUFFER_LENGTH];
	char filter;
	char edge;
	char stayAlive = 1;
	int i;
	IMAGE *curr_image;
	IMAGE *new_image = (IMAGE *) malloc(sizeof(IMAGE));
	
	/*
	 ||!pixnew
	 pixnew = (BYTE *) malloc(sizeof(BYTE)*(bmih->BiSizeImage/3));
	 bmp_colour_to_grayscale(bmfh, bmih, pixold, pixnew);
	 
	 char * pixfilter = 0;
	 
	 filter_image(pixnew, &pixfilter, 5, 1, 512, 512);
	 
	 bmp_save(File2, bmfh, bmih, pixfilter, pal);
	 */
	
	printf("Welcome to Frederik & Melvin's camera software\n"
		   "\n");
	while (stayAlive) {
		ccd_capture_image();
		curr_image = (IMAGE *) malloc(sizeof(IMAGE));
		for(i = 0;i<ccd_get_height()*ccd_get_width();i++)
			curr_image->Pixels[i] = ccd_get_pixel();
		
		printf("\nDo you want to manipulate the picture? [yes/no]\n");
		while(command != "yes" || command != "no"){
			fgets(command, BUFFER_LENGTH, stdin);
			sscanf(command, "%s", command);
		}
		if(command == "yes") {
			filter = 0;
			printf("\nPlease choose a filter to apply: \n"
				   "1. Low-pass (Noise Remover)\n"
				   "2. High-pass Edge Detection\n"
				   "3. High-pass Line Detection\n"
				   "\n"
				   "Enter a number:\n");
			while(filter <= 1 && filter >= 3) {
				fgets(command, BUFFER_LENGTH, stdin);
				sscanf(command, "%c", filter);
			}
			if(filter >= 1 && filter <= 3) {
				filter_image(curr_image, new_image, filter, edge, ccd_get_width(), ccd_get_height());
				for(i=0;i<ccd_get_height()*ccd_get_width();i++)
					lcd_set_pixel();
			} else {
				
			}
			
			return 0;
			
			/*#ifdef DEBUG
			 printf("Start of main.\n");
			 #endif
			 
			 /*char File[] = "./example24.bmp";
			 bmp_gprof(File);*/
			
			printf("Testing...\n");
			char File_[]="./example24.bmp";
			bmp_test(File_);
			
			/*printf("Image filtering...\n");
			 char File[]="./example24.bmp";
			 char File2[]="./result.bmp";
			 bmp_ctgc(File,File2);
			 
			 printf("Decompression...\n");
			 char File3[]="./example8rle.bmp";
			 char File4[]="./result_uncompressed.bmp";
			 bmp_decompress(File3,File4);
			 
			 printf("Compression...\n");
			 char File5[]="./resultx_-1.bmp";
			 char File6[]="./result_compressed.bmp";
			 bmp_compress(File5,File6);*/
			
#ifdef DEBUG
			printf("End of main.\n");
#endif
			
			return 0;
			>>>>>>> .r14*/
		}

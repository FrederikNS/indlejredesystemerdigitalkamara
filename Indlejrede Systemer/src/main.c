#include <stdio.h>
#include <stdlib.h>
#include "ccd.h"
#include "lcd.h"
#include "filtering.h"
#include "types.h"

#define TEST
#ifdef TEST
	#include "filtering_test.h"
	#include "compression_test.h"
#endif

#define DEBUG

#define BUFFER_LENGTH 255

#define MAX_WIDTH 512
#define MAX_HEIGHT 512

int main (int argc, char *argv[]) {
	char command[BUFFER_LENGTH];
	char filter;
	char stayAlive = 1;
	int i;
	int filter_size;
	IMAGE *curr_image = (IMAGE *) malloc(sizeof(IMAGE));
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
		printf("Test\n");

		for(i = 0;i<ccd_get_height()*ccd_get_width();i++)
			curr_image->Pixels[i] = ccd_get_pixel();

		curr
		test_filtering(curr_image);

		printf("\nDo you want to manipulate the picture? [yes/no]\n");
		while(command != "yes" || command != "no"){
			fgets(command, BUFFER_LENGTH, stdin);
			sscanf(command, "%s", command);
		}
		if(command == "yes") {
			filter = 0;
			printf("\nPlease choose a filter to apply: \n"
				   "1. High-pass Line Detection\n"
				   "2. High-pass Edge Detection\n"
				   "3. Low-pass (Noise Remover)\n"
				   "\n"
				   "Enter a number:\n");
			while(filter <= 1 && filter >= 3) {
				fgets(command, BUFFER_LENGTH, stdin);
				sscanf(command, "%c", filter);
			}
			if(filter >= 1 && filter <= 3) {
				if(filter == 3) {
					printf("How large do you wan't the filter mask to be? (it must be a positive and odd number)");
					filter_size=0;
					while(filter_size > 0 && filter_size % 2 == 1) {
						fgets(command, BUFFER_LENGTH, stdin);
						sscanf(command, "%d", filter_size);
					}
				} else {
					filter_size = 0;
				}
				filter_image(curr_image, new_image, filter-1, filter_size);
				for(i=0;i<ccd_get_height()*ccd_get_width();i++)
					lcd_set_pixel(new_image->Pixels[i]);

			} else {
				for(i=0;i<ccd_get_height()*ccd_get_width();i++)
					lcd_set_pixel(curr_image->Pixels[i]);
			}
			lcd_show_image();
		}
		free(curr_image);
	}
	return 0;
}
/*#ifdef DEBUG
 printf("Start of main.\n");
 #endif

 /*char File[] = "./example24.bmp";
 bmp_gprof(File);*/

/*printf("Testing...\n");
 char File_[]="./example24.bmp";
 bmp_test(File_);*/

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

/*#ifdef DEBUG
 printf("End of main.\n");
 #endif

 return 0;'
 }*/

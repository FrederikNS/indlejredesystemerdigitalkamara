#include <stdio.h>
#include <stdlib.h>
#include "ccd.h"
#include "lcd.h"
#include "filtering.h"
#include "filtering_test.h"
#include "compression.h"
#include "compression_test.h"
#include "types.h"

#define DEBUG
//#define TEST

#define BUFFER_LENGTH 255

#define MAX_WIDTH 512
#define MAX_HEIGHT 512

int main (int argc, char *argv[]) {
	char command[BUFFER_LENGTH];
	char command2 = 0;
	int filter;
	int i;
	int filter_size;

	printf("Welcome to Frederik & Melvin's camera software\n"
			"\n");
	ccd_capture_image();
	IMAGE *curr_image = (IMAGE *) malloc(sizeof(IMAGE));
	for(i = 0;i<ccd_get_height()*ccd_get_width();i++)
		curr_image->Pixels[i] = ccd_get_pixel();

	curr_image->Height=ccd_get_height();
	curr_image->Width=ccd_get_width();

	ccd_free();


#ifdef TEST
	test_filtering(curr_image);
	test_compression();
#endif

	printf("\nDo you want to manipulate the picture? [y/n]\n");
	while(command2 != 121 && command2 != 110){
		fgets(command, BUFFER_LENGTH, stdin);
		command2 = command[0];
	}
	if(command[0] == 'y') {
		filter = 0;
		printf("\nPlease choose a filter to apply: \n"
				"1. High-pass Line Detection\n"
				"2. High-pass Edge Detection\n"
				"3. Low-pass (Noise Remover)\n"
				"\n"
				"Enter a number:\n");
		while(filter < 1 || filter > 3) {
			fgets(command, BUFFER_LENGTH, stdin);
			sscanf(command, "%d", &filter);
		}
		if(filter >= 1 && filter <= 3) {
			if(filter == 3) {
				printf("How large do you wan't the filter mask to be? (it must be a positive and odd number)");
				filter_size=0;
				while(!(filter_size > 0 && filter_size % 2 == 1)) {
					fgets(command, BUFFER_LENGTH, stdin);
					sscanf(command, "%d", &filter_size);
					printf("Filter size: %d\n", filter_size);
				}
			} else {
				filter_size = 0;
			}

			IMAGE *new_image = (IMAGE *) malloc(sizeof(IMAGE));
			filter_image(curr_image, new_image, filter-1, filter_size);
			new_image->Width = curr_image->Width;
			new_image->Height = curr_image->Height;
			free(curr_image);

			lcd_malloc();
			for(i=0;i<new_image->Height*new_image->Width;i++)
				lcd_set_pixel(new_image->Pixels[i]);
			lcd_set_height(new_image->Height);
			lcd_set_width(new_image->Width);
		}
	} else {
		lcd_malloc();
		for(i=0;i<curr_image->Height*curr_image->Width;i++)
			lcd_set_pixel(curr_image->Pixels[i]);
		lcd_set_height(curr_image->Height);
		lcd_set_width(curr_image->Width);
	}
	lcd_show_image();
	return 0;
}

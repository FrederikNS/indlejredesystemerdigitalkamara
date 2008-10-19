
/********************************************************************************
 * LCD
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "bmp.h"

/* todo: add image data structure */

/* image representation compabible with bmp library */
static IMAGE image_to_save;

/* width and height of the image */
static int image_width;
static int image_height;

/* next pixel to write */
static int lcd_pixel_pointer;

void lcd_set_width(int width) {
	/* set desired image width */
	image_width = width;
}

void lcd_set_height(int height) {
	/* set desired image height */
	image_height = height;
}

void lcd_show_image() {
	image_to_save.Width = image_width;
	image_to_save.Height = image_height;
	if(bmp_save_image("output.bmp", &image_to_save)) {
		printf("lcd_show_image(): failed to save file\n");
	}
}

void lcd_reset_pointer() {

	/* reset, ready for new image */
	lcd_pixel_pointer = 0;
}

void lcd_set_pixel(BYTE pixel) {
	image_to_save.Pixels[lcd_pixel_pointer++] = pixel;
}

void lcd_set_pixels(WORD pixels) {

	/* todo: store (four) pixels */

	/* update pixel pointer */
	lcd_pixel_pointer += 4;
}

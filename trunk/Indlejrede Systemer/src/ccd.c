
/*******
 * CCD *
 *******/

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "bmp.h"

static IMAGE *current_image;

WORD ccd_pixel_pointer;

WORD ccd_get_height() {
	return current_image->Height;
}

WORD ccd_get_width() {
	return current_image->Width;
}

void ccd_capture_image() {

	current_image = (IMAGE *) malloc(sizeof(IMAGE));

	if(bmp_open("resultx_-1.bmp", current_image)) {
		printf("ccd_capture_image(): failed to open file\n");
		exit(0);
	}
}

void ccd_reset_pointer() {

	/* reset */
	ccd_pixel_pointer = 0;
}

BYTE ccd_get_pixel() {
	return current_image->Pixels[ccd_pixel_pointer++];
}

WORD ccd_get_pixels() {
	/* update pixel pointer */
	ccd_pixel_pointer += 4;

	return ((WORD)current_image->Pixels[ccd_pixel_pointer])<<8*3 || ((WORD)current_image->Pixels[ccd_pixel_pointer+1])<<8*2 || ((WORD)current_image->Pixels[ccd_pixel_pointer+2])<<8 || ((WORD)current_image->Pixels[ccd_pixel_pointer+4]);
}

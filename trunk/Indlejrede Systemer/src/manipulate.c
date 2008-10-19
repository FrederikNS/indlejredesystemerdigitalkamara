/*
 *  Manipulate.c
 *  Indlejrede Systemer
 *
 *  Created by Frederik Sabroe on 12/10/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "manipulate.h"
#include "bmp.h"
#include "types.h"

#define BUFFER_LENGTH 255

char manipulate() {
	char *command = (char *) malloc(sizeof(char)*BUFFER_LENGTH);
	char *File = (char *) malloc(sizeof(char)*BUFFER_LENGTH);
	char *File2 = (char *) malloc(sizeof(char)*BUFFER_LENGTH);
	char choice;
	char filterLoop;
	BITMAPFILEHEADER *bmfh;
	BITMAPINFOHEADER *bmih;
	BYTE *pixold = 0;
	BYTE *pixnew = 0;
	BYTE *pal = (BYTE *) malloc(sizeof(BYTE)*1024);
	
	printf("Please input a path to a picture you want to work with:\n"
		   "(It will automatically be converted to grayscale)\n");
	
	while (1 /*FILE_COULD_NOT_BE_OPENED*/) {
		fgets(command, BUFFER_LENGTH, stdin);
		sscanf(command, "%s", File);
	}
	bmfh = (BITMAPFILEHEADER *) malloc(sizeof(BITMAPFILEHEADER));
	bmih = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
	bmp_info_reader(File, bmfh, bmih);
	
	pixold = (BYTE *) malloc(sizeof(BYTE)*(bmih->BiSizeImage));
	bmp_image_reader(File, bmfh, bmih, pixold);
	
	if(bmih->BiBitCount == 24) {
		pixnew = (BYTE *) malloc(sizeof(BYTE)*(bmih->BiSizeImage/3));
		bmp_colour_to_grayscale(bmfh, bmih, pixold, pixnew);
	}
	if(bmih->BiBitCount == 8)
		pixnew = (BYTE *) malloc(sizeof(BYTE)*(bmih->BiSizeImage));
	
	
	
	while (filterLoop == 1) {
		printf("\nPlease choose a filter to apply: \n"
			   "1. Low-pass (Noise Remover)\n"
			   "2. High-pass Edge Detection\n"
			   "3. High-pass Line Detection\n"
			   "\n"
			   "Enter a number:\n");
		choice = 0;
		while(choice != 1 || choice != 2 || choice != 3) {
			fgets(command, BUFFER_LENGTH, stdin);
			sscanf(command, "%c", choice);
		}
		
		printf("\nThe filter has been applied, do you want to apply more filters? [yes/no]\n");
		command = NULL;
		while(command != "yes" || command != "no"){
			fgets(command, BUFFER_LENGTH, stdin);
		}
		
		if(command == "yes") {
			break;
		}
		
		printf("\nPlease input a path to where you want the picture saved:\n");
		File2 = NULL;
		while (1 /*FILE_COULD_NOT_BE_OPENED*/) {
			fgets(command, BUFFER_LENGTH, stdin);
			sscanf(command, "%s", File2);
		}
		bmp_palette_creator(pal);
		bmp_save(File2, bmfh, bmih, pixnew, pal);
		
		free(bmfh);
		free(bmih);
		free(pixold);
		free(pixnew);
		free(pal);
		
		choice = 2;
		printf("\nDo you want to quit? [yes/no]\n");
		while(choice != 0 || choice != 1) {
			fgets(command, BUFFER_LENGTH, stdin);
			sscanf(command, "%c", choice);
		}
		if(choice == 1)
			break;
	}
	
	return 0;
}


/*
 char File[]="./example24.bmp";
 char File2[]="./result.bmp";
 bmp_ctgc(File,File2);
 
 printf("I survived...");*/
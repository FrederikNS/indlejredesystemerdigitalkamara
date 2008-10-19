#include "filtering.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STEEPNESS_MAX_VALUE 2550//=(3²*255² + 1²*255²)/255
#define DEBUG

int fold_mean_small(unsigned char * image, unsigned char * image2, int filter_size, int image_width, int image_height);
int fold_mean_width(unsigned char * image, unsigned char * image2, int filter_size, int image_width, int image_height);
int fold_mean_height(unsigned char * image, unsigned char * image2, int filter_size, int image_width, int image_height);
int fold_mean_large(unsigned char * image, unsigned char * image2, int filter_size, int image_width, int image_height);

int filter_image(unsigned char * image, unsigned char ** image2, char folding_type, int filter_size, int image_width, int image_height) {

    //Allocate memory.
	unsigned char* new_image = (unsigned char*) malloc(sizeof(char)*image_width*image_height);
	if (!new_image) {
		#ifdef DEBUG
		printf("ERROR: Failed to allocate memory to filtered image.\n");
		#endif
		return 1;
	}
	*image2 = &new_image[0];

	//Fold.

	int success = 0;

	switch (folding_type) {
		case 0: {
			//A laplacian fold. Detects the change in steepness around each pixel.
			success = fold_laplacian(image, *image2, image_width, image_height);
			break;
		}
		case 1: {
			/*
			A combined horizontal and vertical prewitt filter.
			The value from the horizontal and vertical prewitt filter result in
			an "elevation" vector. By taking the size of that vector, an indication
			of the elevation of the area of the pixel is found.
			In this implementation, the algoritm doesn't compute the length,
			but the length squared. This results in the higher values taking up
			much more of the grayscale than the low values, making them clearer.
			*/
			success = fold_steepness(image, *image2, image_width, image_height);
			break;
		}
		case 2: {
			//A simple mean fold,
			success = fold_mean(image, *image2, filter_size, image_width, image_height);
			break;
		}
		default: {
			#ifdef DEBUG
			printf("ERROR: Selected filter option [%d] does not exist.\n", folding_type);
			#endif
			free(new_image);
			return 1;
		}
	}

	if (success) {
		#ifdef DEBUG
		printf("ERROR: Failed to fold image.\n");
		#endif
		free(new_image);
		return 1;
	}

	return 0;
}

int min(int x, int y) {
    if (x < y) {
        return x;
    }
    return y;
}

int max(int x, int y) {
    if (x > y) {
        return x;
    }
    return y;
}

int fold_mean(unsigned char * image, unsigned char * image2, int filter_size, int image_width, int image_height) {

	/*
	The algorithm works by taking each pixel,
	looking at the area the mean filter operates on,
	and splits the area into 9 types;
	1 inner type, which consists of the actual pixels of the mean filter area,
	4 side types, which consists of the boundary pixels below, above, right or left
	to the image,
	4 corner types, which consists of the remainder of the boundary pixels
	in the mean filter area.
	*/

	if (filter_size < 1 || filter_size % 2 == 0) {
		#ifdef DEBUG
		printf("ERROR: Selected filter size [%d] for mean filter is not valid.\n",
				filter_size);
		#endif
		return 1;
	}

	int filter_area = filter_size*filter_size;
	int i, a, e, o; //i column/x in image, a row/y in image, e column/x in filter,
		//o row/y in filter.

	int sum = 0;//The temporary sum for a pixel.

	int filter_half = (filter_size-1)/2;

	for (i = 0; i < image_width; i++) {
		for (a = 0; a < image_height; a++) {
			//Loop through each pixel in the neighbourhood of the current pixel in the new image,
			//and add it to the temporary sum which is used to determine the current pixels value.

			sum = 0;

			//Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image.
			for (e = -filter_half; e < -i; e++) {
				for (o = -filter_half; o < -a; o++) {//
					sum += image[0];
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					sum += image[(a+o)*image_width];
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					sum += image[image_width*(image_height-1)];
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are inside the image.
			for (e = max(-i, -filter_half); e < min(image_width-i, filter_half+1); e++) {
				for (o = -filter_half; o < -a; o++) {
					sum += image[i+e];
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					sum += image[i+e+(a+o)*image_width];
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					sum += image[image_width*(image_height-1)+i+e];
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image.
			for (e = image_width-i; e < filter_half+1; e++) {
				for (o = -filter_half; o < -a; o++) {
					sum += image[image_width-1];
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					sum += image[image_width-1+(a+o)*image_width];
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					sum += image[image_width*image_height-1];
				}
			}

			image2[i+a*image_width] = sum/filter_area;
		}
	}

	return 0;
}

int fold_laplacian(unsigned char * image, unsigned char * image2, int image_width, int image_height) {

    /*Specialized, optimized laplacian.

	Filter is:
    0   -1   0
    -1   4  -1
    0   -1   0

    Boundary is: even reflection.

    Method used:
    Get the filter values, convert to the interval 0-255, stretch the distribution,
    get valid pixels value (< 0 to 0, >255 to 255).

    Steps:
    Add 4 multiplied with relative pixel (0,0),
    subtract relative pixel (-1, 0), (0, -1), (1, 0), (0, 1),
    linear shift from -4*255_4*255 to 0_255 interval,
    subtract general mean (128),
    stretch distribution (multiply by 10),
    add general mean,
    negative to zero and >255 to 255,
    done.
    The used stretching works, since the vast majority of values on
    regular images are very close to 128.



    The steps is modified for the 8 outer regions of the image;
    the sides and the corners.*/

	int new_value = 0; //The value after stretching,
					   //but possibly with values outside 0-255.
	int i, a; //i is column number (from bottom to top), a is row number.

	int image_width_less = image_width-1;
	int image_height_less = image_height-1;

	//Taking care of the special case in which the image is 1 pixel high xor wide.
	if ((image_height == 1 || image_width == 1) && !(image_width == 1 && image_height == 1)) {

		int length_less;
		if (image_height == 1) {
			length_less = image_width_less;
		}
		else {
			length_less = image_height_less;
		}

		//First pixel
		new_value = (image[0] - image[1])*5/4 + 123;
		if (new_value < 0) {(image2)[0] = 0;}
		else if (new_value > 255) {(image2)[0] = 255;}
		else {(image2)[0] = new_value;}

		//Middle pixels.
		for (i = 1; i < length_less; i++) {
			new_value = (3*image[i]-//"3*" due to reflection.
				(image[i-1]
				+image[i + image_width]
				+image[i+1]))
				*5/4 + 123;

			if (new_value < 0) {(image2)[i] = 0;}
			else if (new_value > 255) {(image2)[i] = 255;}
			else {(image2)[i] = new_value;}
		}

		//Last pixel.
		new_value = (image[image_width-1] - image[image_width-2])*5/4 + 123;
		if (new_value < 0) {(image2)[image_width-1] = 0;}
		else if (new_value > 255) {(image2)[image_width-1] = 255;}
		else {(image2)[image_width-1] = new_value;}
		return 0;
	}
	//Taking care of the special case in which the pixel is exactly 1 pixel high and wide.
	else if (image_width == 1 && image_height == 1) {
		image2[0] = 0;
		return 0;
	}

	//General case (image size min. 2X2).

	//Middle.
	for (i = 1; i < image_width_less; i++) {
		for (a = 1; a < image_height_less; a++) {
			new_value = (4*image[i + image_width*a]-
				(image[i + image_width*(a-1)]
				+image[i-1 + image_width*a]
				+image[i + image_width*(a+1)]
				+image[i+1 + image_width*a]))
				*5/4 + 123;

			if (new_value < 0) {(image2)[i+a*image_width] = 0;}
			else if (new_value > 255) {(image2)[i+a*image_width] = 255;}
			else {(image2)[i+a*image_width] = new_value;}
		}
	}

	//Bottom side.
	for (i = 1; i < image_width_less; i++) {
		new_value = (3*image[i]-//"3*" due to reflection.
			(image[i-1]
			+image[i + image_width]
			+image[i+1]))
			*5/4 + 123;

		if (new_value < 0) {(image2)[i] = 0;}
		else if (new_value > 255) {(image2)[i] = 255;}
		else {(image2)[i] = new_value;}
	}

	//Top side.
	for (i = 1; i < image_width_less; i++) {
		new_value = (3*image[i+image_width*image_height_less]-//"3*" due to reflection.
			(image[i+image_width*image_height_less-1]
			+image[i+image_width*image_height_less+1]
			+image[i+image_width*(image_height_less-1)]))
			*5/4 + 123;

		if (new_value < 0) {(image2)[i+image_width*image_height_less] = 0;}
		else if (new_value > 255) {(image2)[i+image_width*image_height_less] = 255;}
		else {(image2)[i+image_width*image_height_less] = new_value;}
	}

	//Left side.
	for (a = 1; a < image_height_less; a++) {
		new_value = (3*image[image_width*a]-//"3*" due to reflection.
			(image[image_width*(a-1)]
			+image[image_width*(a+1)]
			+image[1 + image_width*a]))
			*5/4 + 123;

		if (new_value < 0) {(image2)[image_width*a] = 0;}
		else if (new_value > 255) {(image2)[image_width*a] = 255;}
		else {(image2)[image_width*a] = new_value;}
	}

	//Right side.
	for (a = 1; a < image_height_less; a++) {
		new_value = (3*image[image_width_less+image_width*a]-//"3*" due to reflection.
			(image[image_width_less+image_width*(a-1)]
			+image[image_width_less+image_width*(a+1)]
			+image[image_width_less+image_width*a-1]))
			*5/4 + 123;

		if (new_value < 0) {(image2)[image_width_less+image_width*a] = 0;}
		else if (new_value > 255) {(image2)[image_width_less+image_width*a] = 255;}
		else {(image2)[image_width_less+image_width*a] = new_value;}
	}

	//Bottom-left corner.
	new_value = (2*image[0]-//"2*" due to reflection.
		(image[1]
		+image[image_width]))
		*5/4 + 123;

	if (new_value < 0) {(image2)[0] = 0;}
	else if (new_value > 255) {(image2)[0] = 255;}
	else {(image2)[0] = new_value;}

	//Bottom-right corner.
	new_value = (2*image[image_width-1]-//"2*" due to reflection.
		(image[2*image_width-1]
		+image[image_width-2]))
		*5/4 + 123;

	if (new_value < 0) {(image2)[image_width-1] = 0;}
	else if (new_value > 255) {(image2)[image_width-1] = 255;}
	else {(image2)[image_width-1] = new_value;}

	//Top-left corner.
	new_value = (2*image[image_width*(image_height-1)]-//"2*" due to reflection.
		(image[image_width*(image_height-1)+1]
		+image[image_width*(image_height-2)]))
		*5/4 + 123;

	if (new_value < 0) {(image2)[image_width*(image_height-1)] = 0;}
	else if (new_value > 255) {(image2)[image_width*(image_height-1)] = 255;}
	else {(image2)[image_width*(image_height-1)] = new_value;}

	//Top-right corner.
	new_value = (2*image[image_width*image_height-1]-//"2*" due to reflection.
		(image[image_width*image_height-2]
		+image[image_width*(image_height-1)-1]))
		*5/4 + 123;

	if (new_value < 0) {(image2)[image_width*image_height-1] = 0;}
	else if (new_value > 255) {(image2)[image_width*image_height-1] = 255;}
	else {(image2)[image_width*image_height-1] = new_value;}

	return 0;

}

int fold_steepness(unsigned char * image, unsigned char * image2, int image_width, int image_height) {

	int sum_horizontal = 0; //Horizontal sum of filter.
	int sum_vertical = 0; //Vertical sum of filter.
	int i, a; //i is column number (from bottom to top), a is row number.

	int image_width_less = image_width-1;
	int image_height_less = image_height-1;

	/*
	Horizontal filter:

	1  0 -1
	1  0 -1
	1  0 -1

	Vertical filter:

	 1  1  1
	 0  0  0
	-1 -1 -1
	*/

	//Handle the special case in which the image is 1 pixel wide and >1 pixel high.
	if (image_width == 1 && image_height != 1) {
		image2[0] = (3*image[1]-3*image[0])*(3*image[1]-3*image[0])/STEEPNESS_MAX_VALUE;
		for (a = 1; a < image_height_less; a++) {
			image2[a] = (3*image[a+1]-3*image[a-1])*(3*image[a+1]-3*image[a-1])
				/STEEPNESS_MAX_VALUE;
		}
		image2[image_height_less] =
			(3*image[image_height_less]-3*image[image_height_less-1])*
			(3*image[image_height_less]-3*image[image_height_less-1])/STEEPNESS_MAX_VALUE;
		return 0;
	}
	//Handle the special case in which the image is >1 pixel wide and 1 pixel high.
	else if (image_width != 1 && image_height == 1) {
		image2[0] = (3*image[0]-3*image[1])*(3*image[0]-3*image[1])/STEEPNESS_MAX_VALUE;
		for (i = 1; i < image_width_less; i++) {
			image2[i] = (3*image[i-1]-3*image[i+1])*(3*image[i-1]-3*image[i+1])
				/STEEPNESS_MAX_VALUE;
		}
		image2[image_width_less] =
			(3*image[image_width_less-1]-3*image[image_width_less])*
			(3*image[image_width_less-1]-3*image[image_width_less])/STEEPNESS_MAX_VALUE;
		return 0;
	}
	//Handle the special case in which the image is 1 pixel wide and 1 pixel high.
	else if (image_width == 1 && image_height == 1) {
		image2[0] = 0;
		return 0;
	}

	//Middle.
	for (i = 1; i < image_width_less; i++) {
		for (a = 1; a < image_height_less; a++) {
			sum_horizontal = (
				image[i + image_width*(a-1)-1]
				+image[i + image_width*a-1]
				+image[i + image_width*(a+1)-1]
			    -(image[i + image_width*(a-1)+1]
				+image[i + image_width*a+1]
				+image[i + image_width*(a+1)+1]));
			sum_vertical = (
				image[i + image_width*(a+1)-1]
				+image[i + image_width*(a+1)]
				+image[i + image_width*(a+1)+1]
				-(image[i + image_width*(a-1)-1]
				+image[i + image_width*(a-1)]
				+image[i + image_width*(a-1)+1]));
			image2[i + image_width*a] = (
					(sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
					/STEEPNESS_MAX_VALUE);
		}
	}

	//Bottom side.
	for (i = 1; i < image_width_less; i++) {
		sum_horizontal = (
			2*image[i -1]//"2*" due to reflection.
			+image[i + image_width-1]
			-(2*image[i + 1]//"2*" due to reflection.
			+image[i + image_width+1]));
		sum_vertical = (
			image[i + image_width-1]
			+image[i + image_width]
			+image[i + image_width+1]
			-(image[i -1]
			+image[i]
			+image[i+1]));

		image2[i] = (
				sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
				/STEEPNESS_MAX_VALUE;
	}

	//Top side.
	for (i = 1; i < image_width_less; i++) {
		sum_horizontal = (
			image[i -1 + image_width*(image_height_less-1)]
			+2*image[i -1 + image_width*image_height_less]//"2*" due to reflection.
			-(image[i +1 + image_width*(image_height_less-1)]
			+2*image[i +1 + image_width*image_height_less]));//"2*" due to reflection.
		sum_vertical = (
			image[i + image_width*image_height_less-1]
			+image[i + image_width*image_height_less]
			+image[i + image_width*image_height_less+1]
			-(image[i+ image_width*(image_height_less-1)-1]
			+image[i+image_width*(image_height_less-1)]
			+image[i+image_width*(image_height_less-1)+1]));

		image2[i+image_width*image_height_less] = (
				sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
				/STEEPNESS_MAX_VALUE;
	}

	//Left side.
	for (a = 1; a < image_height_less; a++) {
		sum_horizontal = (
			image[image_width*(a-1)]
			+image[image_width*a]
			+image[image_width*(a+1)]
			-(image[image_width*(a-1)+1]
			+image[image_width*a+1]
			+image[image_width*(a+1)+1]));
		sum_vertical = (
			2*image[image_width*(a+1)]//"2*" due to reflection.
			+image[image_width*(a+1)+1]
			-(2*image[image_width*(a-1)]//"2*" due to reflection.
			+image[image_width*(a-1)+1]));
		image2[image_width*a] = (
				sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
				/STEEPNESS_MAX_VALUE;
	}

	//Right side.
	for (a = 1; a < image_height_less; a++) {
		sum_horizontal = (
			image[-2+image_width*a]
			+image[-2+image_width*(a+1)]
			+image[-2+image_width*(a+2)]
			-(image[-1+image_width*a]
			+image[-1+image_width*(a+1)]
			+image[-1+image_width*(a+2)]));
		sum_vertical = (
			2*image[-1+image_width*(a+2)]//"2*" due to reflection.
			+image[-2+image_width*(a+2)]
			-(2*image[-1+image_width*a]//"2*" due to reflection.
			+image[-2+image_width*a]));
		image2[image_width*(a+1)-1] = (
				sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
				/STEEPNESS_MAX_VALUE;
	}

	//Bottom-left corner.
	sum_horizontal = (
		2*image[0]//"2*" due to reflection.
		+image[image_width]
		-(2*image[1]//"2*" due to reflection.
		+image[image_width+1]));
	sum_vertical = (
		2*image[image_width]//"2*" due to reflection.
		+image[image_width+1]
		-(2*image[0]//"2*" due to reflection.
		+image[1]));
	image2[0] = (
			sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
			/STEEPNESS_MAX_VALUE;

	//Bottom-right corner.
	sum_horizontal = (
		2*image[image_width-2]//"2*" due to reflection.
		+image[image_width*2-2]
		-(2*image[image_width-1]//"2*" due to reflection.
		+image[image_width*2-1]));
	sum_vertical = (
		2*image[image_width*2-1]//"2*" due to reflection.
		+image[image_width*2-2]
		-(2*image[image_width-1]//"2*" due to reflection.
		+image[image_width-2]));
	image2[image_width-1] = (
			sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
			/STEEPNESS_MAX_VALUE;

	//Top-left corner.
	sum_horizontal = (
		2*image[image_width*image_height_less]//"2*" due to reflection.
		+image[image_width*(image_height_less-1)]
		-(2*image[image_width*image_height_less+1]//"2*" due to reflection.
		+image[image_width*(image_height_less-1)+1]));
	sum_vertical = (
		2*image[image_width*image_height_less]//"2*" due to reflection.
		+image[image_width*image_height_less+1]
		-(2*image[image_width*(image_height_less-1)]//"2*" due to reflection.
		+image[image_width*(image_height_less-1)+1]));
	image2[image_width*image_height_less] = (
			sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
			/STEEPNESS_MAX_VALUE;

	//Top-right corner.
	sum_horizontal = (
		2*image[image_width*image_height-2]//"2*" due to reflection.
		+image[image_width*image_height_less-2]
		-(2*image[image_width*image_height-1]//"2*" due to reflection.
		+image[image_width*image_height_less-1]));
	sum_vertical = (
		2*image[image_width*image_height-1]//"2*" due to reflection.
		+image[image_width*image_height-2]
		-(2*image[image_width*image_height_less-1]//"2*" due to reflection.
		+image[image_width*image_height_less-2]));
	image2[image_width*image_height-1] = (
			sum_horizontal*sum_horizontal+sum_vertical*sum_vertical)
			/STEEPNESS_MAX_VALUE;

	return 0;
}

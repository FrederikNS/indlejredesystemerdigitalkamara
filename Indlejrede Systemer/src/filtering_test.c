#include <stdlib.h>
#include <stdio.h>
#include "filtering.h"

#define STEEPNESS_MAX_VALUE 2550//=(3²*255² + 1²*255²)/255

int test_filtering(unsigned char * pixnew, int image_width, int image_height) {

	/*Test.*/

	printf("Testing 3 filters. 0 for successful testing, anything else failure.\n");

	unsigned char *pixfilter = 0;
	unsigned char *pixtest = 0;
	filter_image(pixnew, &pixfilter, 0, 3, image_width, image_height);
	test_fold_laplacian(pixnew, &pixtest, image_width, image_height);
	int test_result = test_image_equality(pixfilter, pixtest, image_width, image_height);
	free(pixfilter);
	free(pixtest);

	printf("Test laplacian: %d\n", test_result);

	unsigned char *pixfilter2 = 0;
	unsigned char *pixtest2 = 0;
	filter_image(pixnew, &pixfilter2, 1, 13, image_width, image_height);
	test_fold_steepness(pixnew, &pixtest2, image_width, image_height);
	test_result = test_image_equality(pixfilter2, pixtest2, image_width, image_height);
	free(pixfilter2);
	free(pixtest2);

	printf("Test steepness: %d\n", test_result);

	unsigned char *pixfilter3 = 0;
	unsigned char *pixtest3 = 0;
	filter_image(pixnew, &pixfilter3, 2, 3, image_width, image_height);
	test_fold_mean(pixnew, &pixtest3, image_width, image_height, 3);
	test_result = test_image_equality(pixfilter3, pixtest3, image_width, image_height);
	free(pixfilter3);
	free(pixtest3);

	printf("Test mean: %d\n", test_result);

	return 0;

	/*End test.*/
}

int test_fold_laplacian(unsigned char * image, unsigned char ** image2, int image_width, int image_height) {

	//Filter the image
	int i, a, e, o; //i column/x in image, a row/y in image, e column/x in filter, o row/y in filter.
	unsigned char* new_image = (unsigned char*) malloc(sizeof(char)*image_width*image_height);
	if (!new_image) {
		#ifdef DEBUG
		printf("ERROR: Failed to allocate memory to filtered image.\n");
		#endif
		return 1;
	}
	*image2 = &new_image[0];

	int temp_sum = 0;//The temporary sum of the filter times image for a pixel.


	char* filter = (char *) malloc(sizeof(char)*9);

	//Filling out filter to laplacian.
	filter[0] = filter [2] = filter[6] = filter[8] = 0;
	filter[1] = filter[3] = filter[5] = filter[7] = -1;
	filter[4] = 4;

	int new_value = 0;
	int filter_half = 1;
	int filter_width = 3;

	for (i = 0; i < image_width; i++) {
		for (a = 0; a < image_height; a++) {
			//Loop through each pixel in the neighbourhood of the current pixel in the new image,
			//multiply it with the corresponding value in the filter,
			//and add it to the temporary sum which is used to determine the current pixels value.

			temp_sum = 0;

			//Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image.
			for (e = -filter_half; e < -i; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[0]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[(a+o)*image_width]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[image_width*(image_height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are inside the image.
			for (e = max(-i, -filter_half); e < min(image_width-i, filter_half+1); e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[i+e]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[i+e + image_width*(a+o)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[i+e + image_width*(image_height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image.
			for (e = image_width-i; e < filter_half+1; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[image_width-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[image_width*(a+o+1)-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[image_width*image_height-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Take the filters sum, and divide it by the calculated sum.
			new_value = temp_sum*5/4 + 123;

			if (new_value < 0) {(*image2)[i+a*image_width] = 0;}
			else if (new_value > 255) {(*image2)[i+a*image_width] = 255;}
			else {(*image2)[i+a*image_width] = new_value;}
		}
	}

	return 0;
}

int test_fold_steepness(unsigned char * image, unsigned char ** image2, int image_width, int image_height) {

	//Filter the image
	int i, a, e, o; //i column/x in image, a row/y in image, e column/x in filter, o row/y in filter.
	unsigned char* new_image = (unsigned char*) malloc(sizeof(char)*image_width*image_height);
	if (!new_image) {
		#ifdef DEBUG
		printf("ERROR: Failed to allocate memory to filtered image.\n");
		#endif
		return 1;
	}
	*image2 = &new_image[0];

	int temp_sum = 0, temp_sum2;//The temporary sum of the filter times image for a pixel.


	char* filter = (char *) malloc(sizeof(char)*9);
	char* filter2 = (char *) malloc(sizeof(char)*9);

	//Filling out filter to horizontal and vertical prewitt, respectively.
	filter[0] = filter[3] = filter[6] = 1;
	filter[2] = filter[5] = filter[8] = -1;

	filter2[0] = filter2[1] = filter2[2] = 1;
	filter2[6] = filter2[7] = filter2[8] = -1;

	int filter_half = 1;
	int filter_width = 3;

	for (i = 0; i < image_width; i++) {
		for (a = 0; a < image_height; a++) {
			//Loop through each pixel in the neighbourhood of the current pixel in the new image,
			//multiply it with the corresponding value in the filter,
			//and add it to the temporary sum which is used to determine the current pixels value.

			temp_sum = 0;
			temp_sum2 = 0;

			//Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image.
			for (e = -filter_half; e < -i; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[0]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[0]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[(a+o)*image_width]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[(a+o)*image_width]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[image_width*(image_height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[image_width*(image_height-1)]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are inside the image.
			for (e = max(-i, -filter_half); e < min(image_width-i, filter_half+1); e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[i+e]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[i+e]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[i+e + image_width*(a+o)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[i+e + image_width*(a+o)]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[i+e + image_width*(image_height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[i+e + image_width*(image_height-1)]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image.
			for (e = image_width-i; e < filter_half+1; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[image_width-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[image_width-1]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[image_width*(a+o+1)-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[image_width*(a+o+1)-1]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[image_width*image_height-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image[image_width*image_height-1]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Take the filters sum, and divide it by the calculated sum.

			(*image2)[i+a*image_width] = (temp_sum*temp_sum + temp_sum2*temp_sum2)
				/STEEPNESS_MAX_VALUE;
		}
	}

	return 0;
}

int test_fold_mean(unsigned char * image, unsigned char ** image2, int image_width, int image_height, int filter_size) {

	//Filter the image
	int i, a, e, o; //i column/x in image, a row/y in image, e column/x in filter, o row/y in filter.
	unsigned char* new_image = (unsigned char*) malloc(sizeof(char)*image_width*image_height);
	if (!new_image) {
		#ifdef DEBUG
		printf("ERROR: Failed to allocate memory to filtered image.\n");
		#endif
		return 1;
	}
	*image2 = &new_image[0];

	int temp_sum = 0;//The temporary sum of the filter times image for a pixel.


	char* filter = (char *) malloc(sizeof(char)*filter_size*filter_size);

	//Filling out x*x mean filter.
	for (e = 0; e < filter_size*filter_size; e++) {
		filter[e] = 1;
	}

	int filter_half = (filter_size-1)/2;
	int filter_width = filter_size;
	int filter_area = filter_size*filter_size;

	for (i = 0; i < image_width; i++) {
		for (a = 0; a < image_height; a++) {
			//Loop through each pixel in the neighbourhood of the current pixel in the new image,
			//multiply it with the corresponding value in the filter,
			//and add it to the temporary sum which is used to determine the current pixels value.

			temp_sum = 0;

			//Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image.
			for (e = -filter_half; e < -i; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[0]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[(a+o)*image_width]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[image_width*(image_height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are inside the image.
			for (e = max(-i, -filter_half); e < min(image_width-i, filter_half+1); e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[i+e]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[i+e + image_width*(a+o)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[i+e + image_width*(image_height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image.
			for (e = image_width-i; e < filter_half+1; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image[image_width-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image_height-a, filter_half+1); o++) {
					temp_sum += image[image_width*(a+o+1)-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image_height-a; o < filter_half+1; o++) {
					temp_sum += image[image_width*image_height-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Take the filters sum, and divide it by the calculated sum.
			(*image2)[i+a*image_width] = temp_sum/filter_area;
		}
	}

	return 0;
}

int test_image_equality(unsigned char * image, unsigned char * image2, int image_width, int image_height) {

	int i, a;

	for (i = 0; i < image_width; i++) {
		for (a = 0; a < image_height; a++) {
			if (image[i+image_width*a] != image2[i+image_width*a]) {
				printf("First value: %d,  second value: %d,  pos: (%d, %d)\n",
							image[i+image_width*a],
							image2[i+image_width*a],
							i,
							a
						);
				return 1;
			}
		}
	}

	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include "filtering.h"
#include "filtering_test.h"
#include "types.h"

#define STEEPNESS_MAX_VALUE 2550//=(3²*255² + 1²*255²)/255

int test_filtering(IMAGE *old_image) {

	/*Test.*/

	printf("Testing 3 filters. 0 for successful testing, anything else failure.\n");

	IMAGE *test_image = (IMAGE *) malloc(sizeof(IMAGE));
	IMAGE *new_image = (IMAGE *) malloc(sizeof(IMAGE));
	if (!(test_image && new_image)) {
		printf("ERROR: Failed to allocate memory to test filtering\n");
		return 1;
	}

	filter_image(old_image, new_image, 0, 3);
	test_fold_laplacian(old_image, test_image);
	int test_result = test_image_equality(new_image, test_image);
	printf("Test laplacian: %d\n", test_result);

	filter_image(old_image, new_image, 1, 3);
	test_fold_steepness(old_image, test_image);
	test_result = test_image_equality(new_image, test_image);
	printf("Test steepness: %d\n", test_result);

	filter_image(old_image, new_image, 2, 3);
	test_fold_mean(old_image, test_image, 3);
	test_result = test_image_equality(new_image, test_image);
	printf("Test mean: %d\n", test_result);

	free(test_image);
	free(new_image);

	return 0;

	/*End test.*/
}

int test_fold_laplacian(IMAGE * image, IMAGE * image2) {

	//Filter the image->Pixels
	int i, a, e, o; //i column/x in image->Pixels, a row/y in image->Pixels, e column/x in filter, o row/y in filter.
	int temp_sum = 0;//The temporary sum of the filter times image->Pixels for a pixel.

	char* filter = (char *) malloc(sizeof(char)*9);

	//Filling out filter to laplacian.
	filter[0] = filter [2] = filter[6] = filter[8] = 0;
	filter[1] = filter[3] = filter[5] = filter[7] = -1;
	filter[4] = 4;

	int new_value = 0;
	int filter_half = 1;
	int filter_width = 3;

	for (i = 0; i < image->Width; i++) {
		for (a = 0; a < image->Height; a++) {
			//Loop through each pixel in the neighbourhood of the current pixel in the new image->Pixels,
			//multiply it with the corresponding value in the filter,
			//and add it to the temporary sum which is used to determine the current pixels value.

			temp_sum = 0;

			//Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image->Pixels.
			for (e = -filter_half; e < -i; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[0]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[(a+o)*image->Width]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image->Height-a; o < filter_half+1; o++) {
					temp_sum += image->Pixels[image->Width*(image->Height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are inside the image->Pixels.
			for (e = max(-i, -filter_half); e < min(image->Width-i, filter_half+1); e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[i+e]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[i+e + image->Width*(a+o)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image->Height-a; o < filter_half+1; o++) {
					temp_sum += image->Pixels[i+e + image->Width*(image->Height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image->Pixels.
			for (e = image->Width-i; e < filter_half+1; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[image->Width-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[image->Width*(a+o+1)-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image->Height-a; o < filter_half+1; o++) {
					temp_sum += image->Pixels[image->Width*image->Height-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Take the filters sum, and divide it by the calculated sum.
			new_value = temp_sum*5/4 + 123;

			if (new_value < 0) {(image2->Pixels)[i+a*image->Width] = 0;}
			else if (new_value > 255) {(image2->Pixels)[i+a*image->Width] = 255;}
			else {(image2->Pixels)[i+a*image->Width] = new_value;}
		}
	}

	return 0;
}

int test_fold_steepness(IMAGE * image, IMAGE * image2) {

	//Filter the image
	int i, a, e, o; //i column/x in image, a row/y in image, e column/x in filter, o row/y in filter.
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

	for (i = 0; i < image->Width; i++) {
		for (a = 0; a < image->Height; a++) {
			//Loop through each pixel in the neighbourhood of the current pixel in the new image,
			//multiply it with the corresponding value in the filter,
			//and add it to the temporary sum which is used to determine the current pixels value.

			temp_sum = 0;
			temp_sum2 = 0;

			//Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image.
			for (e = -filter_half; e < -i; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[0]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image->Pixels[0]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[(a+o)*image->Width]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image->Pixels[(a+o)*image->Width]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image->Height-a; o < filter_half+1; o++) {
					temp_sum += image->Pixels[image->Width*(image->Height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image->Pixels[image->Width*(image->Height-1)]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are inside the image.
			for (e = max(-i, -filter_half); e < min(image->Width-i, filter_half+1); e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[i+e]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image->Pixels[i+e]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[i+e + image->Width*(a+o)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image->Pixels[i+e + image->Width*(a+o)]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image->Height-a; o < filter_half+1; o++) {
					temp_sum += image->Pixels[i+e + image->Width*(image->Height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image->Pixels[i+e + image->Width*(image->Height-1)]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image.
			for (e = image->Width-i; e < filter_half+1; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[image->Width-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image->Pixels[image->Width-1]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[image->Width*(a+o+1)-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
					temp_sum2 += image->Pixels[image->Width*image->Height-1]*(filter2[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Take the filters sum, and divide it by the calculated sum.

			image2->Pixels[i+a*image->Width] = (temp_sum*temp_sum + temp_sum2*temp_sum2)
				/STEEPNESS_MAX_VALUE;
		}
	}

	return 0;
}

int test_fold_mean(IMAGE * image, IMAGE * image2, int filter_size) {

	//Filter the image-Pixels
	int i, a, e, o; //i column/x in image-Pixels, a row/y in image-Pixels, e column/x in filter, o row/y in filter.
	int temp_sum = 0;//The temporary sum of the filter times image-Pixels for a pixel.


	char* filter = (char *) malloc(sizeof(char)*filter_size*filter_size);

	//Filling out x*x mean filter.
	for (e = 0; e < filter_size*filter_size; e++) {
		filter[e] = 1;
	}

	int filter_half = (filter_size-1)/2;
	int filter_width = filter_size;
	int filter_area = filter_size*filter_size;

	for (i = 0; i < image->Width; i++) {
		for (a = 0; a < image->Height; a++) {
			//Loop through each pixel in the neighbourhood of the current pixel in the new image-Pixels,
			//multiply it with the corresponding value in the filter,
			//and add it to the temporary sum which is used to determine the current pixels value.

			temp_sum = 0;

			//Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image-Pixels.
			for (e = -filter_half; e < -i; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[0]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[(a+o)*image->Width]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image->Height-a; o < filter_half+1; o++) {
					temp_sum += image->Pixels[image->Width*(image->Height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are inside the image-Pixels.
			for (e = max(-i, -filter_half); e < min(image->Width-i, filter_half+1); e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[i+e]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[i+e + image->Width*(a+o)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image->Height-a; o < filter_half+1; o++) {
					temp_sum += image->Pixels[i+e + image->Width*(image->Height-1)]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image-Pixels.
			for (e = image->Width-i; e < filter_half+1; e++) {
				for (o = -filter_half; o < -a; o++) {
					temp_sum += image->Pixels[image->Width-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = max(-a, -filter_half); o < min(image->Height-a, filter_half+1); o++) {
					temp_sum += image->Pixels[image->Width*(a+o+1)-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
				for (o = image->Height-a; o < filter_half+1; o++) {
					temp_sum += image->Pixels[image->Width*image->Height-1]*(filter[(e+filter_half)+filter_width*(o+filter_half)]);
				}
			}

			//Take the filters sum, and divide it by the calculated sum.
			(image2->Pixels)[i+a*image->Width] = temp_sum/filter_area;
		}
	}

	return 0;
}

int test_image_equality(IMAGE * image, IMAGE * image2) {

	int i, a;

	for (i = 0; i < image->Width; i++) {
		for (a = 0; a < image->Height; a++) {
			if (image->Pixels[i+image->Width*a] != image2->Pixels[i+image->Width*a]) {
				printf("First value: %d,  second value: %d,  pos: (%d, %d)\n",
							image->Pixels[i+image->Width*a],
							image2->Pixels[i+image->Width*a],
							i,
							a
						);
				return 1;
			}
		}
	}

	return 0;
}

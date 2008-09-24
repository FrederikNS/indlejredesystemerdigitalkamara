#include "filtering.h"
#include <stdio.h>
#include <stdlib.h>

int filter_image(char * image, char ** image2, char filter_type, char boundary_setting, int image_width, int image_height) {

    //TODO : Need to find a proper way to get info about image width and height.

    char * filter = 0;
    int filter_width = 0;

    if (filter_get(filter_type, &filter, &filter_width) != 0) {
        #ifdef DEBUG
        printf("ERROR: Setting up filter failed.\n");
        #endif

        return 1;
    }

    #ifdef DEBUG
    printf("Filter-type chosen successfully.\n");
    #endif

    //Calculate filter sum.
    //TODO: Discuss how filter sum should be calculated.

    int i;
    double sum = 0;
    for (i = 0; i < filter_width*filter_width; i++) {
        sum = sum+abs(filter[i]);
        printf("value: %d.   sum:%f\n", abs(filter[i]), sum);
    }

    printf("\nSum is: %f\n", sum);

    //Calculate filter half_length:
    int filter_half_length = (filter_width-1)/2;


    /*Select boundary and perform filtering*/


    if (boundary_setting == 0) {//If boundary is set to valid:

        //Check for filter dimension vs. image dimension.
        //If image in any dimension is smaller than the filter,
        //the filter cannot be applid.
        if (filter_width > image_width || filter_width > image_height) {
            #ifdef DEBUG
            printf("ERROR: Selected filters dimension doesn't fit with the images dimension using the \"valid\" boundary setting.\n");
            #endif
            return 1;
        }

        //Filter the image
        int a, e, o; //i column/x in image, a row/y in image, e column/x in filter, o row/y in filter.
        char* new_image = (char*) malloc(sizeof(char)*(image_width-filter_half_length*2)*(image_height-filter_half_length*2));
        if (!new_image) {
            #ifdef DEBUG
            printf("ERROR: Failed to allocate memory to filtered image.\n");
            #endif
            return 1;
        }
        *image2 = &new_image[0];

        int temp_sum = 0;//The temporary sum of the filter times image for a pixel.
        double temp_sum2 = 0.0;//The final value for the pixel in the new image, calculated from temp_sum
                                //and sum, the value of the filter.
        //Loop through each item in the new image and find the new pixel value.
        for (i = 0+filter_half_length; i < image_width-filter_half_length; i++) {
            for (a = 0+filter_half_length; a < image_height-filter_half_length; a++) {
                //Loop through each pixel in the neighbourhood of the current pixel in the new image,
                //multiply it with the corresponding value in the filter,
                //and add it to the temporary sum which is used to determine the current pixels value.
                temp_sum = 0;
                for (e = -filter_half_length; e < filter_half_length+1; e++) {
                    for (o = -filter_half_length; o < filter_half_length+1; o++) {
                        temp_sum += image[i+e + image_width*(a+o)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                }
                //printf("\n%d", temp_sum);
                temp_sum2 = ((double)temp_sum)/sum;//Take the filters sum, and divide it by the calculated sum.
                (*image2)[i-filter_half_length+((a)-filter_half_length)*(image_width-filter_half_length*2)] = temp_sum2;
            }
            //printf("i: %d\n", i);
        }
    }

    else if (boundary_setting == 1) {//If boundary is set to fixed with constant value 0.


        //Filter the image
        int a, e, o; //i column/x in image, a row/y in image, e column/x in filter, o row/y in filter.

        //printf("I survived_5_0, image_width: %d,  image_height%d\nSize:%d", image_width, image_height, sizeof(char)*image_width*image_height);
        int size = sizeof(char)*image_width*image_height;
        printf("I survived_5_2:\n%d\n", size);
        char* new_image = (char*) malloc(size);
        if(!new_image) {
            printf("no image\n");
        }
        printf("I survived_5_3");

        if (!new_image) {
            #ifdef DEBUG
            printf("ERROR: Failed to allocate memory to filtered image.\n");
            #endif
            return 1;
        }
        *image2 = new_image;

        printf("I survived_5_5");


        int temp_sum = 0;//The temporary sum of the filter times image for a pixel.
        double temp_sum2 = 0.0;//The final value for the pixel in the new image, calculated from temp_sum
                                //and sum, the value of the filter.

        /*   This boundary handling was developed from boundary setting 2,
             the difference is that a lot of unnecessary loops have been removed.   */

        for (i = 0; i < image_width; i++) {
            for (a = 0; a < image_height; a++) {
                //Loop through each pixel in the neighbourhood of the current pixel in the new image,
                //multiply it with the corresponding value in the filter,
                //and add it to the temporary sum which is used to determine the current pixels value.

                temp_sum = 0;

                //Loop for the horizontal part of the pixel-filter area which are inside the image.
                for (e = max(-i, -filter_half_length); e < min(image_width-i, filter_half_length+1); e++) {

                    for (o = max(-a, -filter_half_length); o < min(image_height-a, filter_half_length+1); o++) {
                        temp_sum += image[i+e + image_width*(a+o)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                        if (a == 8 && i < 2) {
                            printf("Sum is: %d, o is: %d \n", temp_sum, o);
                        }
                    }
                    //printf("\nSum: %d", temp_sum);
                }

                //printf("\n%d", temp_sum);
                temp_sum2 = ((double)temp_sum)/sum;//Take the filters sum, and divide it by the calculated sum.
                (*image2)[i+a*image_width] = temp_sum2;
            }
            //printf("i: %d\n", i);
        }
    }

    else if (boundary_setting == 2) {//If boundary is set to fixed with constant value 50.

        int boundary_value = 50;

        //Filter the image
        int a, e, o; //i column/x in image, a row/y in image, e column/x in filter, o row/y in filter.
        char* new_image = (char*) malloc(sizeof(char)*image_width*image_height);
        if (!new_image) {
            #ifdef DEBUG
            printf("ERROR: Failed to allocate memory to filtered image.\n");
            #endif
            return 1;
        }
        *image2 = &new_image[0];

        int temp_sum = 0;//The temporary sum of the filter times image for a pixel.
        double temp_sum2 = 0.0;//The final value for the pixel in the new image, calculated from temp_sum
                                //and sum, the value of the filter.

        for (i = 0; i < image_width; i++) {
            for (a = 0; a < image_height; a++) {
                //Loop through each pixel in the neighbourhood of the current pixel in the new image,
                //multiply it with the corresponding value in the filter,
                //and add it to the temporary sum which is used to determine the current pixels value.

                temp_sum = 0;

                //Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image.
                for (e = -filter_half_length; e < -i; e++) {
                    for (o = -filter_half_length; o < -a; o++) {
                        temp_sum += boundary_value*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    for (o = max(-a, -filter_half_length); o < min(image_height-a, filter_half_length+1); o++) {
                        temp_sum += boundary_value*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    for (o = image_height-a; o < filter_half_length+1; o++) {
                        temp_sum += boundary_value*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                }

                //Loop for the horizontal part of the pixel-filter area which are inside the image.
                for (e = max(-i, -filter_half_length); e < min(image_width-i, filter_half_length+1); e++) {
                    for (o = -filter_half_length; o < -a; o++) {
                        temp_sum += boundary_value*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                    for (o = max(-a, -filter_half_length); o < min(image_height-a, filter_half_length+1); o++) {
                        temp_sum += image[i+e + image_width*(a+o)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                        if (a == 8 && i < 2) {
                            printf("Sum is: %d, o is: %d \n", temp_sum, o);
                        }
                    }
                    //printf("\nSum: %d", temp_sum);
                    for (o = image_height-a; o < filter_half_length+1; o++) {
                        temp_sum += boundary_value*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                }

                //Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image.
                for (e = image_width-i; e < filter_half_length+1; e++) {
                    for (o = -filter_half_length; o < -a; o++) {
                        temp_sum += boundary_value*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    for (o = max(-a, -filter_half_length); o < min(image_height-a, filter_half_length+1); o++) {
                        temp_sum += boundary_value*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    for (o = image_height-a; o < filter_half_length+1; o++) {
                        temp_sum += boundary_value*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                }

                //printf("\n%d", temp_sum);
                temp_sum2 = ((double)temp_sum)/sum;//Take the filters sum, and divide it by the calculated sum.
                (*image2)[i+a*image_width] = temp_sum2;
            }
            //printf("i: %d\n", i);
        }

    }

    else if (boundary_setting == 3) {//If boundary is set to periodic.

        //Filter the image
        int a, e, o; //i column/x in image, a row/y in image, e column/x in filter, o row/y in filter.
        char* new_image = (char*) malloc(sizeof(char)*image_width*image_height);
        if (!new_image) {
            #ifdef DEBUG
            printf("ERROR: Failed to allocate memory to filtered image.\n");
            #endif
            return 1;
        }
        *image2 = &new_image[0];

        int temp_sum = 0;//The temporary sum of the filter times image for a pixel.
        double temp_sum2 = 0.0;//The final value for the pixel in the new image, calculated from temp_sum
                                //and sum, the value of the filter.

        for (i = 0; i < image_width; i++) {
            for (a = 0; a < image_height; a++) {
                //Loop through each pixel in the neighbourhood of the current pixel in the new image,
                //multiply it with the corresponding value in the filter,
                //and add it to the temporary sum which is used to determine the current pixels value.

                temp_sum = 0;

                //Loop for the horizontal part of the pixel-filter area which are outside and on the left of the image.
                for (e = -filter_half_length; e < -i; e++) {
                    for (o = -filter_half_length; o < -a; o++) {
                        temp_sum += image[mod((i+e), image_width) + image_width*mod((a+o), (image_height))]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    for (o = max(-a, -filter_half_length); o < min(image_height-a, filter_half_length+1); o++) {
                        temp_sum += image[mod((i+e), image_width) + image_width*(a+o)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    for (o = image_height-a; o < filter_half_length+1; o++) {
                        temp_sum += image[mod((i+e), image_width) + image_width*((a+o)%image_height)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                }

                //Loop for the horizontal part of the pixel-filter area which are inside the image.
                for (e = max(-i, -filter_half_length); e < min(image_width-i, filter_half_length+1); e++) {
                    for (o = -filter_half_length; o < -a; o++) {
                        temp_sum += image[i+e + image_width*mod((a+o), (image_height))]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                    for (o = max(-a, -filter_half_length); o < min(image_height-a, filter_half_length+1); o++) {
                        temp_sum += image[i+e + image_width*(a+o)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                    for (o = image_height-a; o < filter_half_length+1; o++) {
                        temp_sum += image[i+e + image_width*((a+o)%image_height)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                }

                //Loop for the horizontal part of the pixel-filter area which are outside and on the right of the image.
                for (e = image_width-i; e < filter_half_length+1; e++) {
                    for (o = -filter_half_length; o < -a; o++) {
                        temp_sum += image[((i+e) % image_width) + image_width*((a+o)%image_height)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    for (o = max(-a, -filter_half_length); o < min(image_height-a, filter_half_length+1); o++) {
                        temp_sum += image[((i+e) % image_width) + image_width*(a+o)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    for (o = image_height-a; o < filter_half_length+1; o++) {
                        temp_sum += image[((i+e) % image_width) + image_width*((a+o)%image_height)]*(filter[(e+filter_half_length)+filter_width*(o+filter_half_length)]);
                    }
                    //printf("\nSum: %d", temp_sum);
                }

                //printf("\n%d", temp_sum);
                temp_sum2 = ((double)temp_sum)/sum;//Take the filters sum, and divide it by the calculated sum.
                (*image2)[i+a*image_width] = temp_sum2;
            }
            //printf("i: %d\n", i);
        }
    }

    free(filter);

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

//Must have positive y.
int mod(int x, int y) {
    while (x < 0) {
        x += y;
    }
    return (x % y);
}

int filter_get(char filter_type, char ** filter, int * filter_width) {

    char* f = (char *) malloc(sizeof(char)*9);

    if (!f) {
        #ifdef DEBUG
        printf("ERROR: Tried to allocate 9 bytes memory to filter.\n");
        #endif
        return 1;
    }

    if (filter_type == 0) {//9-element low-pass mean 3X3 filter.

        int i;
        for (i = 0; i < 9; i++) {
            f[i] = 1;
        }

        *filter = &f[0];
        *filter_width = 3;

        return 0;
    }
    else if (filter_type == 1) {//9-element low-pass weighted mean 3X3 filter.

        f[0] = 1; f[1] = 2; f[2] = 1;
        f[3] = 2; f[4] = 3; f[5] = 2;
        f[6] = 1; f[7] = 2; f[8] = 1;

        *filter = &f[0];
        *filter_width = 3;

        return 0;
    }
    else if (filter_type == 2) {//Prewitt high-pass horizontal edge detection 3X3 filter.

        f[0] = f[3] = f[6] = -1;
        f[1] = f[4] = f[7] = 0;
        f[2] = f[5] = f[8] = 1;

        *filter = &f[0];
        *filter_width = 3;
        /*for (i = 0; i < 9; i++) {
            printf("%d ", (*filter)[i]);
        }*/

        return 0;
    }
    else if (filter_type == 3) {//Prewitt high-pass vertical edge detection 3X3 filter.

        f[0] = f[1] = f[2] = -1;
        f[3] = f[4] = f[5] = 0;
        f[6] = f[7] = f[8] = 1;

        *filter = &f[0];
        *filter_width = 3;

        return 0;
    }
    else if (filter_type == 4) {//Prewitt high-pass vertical edge detection 3X3 filter.

        f[0] = f[1] = f[2] = -1;
        f[3] = f[4] = f[5] = 0;
        f[6] = f[7] = f[8] = 1;

        *filter = &f[0];
        *filter_width = 3;

        return 0;
    }
    else if (filter_type == 5) {//Negative Laplacian high-pass edge and line detection 3X3 filter.

        f[0] = 0; f[1] = -1; f[2] = 0;
        f[3] = -1; f[4] = 4; f[5] = -1;
        f[6] = 0; f[7] = -1; f[8] = 0;

        *filter = &f[0];
        *filter_width = 3;

        return 0;
    }
    else if (filter_type == 6) {//25-element low-pass mean 5X5 filter.

        free(f);

        //Allocate memory

        char* f = (char *) malloc(sizeof(char)*25);

        if (!f) {
            #ifdef DEBUG
            printf("ERROR: Tried to allocate 25 bytes memory to filter.\n");
            #endif

            return 1;
        }

        int i;
        for (i = 0; i < 25; i++) {
            f[i] = 1;
        }

        *filter = &f[0];
        *filter_width = 5;

        return 0;
    }

    return 1;
}

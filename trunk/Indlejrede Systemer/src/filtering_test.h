/*
 * filtering_test.h
 *
 *  Created on: Oct 17, 2008
 *      Author: melvin
 */

#ifndef FILTERING_TEST_H_
#define FILTERING_TEST_H_

//These functions have been implemented by an flexible, but slow, folding implementation.
//test_image_equality simply checks if the 2 images are, in fact, identical.
int test_filtering(IMAGE *pix);
int test_fold_laplacian(unsigned char * image, unsigned char ** image2, int image_width, int image_height);
int test_fold_steepness(unsigned char * image, unsigned char ** image2, int image_width, int image_height);
int test_fold_mean(unsigned char * image, unsigned char ** image2, int image_width, int image_height, int filter_size);
int test_image_equality(unsigned char * image, unsigned char * image2, int image_width, int image_height);

#endif /* FILTERING_TEST_H_ */

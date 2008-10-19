/*
 * filtering_test.h
 *
 *  Created on: Oct 17, 2008
 *      Author: melvin
 */

#ifndef FILTERING_TEST_H_
#define FILTERING_TEST_H_

#include "types.h"

//These functions have been implemented by an flexible, but slow, folding implementation.
//test_image_equality simply checks if the 2 images are, in fact, identical.
int test_filtering(IMAGE *pix);
int test_fold_laplacian(IMAGE * image, IMAGE * image2);
int test_fold_steepness(IMAGE * image, IMAGE * image2);
int test_fold_mean(IMAGE * image, IMAGE * image2, int filter_size);
int test_image_equality(IMAGE * image, IMAGE * image2);

#endif /* FILTERING_TEST_H_ */

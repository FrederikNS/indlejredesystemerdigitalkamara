
/*Filter the image with 3 possible folding types:
0 for laplacian, 1 for steepness, 2 for mean.
The fourth argument indicates the size of folding type 2 (the size must be uneven and >0).*/
int filter_image(unsigned char * image, unsigned char ** image2, char folding_type, int filter_size, int image_width, int image_height);

//Used by filter_image to fold a laplacian filter with closest-pixel boundary.
int fold_laplacian(unsigned char * image, unsigned char * image2, int image_width, int image_height);

/*Used by filter_image to filter an image for steepness with closest-pixel boundary.
The steepness is calculated from applying horizontal and vertical prewitt filters.*/
int fold_steepness(unsigned char * image, unsigned char * image2, int image_width, int image_height);

//Used by filter_image to fold a mean filter with closest-pixel boundary.
int fold_mean(unsigned char * image, unsigned char * image2, int filter_size, int image_width, int image_height);
int min(int x, int y);
int max(int x, int y);

#ifndef IMAGE_MANIP_H
#define IMAGE_MANIP_H

#include "ppm_io.h"

// store PI as a constant
#define PI 3.14159265358979323846

// macro to square a number
#define sq(X) ((X) * (X))

// macro to find the max of a number
#define MAX(a,b) ((a > b) ? (a) : (b))


/* HELPER for binarize:
 * convert a RGB pixel to a single grayscale intensity;
 * uses NTSC standard conversion
 */
unsigned char pixel_to_gray (const Pixel *p);


//______binarize___
/* convert image to black and white only based on threshold value
 */
int binarize(Image * img1, FILE * new_img, float thrshld);


//______crop___
/* crop the image given two corner pixel locations
 */
int crop(Image * img1, FILE * new_image, int upper_col, int upper_row,
         int lower_col, int lower_row);


//_____zoom_in___
/* "zoom in" an image, by duplicating each pixel into a 2x2 square of pixels
 */
int zoom_in(Image * img1, FILE * new_image);


//___rotate_left___
/* rotate the image 90 degrees to the left (counter-clockwise)
 */
int rotate_left(Image * img1, FILE * new_image);


//___pointilism___
/* apply painting-like pointilism technique to image
 */
int pointilism(Image * img1, FILE * new_image);


//___blur___
/* apply a blurring filter to the image
 */
int blur(Image * img1, FILE * new_image, float sigma);


#endif

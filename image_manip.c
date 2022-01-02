#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "image_manip.h"
#include "ppm_io.h"



unsigned char pixel_to_gray (const Pixel *p) {
    return (unsigned char)( (0.3 * (double)p->r) +
                            (0.59 * (double)p->g) +
                            (0.11 * (double)p->b) );
}


int binarize(Image * img1, FILE * new_img, float thrshld) {

    // Checks that threshold is valid
    if (thrshld < 0 || thrshld > 255) {
        return -1;
    }

    // Converts threshold to int
    int threshold = (int)(thrshld);

    // Traverses through array of pixels and changes rgb values accordingly
    for (int i = 0; i < img1->rows; i++) {
        for (int j = 0; j < img1->cols; j++) {

            // Flattens data array to 1d, of length num_rows * num_cols
            int k = i*img1->cols + j;
            int grayscale = pixel_to_gray(img1->data + k);

	        // Converts pixel color to either black or white
            if (grayscale < threshold) {
                img1->data[k].r = 0;
                img1->data[k].g = 0;
                img1->data[k].b = 0;
            } else {
                img1->data[k].r = 255;
                img1->data[k].g = 255;
                img1->data[k].b = 255;
            }

        }
    }

    // Writes binarized image
    int result = write_ppm(new_img, img1);
    return result;

}



int crop(Image * img1, FILE * new_image, int upper_col, int upper_row,
            int lower_col, int lower_row) {   
    Image * img2 = malloc(sizeof(Image));

    // Checks that boundary inputs are in bounds
    if (lower_row > img1->rows || lower_col > img1->cols ||
            upper_row < 0 || upper_col < 0) {
        free_image(&img2);
        return -1;
    }

    // Checks that the bounds make sense in relationship to each other
    if (lower_col <= upper_col || lower_row <= upper_row) {
        free_image(&img2);
        return -1;
    }

    // Gets dimensions/space for cropped image
    img2->rows = lower_row - upper_row;
    img2->cols = lower_col - upper_col;
    img2->data = malloc(img2->rows * img2->cols * sizeof(Pixel));


    // Loops through region of interest in original image, and copies each
    // pixel to the new image
    for (int i = 0; i < img2->rows; i++) {
        for (int j = 0; j < img2->cols; j++) {

            int k1 = (i + upper_row) * img1->cols + j + upper_col;
            int k2 = i * img2->cols + j;

	        // Modifies data
            img2->data[k2] = img1->data[k1];

        }
    }

    // Writes cropped image
    int result = write_ppm(new_image, img2);
    free_image(&img2);

    return result;
}



int zoom_in(Image * img1, FILE * new_image) {
    Image * img2 = malloc(sizeof(Image));

    // Gets dimensions and space for new image
    img2->rows = 2 * img1->rows;
    img2->cols = 2 * img1->cols;
    img2->data = malloc(img2->rows * img2->cols * sizeof(int));

    // Maps each coordinate pair to new locations in enlarged image
    // For example, Coordinate i, j maps to the points (2i, 2j), (2i + 1, 2j),
    // (2i, 2j + 1), (2i + 1, 2j + 1).
    for (int i = 0; i < img1->rows; i++) {
        for (int j = 0; j < img1->cols; j++) {
            int k = i*img1->cols + j;

            int k1 = (2*i)*img2->cols + (2 * j);
            int k2 = k1 + img2->cols;

	    // Modify data: create block of pixels in correct location
            img2->data[k1] = img1->data[k];
            img2->data[k1 + 1] = img1->data[k];
            img2->data[k2] = img1->data[k];
            img2->data[k2 + 1] = img1->data[k];
        }
    }

    // Writes zoomed image
    int result = write_ppm(new_image, img2);
    free_image(&img2);

    return result;
}



int rotate_left(Image * img1, FILE * new_image) {
    Image * img2 = malloc(sizeof(Image));

    // New image will have reverse dimension of the original
    img2->rows = img1->cols;
    img2->cols = img1->rows;
    img2->data = malloc(img2->rows * img2->cols * sizeof(int));

    // Pixel at (i, j) maps to (c - j - 1, i) in new image, where c is the
    // number of columns in the original image
    for (int i = 0; i < img1->rows; i++) {
        for (int j = 0; j < img1->cols; j++) {
            int k1 = i*img1->cols + j;
            int k2 = (img1->cols - 1 - j)*img2->cols + i;

            img2->data[k2] = img1->data[k1];
        }
    }

    // Writes rotated image
    int result = write_ppm(new_image, img2);
    free_image(&img2);

    return result;
}


int pointillism(Image * img1, FILE * new_image) {
    Image * img2 = malloc(sizeof(Image));

    // Setting up space for new image
    img2->rows = img1->rows;
    img2->cols = img1->cols;
    img2->data = malloc(img2->rows * img2->cols * sizeof(int));

    // Loops through image for first time, and copies all contents to second image
    for (int i = 0; i < img1->rows; i++) {
        for (int j = 0; j < img1->cols; j++) {
            int k = i*img1->cols + j;
            img2->data[k] = img1->data[k];
        }
    }

    // Loops through copied image, and applies pointillism to randomly selected
    // pixels.
    for (int i = 0; i < img1->rows; i++) {
        for (int j = 0; j < img1->cols; j++) {
            int k1 = i*img1->cols + j;
            int pointillism_val = rand() % 100 + 1;

	        // Applies effect only to random group of pixels
            if (pointillism_val <= 3){

                int radius = rand() % 5 + 1;
                for (int m = i - radius; m <= i + radius; m++) {
                    for (int n = j - radius; n <= j + radius; n++) {

                        // Loops through all possible neighbors, to see which lie
                        // in the circle.
                        // If so, edits to match "center" pixel.
                        if (m >= 0 && n >= 0 && m < img2->rows &&
                            n < img2->cols) {

                            if ((m - i)*(m - i) + (n - j)*(n - j) <=
                                radius*radius) {

                                int k2 = m*img1->cols + n;
                                img2->data[k2] = img2->data[k1];
                            }
                        }
                    }
                }
            }
        }

    }

    // Writes new image
    int result = write_ppm(new_image, img2);
    free_image(&img2);

    return result;
}



int blur(Image * img1, FILE * new_image, float sigma) {

    // Modify sigma so it's ready for use
    int n = 10*sigma;
    if (n%2 == 0) {
        n++;
    }

    double* gaussian = (double*) malloc(sizeof(double) * n * n);
    int gaussian_center = n/2;
    
    // Set up gaussian matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int dx = abs(gaussian_center - i);
            int dy = abs(gaussian_center - j);
            gaussian[(int)(i * n + j)] = (1.0 / (2.0 * PI * sq(sigma)))
                    * exp( -(sq(dx) + sq(dy)) / (2 * sq(sigma)));
        }
    }

    Image *img2 = make_copy(img1);
    int row_count = img1->rows;
    int col_count = img1->cols;

    // Loops over input image
    for (int i = 0; i < row_count; i++) {
        for (int j = 0; j < col_count; j++) {
            double sum = 0;
            double avg_r = 0;
            double avg_g = 0;
            double avg_b = 0;

            // Iterates through gaussian matrix
            for (int m = 0; m < n; m++) {
                for (int l = 0; l < n; l++) {
                    int curr_row = i - gaussian_center + m;
                    int curr_col = j - gaussian_center + l;

                    // Checks if values are in range of img
                    if (curr_row >= 0 && curr_row < row_count &&  curr_col >= 0
                        && curr_col < col_count) {

                        // Gets gaussian value and multiply rgb value by it
                        sum += gaussian[m*n + l];
                        avg_r += img1->data[curr_row*col_count+ curr_col].r*
                                            gaussian[m*n+l];
                        avg_g += img1->data[curr_row*col_count+ curr_col].g*
                                            gaussian[m*n+l];
                        avg_b += img1->data[curr_row*col_count+ curr_col].b*
                                            gaussian[m*n+l];
                   }
                }
            }

	        // Normalizes values
            Pixel p;
            p.r = avg_r/sum;
            p.g = avg_g/sum;
            p.b = avg_b/sum;

	        // Sets data
            img2->data[i * col_count + j] = p;

        }
    }

    // Writes blurred image
    int result = write_ppm(new_image, img2);
    free(gaussian);
    free_image(&img2);

    return result;


}

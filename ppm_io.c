#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "ppm_io.h"



/* helper function for read_ppm, takes a filehandle
 * and reads a number, but detects and skips comment lines
 */
int read_num(FILE *fp) {
    assert(fp);

    int ch;
    // # marks a comment line 
    while((ch = fgetc(fp)) == '#') { 
      // Discard characters til end of line
        while( ((ch = fgetc(fp)) != '\n') && ch != EOF ) {
        }
    }
    // Put back the last thing we found                                                                                                                                         
    ungetc(ch, fp);

    int val;
    // Try to get an int 
    if (fscanf(fp, "%d", &val) == 1) { 
        // Drop trailing whitespace 
        while(isspace(ch = fgetc(fp))) {
        }
        ungetc(ch, fp);
	// We got a value, so return it 
        return val; 
    } else {
        fprintf(stderr, "Error:ppm_io - failed to read number from file\n");
        return -1;
    }
}




Image * read_ppm(FILE *fp) {

    /* Confirm that we received a good file handle */
    assert(fp != NULL);

    /* Allocate image (but not space to hold pixels -- yet) */
    Image *im = malloc(sizeof(Image));
    if (!im) {
        fprintf(stderr, "Error:ppm_io - failed to allocate memory for image!\n");
        return NULL;
    }

    /* Initialize fields to error codes, in case we have to bail out early */
    im->rows = im->cols = -1;

    /* Read in tag; fail if not P6 */
    char tag[20];
    tag[19]='\0';
    fscanf(fp, "%19s\n", tag);
    if (strncmp(tag, "P6", 20)) {
        fprintf(stderr, "Error:ppm_io - not a PPM (bad tag)\n");
        free(im);
        return NULL;
    }


    /* Read image dimensions */

    // Read in columns
    im->cols = read_num(fp); // NOTE: cols, then rows (i.e. X size followed by Y size)
    // Read in rows
    im->rows = read_num(fp);

    // Read in colors; fail if not 255
    int colors = read_num(fp);
    if (colors != 255) {
        fprintf(stderr, "Error:ppm_io - PPM file with colors different from 255\n");
        free(im);
        return NULL;
    }

    // Confirm that dimensions are positive
    if (im->cols <= 0 || im->rows <= 0) {
        fprintf(stderr, "Error:ppm_io - PPM file with non-positive dimensions\n");
        free(im);
        return NULL;
    }

    /* Finally, read in Pixels */

    /* Allocate the right amount of space for the Pixels */
    im->data = malloc(sizeof(Pixel) * (im->rows) * (im->cols));

    if (!im->data) {
        fprintf(stderr, "Error:ppm_io - failed to allocate memory for image pixels!\n");
        free(im);
        return NULL;
    }

    /* Read in the binary Pixel data */
    if (fread(im->data, sizeof(Pixel), (im->rows) * (im->cols), fp) !=
        (size_t)((im->rows) * (im->cols))) {
        fprintf(stderr, "Error:ppm_io - failed to read data from file!\n");
        free(im);
        return NULL;
    }

    // Return the image struct pointer
    return im;
}



int write_ppm(FILE *fp, const Image *im) {
    int num_rows = im->rows;
    int num_cols = im->cols;

    // Add necesary file info to top of file
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", num_cols, num_rows);
    fprintf(fp, "255\n");

    // Write image to disk as PPM
    return fwrite(im->data, sizeof(Pixel), num_rows*num_cols, fp);

}



void free_image(Image **im) {
    free((*im)->data);
    free(*im);
    im = NULL;
}



Image * make_image (int rows, int cols) {
    
    // Allocate space
    Image *im = malloc(sizeof(Image));
    if (!im) {
        return NULL;
    }
    // Set size
    im->rows = rows;
    im->cols = cols;

    // Allocate pixel array
    im->data = malloc((im->rows * im->cols) * sizeof(Pixel));
    if (!im->data) {
        free(im);
        return NULL;
    }

    return im;
}



Image* make_copy (Image *orig) {

    // Allocate space
    Image *copy = make_image(orig->rows, orig->cols);

    // If we got space, copy pixel values
    if (copy) {
        memcpy(copy->data, orig->data, (copy->rows * copy->cols) * sizeof(Pixel));
    }

    return copy;
}



void output_dims(Image *im) {
    printf("cols = %d, rows = %d", im->cols, im->rows);
}



int resize_image(Image **im, int rows, int cols) {

    // Create new dimensions and space for new image
    (*im)->rows = rows;
    (*im)->cols = cols;
    (*im)->data = realloc((*im)->data, sizeof(Pixel) * rows * cols);

    // Throw error if invalid data
    if ((*im)->data == NULL) {
        return -1;
    }

    return 0;
}

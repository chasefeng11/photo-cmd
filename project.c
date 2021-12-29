#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ppm_io.h"
#include "image_manip.h"


// Return (exit) codes
#define RC_SUCCESS            0
#define RC_MISSING_FILENAME   1
#define RC_OPEN_FAILED        2
#define RC_INVALID_PPM        3
#define RC_INVALID_OPERATION  4
#define RC_INVALID_OP_ARGS    5
#define RC_OP_ARGS_RANGE_ERR  6
#define RC_WRITE_FAILED       7
#define RC_UNSPECIFIED_ERR    8



void print_usage();

int is_integer(char * str);

int is_float(char * str);

void free_files(FILE * file1, FILE * file2);



int main (int argc, char* argv[]) {

    // Less than 2 command line args means that input or output filename
    // wasn't specified
    if (argc < 3) {
        fprintf(stderr, "Missing input/output filenames\n");
        print_usage();
        return RC_MISSING_FILENAME;
    }

    FILE * fp1;
    FILE * fp2;

    // Opens source and destination files, and returns error code if fails.
    fp1 = fopen(argv[1], "rb");
    if (!fp1){
        fprintf(stderr, "Unable to read\n");
        return RC_OPEN_FAILED;
    }

    fp2 = fopen(argv[2], "wb");
    if (!fp2){
        fprintf(stderr, "Unable to write\n");
        free_files(fp1, fp2);;
        return RC_OPEN_FAILED;
    }


    // Tries to make an image object from the input file, and returns error code
    // if fails
    Image * old_img = read_ppm(fp1);

    if (old_img == NULL){
        fprintf(stderr, "Input file cannot be read as a ppm\n");
        free_files(fp1, fp2);
        free_image(&old_img);
        return RC_INVALID_PPM;
    }


    // Checks to see if operation argument, and returns error code if not
    char * operation;

    if (argc < 4){
        fprintf(stderr, "Missing operation name\n");
        print_usage();
        free_files(fp1, fp2);
        free_image(&old_img);
        return RC_INVALID_OPERATION;
    } else {
        operation = argv[3];
    }

    // Calls binarize
    if (strcmp(operation, "binarize") == 0){
        if (argc != 5){
            fprintf(stderr, "Incorrect number of arguments passed\n");
            print_usage();
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_INVALID_OP_ARGS;
        }

        // Checks binarize parameter is an integer
        if (!is_integer(argv[4]) && atof(argv[4]) == 0){
            fprintf(stderr, "Invalid argument for operation\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_OP_ARGS_RANGE_ERR;
        }

        int threshold = atoi(argv[4]);
        int binarize_output = binarize(old_img, fp2, threshold);

        // Prints possible errors for binarize
        switch (binarize_output){

        case -1:
            fprintf(stderr, "Invalid argument for operation\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_OP_ARGS_RANGE_ERR;
            break;

        case 0:
            fprintf(stderr, "Could not write\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_WRITE_FAILED;
            break;
        }
    }

    // Calls crop
    else if (strcmp(operation, "crop") == 0){
        if (argc != 8){
            fprintf(stderr, "Incorrect number of arguments passed\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            print_usage();
            return RC_INVALID_OP_ARGS;
        }

        if (!(is_integer(argv[4]) || is_integer(argv[5]) || is_integer(argv[6])
                                                || is_integer(argv[7]))){
            fprintf(stderr, "Invalid argument for operation\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_OP_ARGS_RANGE_ERR;
        }

        int upper_col = atoi(argv[4]);
        int upper_row = atoi(argv[5]);
        int lower_col = atoi(argv[6]);
        int lower_row = atoi(argv[7]);

        int crop_output = crop(old_img, fp2, upper_col, upper_row, lower_col,
                               lower_row);

        switch (crop_output){

        case -1:
            fprintf(stderr, "Invalid arguments for operation\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_OP_ARGS_RANGE_ERR;

        case 0:
            fprintf(stderr, "Could not write\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_WRITE_FAILED;
        }
    }


    // Calls zoom_in
    else if (strcmp(operation, "zoom_in") == 0){
        if (argc != 4){
            fprintf(stderr, "Incorrect number of arguments passed\n");
            print_usage();
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_INVALID_OP_ARGS;
        }

        if (zoom_in(old_img, fp2) == 0){
            fprintf(stderr, "Could not write\n");
            free_image(&old_img);
            fclose(fp1);
            fclose(fp2);
            return RC_WRITE_FAILED;
        }
    }


    // Calls rotate-left
    else if (strcmp(operation, "rotate-left") == 0){
        if (argc != 4){
            fprintf(stderr, "Incorrect number of arguments passed\n");
            print_usage();
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_INVALID_OP_ARGS;
        }

        if (rotate_left(old_img, fp2) == 0){
            fprintf(stderr, "Could not write\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_WRITE_FAILED;
        }
    }


    // Calls pointilism
    else if (strcmp(operation, "pointilism") == 0){
        if (argc != 4){
            fprintf(stderr, "Incorrect number of arguments passed\n");
            print_usage();
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_INVALID_OP_ARGS;
        }

        if (pointilism(old_img, fp2) == 0){
            fprintf(stderr, "Could not write\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_WRITE_FAILED;
        }
    }


    // Calls blur
    else if (strcmp(operation, "blur") == 0){
        if (argc != 5){
            fprintf(stderr, "Incorrect number of arguments passed\n");
            print_usage();
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_INVALID_OP_ARGS;
        }

        if (!is_float(argv[4])){
            fprintf(stderr, "Invalid argument for operation\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_OP_ARGS_RANGE_ERR;
        }

        float sigma = atof(argv[4]);

        int blur_output = blur(old_img, fp2, sigma);

        // Prints possible errors for binarize
        switch (blur_output){

        case -1:
            fprintf(stderr, "Invalid argument for operation\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_OP_ARGS_RANGE_ERR;

        case 0:
            fprintf(stderr, "Could not write\n");
            free_files(fp1, fp2);
            free_image(&old_img);
            return RC_WRITE_FAILED;
        }
    }


    // Returns error if operation keyword is invalid
    else {
        fprintf(stderr, "Operation not recognized\n");
        print_usage();
        free_files(fp1, fp2);
        free_image(&old_img);
        return RC_INVALID_OPERATION;
    }


    free_files(fp1, fp2);
    free_image(&old_img);

    return RC_SUCCESS;
}


void print_usage() {
    printf("USAGE: ./project <input-image> <output-image> <command-name> <command-args>\n");
    printf("SUPPORTED COMMANDS:\n");
    printf("   binarize <treshhold>\n");
    printf("   crop <top-lt-col> <top-lt-row> <bot-rt-col> <bot-rt-row>\n");
    printf("   zoom_in\n");
    printf("   rotate-left\n");
    printf("   pointilism\n");
    printf("   blur <sigma>\n");
}


// Returns 0 if there is non-integer character in provided string
int is_integer(char * str){
    for (int i = 0; i < (int) strlen(str); i++){
        if (!isdigit(str[i])){
            return 0;
        }
    }

    return 1;
}


// Returns 0 if there is a non-float character in the provided string
// Allows up to 1 decimal point
int is_float(char * str){
    int decimal = 0;

    for (int i = 0; i < (int) strlen(str); i++){
        if (!isdigit(str[i])){
            if (str[i] == '.'){
                if (decimal == 1){
                    return 0;
                } else {
                    decimal++;
                }
            } else {
                return 0;
            }
        }
    }

    return 1;
}


// Frees the two inputted FILE objects
void free_files(FILE * file1, FILE * file2){
    fclose(file1);
    fclose(file2);
}


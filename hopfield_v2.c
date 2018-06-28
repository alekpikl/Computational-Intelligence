//
//  hoppflied.c
//  CI
//
//  Created by Alek  on 22/01/2018.
//  Copyright © 2018 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// ALL SAVED AS VECTORS
// '*' = -1 , '.' = 1
typedef char** IMAGE_HOLDER;
typedef char* IMAGE_VECTOR;
typedef char PIXEL;
typedef int** IMAGE_HOLDER_INT;
typedef int* IMAGE_VECTOR_INT;
typedef int PIXEL_INT; // +1 or -1
#define HEIGHT 10
#define WIDTH 20
#define MAX_INPUT 20

int weights[HEIGHT*WIDTH][HEIGHT*WIDTH] = {0};

static IMAGE_HOLDER _image_holder = {NULL};
static IMAGE_HOLDER _image_holder_distorted = {NULL};
static IMAGE_HOLDER_INT _image_holder_int = {NULL};
static IMAGE_HOLDER_INT _image_holder_int_distorted = {NULL};
int nrOfImages = 0;
int nrOfExamples = 0;

IMAGE_VECTOR createVector(int rows, int columns) {
    IMAGE_VECTOR line = (IMAGE_VECTOR)calloc(rows*columns, sizeof(PIXEL));
    return line;
}

IMAGE_VECTOR_INT createVector_INT(int rows, int columns) {
    IMAGE_VECTOR_INT line = (IMAGE_VECTOR_INT)calloc(rows*columns, sizeof(PIXEL_INT));
    return line;
}

void readImages(void) {
    _image_holder = (IMAGE_HOLDER)calloc(MAX_INPUT, sizeof(IMAGE_VECTOR));
    //printf("Read Images\n");
    for (int i = 0; i<MAX_INPUT; ++i) {
        _image_holder[i] = createVector(HEIGHT*WIDTH, 1);
    }
    char peak[21];
    int pixel_counter = 0;
    while (scanf(" %s", peak) != EOF) {
        //printf("%s\n", peak);
        if ((peak[0] == '-') && (peak[1] == '-') && (peak[2] == '-')) {
            pixel_counter = 0; ++nrOfImages;
            break; // END OF TRAINING DATA
        } else if (peak[0] == '-') {
            ++nrOfImages; pixel_counter = 0;
        } else {
            for (int pixel = 0; pixel<WIDTH; ++pixel) {
                _image_holder[nrOfImages][pixel_counter] = peak[pixel];
                ++pixel_counter;
            }
        }
    }
}

void change2int(void) {
    _image_holder_int = (IMAGE_HOLDER_INT)calloc(MAX_INPUT, sizeof(IMAGE_VECTOR_INT));
    for (int i = 0; i<nrOfImages; ++i) {
        _image_holder_int[i] = createVector_INT(HEIGHT*WIDTH, 1);
    }
    for (int imageIndex = 0; imageIndex < nrOfImages; ++imageIndex) {
        for (int line = 0; line < HEIGHT; ++line) {
            for (int pixel = 0; pixel < WIDTH; ++pixel) {
                if (_image_holder[imageIndex][WIDTH*line+pixel] == '*') {
                    _image_holder_int[imageIndex][WIDTH*line+pixel] = -1;
                } else if(_image_holder[imageIndex][WIDTH*line+pixel] == '.') {
                    _image_holder_int[imageIndex][WIDTH*line+pixel] = 1;
                }
            }
        }
    }
}

void weightInit(void) {
    int tempStart = 0;
    for (int i = tempStart; i < HEIGHT*WIDTH; ++i) {
        for (int j = tempStart+1; j < HEIGHT*WIDTH; ++j) {
            for (int pattern = 0; pattern < nrOfImages; ++pattern) {
                //printf("i: %d, j: %d, pattern: %d\n", i,j,pattern);
                weights[i][j] += _image_holder_int[pattern][i]*_image_holder_int[pattern][j];
            }
            weights[j][i] = weights[i][j];
            //printf("i: %d, j: %d\n", i,j);
        }
        ++tempStart;
    }
}

void printImages_INT(void) {
    for (int imageIndex = 0; imageIndex < nrOfImages; ++imageIndex) {
        for (int line = 0; line < HEIGHT; ++line) {
            for (int pixel = 0; pixel < WIDTH; ++pixel) {
                printf("%d", _image_holder_int[imageIndex][WIDTH*line+pixel]);
            }
            printf("\n");
        }
        if (imageIndex == nrOfImages-1) {
            printf("---\n");
        } else {
            printf("-\n");
        }
    }
}

void printImages(void) {
    for (int imageIndex = 0; imageIndex < nrOfImages; ++imageIndex) {
        for (int line = 0; line < HEIGHT; ++line) {
            for (int pixel = 0; pixel < WIDTH; ++pixel) {
                printf("%c", _image_holder[imageIndex][WIDTH*line+pixel]);
            }
            printf("\n");
        }
        if (imageIndex == nrOfImages-1) {
            printf("---\n");
        } else {
            printf("-\n");
        }
    }
}

void read_examples() {
    _image_holder_distorted = (IMAGE_HOLDER)calloc(MAX_INPUT, sizeof(IMAGE_VECTOR));
    //printf("Read Images\n");
    for (int i = 0; i<MAX_INPUT; ++i) {
        _image_holder_distorted[i] = createVector(HEIGHT*WIDTH, 1);
    }
    char peak[22];
    int pixel_counter = 0;
    while (scanf(" %s", peak) != EOF) {
        //printf("%s\n", peak);
        if (peak[0] == '-') {
            ++nrOfExamples; pixel_counter = 0;
        } else {
            for (int pixel = 0; pixel<WIDTH; ++pixel) {
                _image_holder_distorted[nrOfExamples][pixel_counter] = peak[pixel];
                ++pixel_counter;
            }
        }
    }
    ++nrOfExamples;
}

void change2int_distorted(void) {
    _image_holder_int_distorted = (IMAGE_HOLDER_INT)calloc(MAX_INPUT, sizeof(IMAGE_VECTOR_INT));
    for (int i = 0; i<nrOfExamples; ++i) {
        _image_holder_int_distorted[i] = createVector_INT(HEIGHT*WIDTH, 1);
    }
    for (int imageIndex = 0; imageIndex < nrOfExamples; ++imageIndex) {
        for (int line = 0; line < HEIGHT; ++line) {
            for (int pixel = 0; pixel < WIDTH; ++pixel) {
                if (_image_holder_distorted[imageIndex][WIDTH*line+pixel] == '*') {
                    _image_holder_int_distorted[imageIndex][WIDTH*line+pixel] = -1;
                } else if(_image_holder_distorted[imageIndex][WIDTH*line+pixel] == '.') {
                    _image_holder_int_distorted[imageIndex][WIDTH*line+pixel] = 1;
                }
            }
        }
    }
}

void findImage() {
    for (int example = 0; example < nrOfExamples; ++example) {
        IMAGE_VECTOR_INT output = createVector_INT(HEIGHT*WIDTH, 1);
        IMAGE_VECTOR output_char = createVector(HEIGHT*WIDTH, 1);
        for (int row = 0; row < HEIGHT*WIDTH; ++row) {
            for (int col = 0; col < HEIGHT*WIDTH; ++col) {
                output[row] += weights[row][col] * _image_holder_int_distorted[example][col];
            }
        }
        for (int pixel = 0; pixel < HEIGHT*WIDTH; ++pixel) {
            if (output[pixel] >= 0) {
                output_char[pixel] = '.';
            } else {
                output_char[pixel] = '*';
            }
        }
        // Check if it has converged to one training example. Otherwise introduce a little noise.
        int bestMatchIndex = 0;
        int smallestError = 201;
        for (int inputSample = 0; inputSample < nrOfImages; ++inputSample) {
            int tempErrorCounter = 0;
            for (int pixel = 0; pixel < HEIGHT*WIDTH; ++pixel) {
                if (output_char[pixel] != _image_holder[inputSample][pixel]) {
                    ++tempErrorCounter;
                    //printf("Not the same. out_char: %c, _image_hold: %c\n", output_char[pixel], _image_holder[inputSample][pixel]);
                }
            }
            if (tempErrorCounter < smallestError) {
                bestMatchIndex = inputSample;
                smallestError = tempErrorCounter;
            }
            //printf("bestMatch: %d, errorCounter: %d\n", bestMatchIndex, tempErrorCounter);
        }
        
        // End of check. Printf
        for (int line = 0; line < HEIGHT; ++line) {
            for (int pixel = 0; pixel < WIDTH; ++pixel) {
                printf("%c", _image_holder[bestMatchIndex][WIDTH*line+pixel]);
            }
            printf("\n");
        }
        /*
        for (int line = 0; line < HEIGHT; ++line) {
            for (int pixel = 0; pixel < WIDTH; ++pixel) {
                printf("%c", output_char[WIDTH*line+pixel]);
            }
            printf("\n");
        }
         */
        if ((example == nrOfExamples-1) || (nrOfExamples == 1)) {
            // do nothing
        } else {
            printf("-\n");
        }
        free(output);
        free(output_char);
    }
}

int main(int argc, char* argv[]) {
    readImages();
    change2int();
    //printImages_INT();
    //printf("Number of Images: %d\n", nrOfImages);
    //printImages();
    weightInit();
    read_examples();
    change2int_distorted();
    findImage();
    
    return 0;
}


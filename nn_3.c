//
//  nn_1.c
//  CI
//
//  Created by Alek  on 10/12/2017.
//  Copyright © 2017 private. All rights reserved.
//
/* Training the network to output 0 or 1 (NOT -1 or 1)
 * TODO: Gradient descent checking, regularization terms (weight decay), early stopping!!!
 * CREATE "FAKE" data -> the data is linear, multiply it with randomly */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define ACTIVATION_FUNCTION 0    // function: 0 - linear, 1 - logistic, 2 - tanh
#define LEARNING_RATE 0.01
#define EPOCHS 400

typedef float** MATRIX;
typedef float* VECTOR;
float max1 = 0.0, max2 = 0.0, min1 = 0.0, min2 = 0.0;

MATRIX createMatrix(int rows, int columns);
VECTOR createVector(int rows, int columns);

float f_prime(float value, int function); // function: 0 - linear, 1 - logistic, 2 - tanh
// DO NOT USE FOR THE INPUT LAYER!!!
float getDeltaForUnit(int i, int j, int unit, VECTOR deltaLayer); // i - current layer, j - posterior layer --> def [0,nr_layers-1] --> 0 means input layer
float sigmoid(float x);
void initializeWeights(void);
void getActivation(float input[]);
bool proofEquality(float x, float y);
void calcDelta(float groundTruth);
void updateWeights(float input);
void normalizeInput(MATRIX data, int numberOfExamples);

float weights[2] = {0.0, 0.0}; // two weights - x and bias
float activation = 0.0;
float delta = 0.0;

/* ********* NEURAL NETWORK ******** */
void recreate(float input[], float output[], int index) {
    if (ACTIVATION_FUNCTION == 0)
        output[index] = input[0] * weights[0] + weights[1];
    if (ACTIVATION_FUNCTION == 1)
        output[index] = sigmoid(input[0] * weights[0] + weights[1]);
    if (ACTIVATION_FUNCTION == 2)
        output[index] = tanhf(input[0] * weights[0] + weights[1]);
}

void train(MATRIX table, int numberOfExamples) {
    //initializeWeights();
    //printf("Weights: w0 = %8f, w1 = %8f, w2 = %8f\n", weights[0], weights[1], weights[2]);
    for (int i = 0; i < numberOfExamples; ++i) {
        getActivation(table[i]);
        //printf("Activation: %f\n", activation);
        calcDelta(table[i][1]);
        //printf("Delta: %f\n", delta);
        updateWeights(table[i][0]);
        //printf("Updated weights: w0 = %8f, w1 = %8f, w2 = %8f\n", weights[0], weights[1], weights[2]);
    }
}

void initializeWeights(void) {
    float order = 0.1;
    
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 2; ++i) {
        if ((float)rand()/(float)(RAND_MAX)>0.5) {
            weights[i] = (float)rand()/(float)(RAND_MAX/order);
        } else {
            weights[i] = -(float)rand()/(float)(RAND_MAX/order);
        }
    }
}

void getActivation(float input[]) {
    float netInput = input[0] * weights[0] + weights[1];
    if (ACTIVATION_FUNCTION == 0)
        activation = netInput;
    else if (ACTIVATION_FUNCTION == 1)
        activation = sigmoid(netInput);
    else if (ACTIVATION_FUNCTION == 2)
        activation = tanhf(netInput);
}
// TODO!!!
bool earlyStopping(float validationErrorLastTen[], MATRIX weightsSaved) {
    // check if the validation Error has gotten up -> stop and keep the best set of weights!!!
    float smallestValidationError = validationErrorLastTen[0];
    int indexOfSmallestError = 0;
    for (int i = 0; i < 10; ++i) {
        if (validationErrorLastTen[i] < smallestValidationError) {
            smallestValidationError = validationErrorLastTen[i];
            indexOfSmallestError = i;
        }
    }
    if (indexOfSmallestError != 9) {
        //printf("Previous Weights: w0 = %f, w1 = %f, w2 = %f\n", weights[0], weights[1], weights[2]);
        weights[0] = weightsSaved[indexOfSmallestError][0];
        weights[1] = weightsSaved[indexOfSmallestError][1];
        //printf("\nSmallest Validation Error: %f\n", validationErrorLastTen[indexOfSmallestError]);
        return true;
    }
    return false;
}
/* ********* MATH ******** */
// function: 0 - linear, 1 - logistic, 2 - tanh
// UNIT - [0,NUMBER_OF_UNITS-1], LAYER analog
float f_prime(float value, int function) {
    if (function == 0) { // 1
        return 1.0;
    } else if (function == 1){ // f(z) * (1 - f(z))
        return (value * (1 - value));
    } else if (function == 2) { // 1 - f(z)*f(z)
        return (1 - value * value);
    } else
        printf("SPECIFY A FUNCTION!"); return -1.0;
}

float errorFunction(float groundTruth[], int size) {
    float error = 0.0, tmp = 0.0;
    // for every output unit calculate the error and add them together at the end
    for (int i = 0; i < size; ++i) {
        tmp = (activation - groundTruth[i]);
        error += tmp * tmp;
    }
    error = error / (2 * size);
    
    // return the final error
    return error;
}

void calcDelta(float groundTruth) {
    delta = - (groundTruth - activation) * f_prime(activation, ACTIVATION_FUNCTION);
}

void updateWeights(float input) {
    weights[0] = weights[0] - LEARNING_RATE * delta * input;
    weights[1] = weights[1] - LEARNING_RATE * delta * 1;
}

float sigmoid(float x) {
    return (1/(1+expf(-x)));
}
/* ********* INPUT ******** */
float reNormAndScale(float activation) {
    return ((activation+1)/2 * (max2-min2) + min2);
}

void normalizeInput(MATRIX data, int numberOfExamples) {
    //find the max
    for (int i=0; i<numberOfExamples; ++i) {
        if (data[i][0] > max1)
            max1 = data[i][0];
        else if (data[i][0] < min1)
            min1 = data[i][0];
        if (data[i][1] > max2)
            max2 = data[i][1];
        else if (data[i][0] < min2)
            min2 = data[i][1];
    }
    for (int i=0; i<numberOfExamples; ++i) {
        data[i][0] = (data[i][0] - min1)/(max1 - min1) * (2) - 1;
        //data[i][1] = (data[i][1] - min2)/(max2 - min2) * (2) - 1;
    }
}
bool proofEquality(float x, float y) {
    float eps = 0.000001;
    if (fabs(x-y) <= eps)
        return true;
    return false;
}
bool endCondRead(float x[]) {
    float eps = 0.000001;
    if (fabs(x[1]-0) <= eps)
        if (fabs(x[0]-0) <= eps)
            return true;
    return false;
}
void readCSV(MATRIX table, int* index) {
    float buffer[2];
    char peak[50] = {'\0'};
    while (scanf(" %s", peak)) {
        sscanf(peak,"%f,%f", &buffer[0],&buffer[1]);
        if (endCondRead(buffer))
            break;
        //printf("Buffer: %s\n", peak);
        table[*index][0] = buffer[0];
        table[*index][1] = buffer[1];
        ++(*index);
    }
}
void printCSV(MATRIX table, int rows, int columns) {
    for (int i = 0; i <  rows; i++) {
        for (int j = 0; j < columns; j++)
            printf("%lf ", table[i][j]);
        printf("\n");
    }
}

MATRIX createMatrix(int rows, int columns) {
    MATRIX matrix = (MATRIX)calloc(rows, sizeof(VECTOR));
    matrix[0] = (VECTOR)calloc(columns*rows, sizeof(float));
    
    for (int i = 0; i<rows; i++)
        matrix[i] = (*matrix + columns*i);
    
    return matrix;
}

float successRate(MATRIX groundTruth, float output[], int numberOfExamples) {
    int sum = 0.0;
    for (int i = 0; i < numberOfExamples ; ++i) {
        //printf("GroundTruth: %f, output: %f\n", groundTruth[i][2], output[i]);
        if (proofEquality(groundTruth[i][2], output[i]))
            ++sum;
    }
    return (float)sum/(float)numberOfExamples;
}

int main(int argc, char** argv) {
    //printf("Hello Neural Networks!\n");
    MATRIX data = createMatrix(1000, 2);
    int numberOfExamples = 0;
    //float validationErrorCounter = 0;
    //float validationError[10] = {0.0};
    //MATRIX weightsSaved = createMatrix(10, 2);
    readCSV(data, &numberOfExamples);
    //printf("Number of training examples: %d\n", numberOfExamples);
    //printCSV(data, numberOfExamples, 2);
    normalizeInput(data, numberOfExamples); // NORMALIZATION FOR REGRESSION???
    //printf("\n");
    //printCSV(data, numberOfExamples, 2);
    // divide the input data 70%-30% (first 70% for training - TODO: make it random? )
    //int trainingExamples = (int)floorf(numberOfExamples * 0.7);
    //printf("Training examples: %d\n", trainingExamples);
    //int testingExamples = numberOfExamples - trainingExamples;
    //printf("Testing examples: %d\n", testingExamples);
    initializeWeights();
    //float output[testingExamples];
    for (int j = 0; j < EPOCHS; ++j) {
        //train(data, trainingExamples);
        train(data, numberOfExamples);
        
        //printf("Weights: w0 = %f, w1 = %f\n", weights[0], weights[1]);
        /*
         int index = 0;
         for (int i = trainingExamples; i < numberOfExamples; ++i) {
         recreate(data[i], output, index);
         ++index;
         } */
        /*
         // COMPUTE THE COST (ERROR) FUNCTION!!! VALIDATION ERROR
         validationError[validationErrorCounter] = errorFunction(output, testingExamples);
         //printf("Validation Error: %f\n", validationError[validationErrorCounter]);
         // TODO: SAVE THE WEIGHTS!!!
         weightsSaved[validationErrorCounter][0] = weights[0];
         weightsSaved[validationErrorCounter][1] = weights[1];
         // Update the index
         validationErrorCounter = (validationErrorCounter + 1) % 10;
         // KEEP TRACK -> when it starts to go up - stop!
         if (j>8) {
         if (earlyStopping(validationError, weightsSaved)) {
         break;
         }
         }
         */
    }
    // Recreation
    MATRIX in = createMatrix(1, 1);
    while (scanf(" %f", &in[0][0])>0) {
        in[0][0] = (in[0][0] - min1)/(max1 - min1) * (2) - 1;
        activation = in[0][0] * weights[0] + weights[1];
        //activation = reNormAndScale(activation);
        printf("%f\n", activation);//
    }
    
    //float successrate = successRate(data, output, numberOfExamples);
    //printf("Successrate: %f\n", successrate);
    //printf("Weights: w0 = %f, w1 = %f, w2 = %f\n", weights[0], weights[1], weights[2]);
    return 0;
}


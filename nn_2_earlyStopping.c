//
//  nn_1.c
//  CI
//
//  Created by Alek  on 26/11/2017.
//  Copyright © 2017 private. All rights reserved.
//

// TODO: UPDATE THE BIASES???

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define NUMBER_OF_LAYERS 7
#define LEARNING_RATE 0.001
#define EPOCHS 700
static const int networkStructure[NUMBER_OF_LAYERS] = {512,1024,2048,1024,512,256,3};

typedef float** MATRIX;
typedef float* VECTOR;
float max1 = 0.0, max2 = 0.0, min1 = 0.0, min2 = 0.0; // normalization values
float errorDistr = 100;

VECTOR errorDistributionOutput[1] = {NULL}; // store the error, how it is distributed over the output units
MATRIX createMatrix(int rows, int columns);
VECTOR createVector(int rows, int columns);
MATRIX thetaIndex[NUMBER_OF_LAYERS-1];
VECTOR activationIndex[NUMBER_OF_LAYERS] = {NULL}; // store the activations + the input values + output values(last activation)
VECTOR getActivations(VECTOR input, int l, int ACTIVATION_LAST_LAYER);
// define the number of units per structure -> first layer are the inputs, last layer the outputs
// UNIT - [0,NUMBER_OF_UNITS-1], LAYER analog
float f_prime(float value, int function); // function: 0 - linear, 1 - logistic, 2 - tanh
// DO NOT USE FOR THE INPUT LAYER!!!
float getDeltaForUnit(int i, int j, int unit, VECTOR deltaLayer); // i - current layer, j - posterior layer --> def [0,nr_layers-1] --> 0 means input layer

/* ********* NEURAL NETWORK ******** */
/* 1.) layers -> number of layers, number of neurons per layer
 2.) Theta (weight) functions as matrices. Dimensions -> s_j units in layer j, s_j+1 units in layer j+1, then
 theta_j (weights from layer j to layer j+1) will be of dim || s_j+1 x (s_j + 1) ||
 3.) Sigmoid function
 4.) Forward Propagation -> calculate the activations for each unit
 5.) Backward Error Propagation
 The Learning of the network outputs a correct set of weights -> it operates on the matrices.
 */
// order is a dec number with with the random values will be multiplicated
void initializeWeights(float order) {
    srand((unsigned int)time(NULL));
    for (int k = 0; k<NUMBER_OF_LAYERS-1; k++)
        for (int i = 0; i < networkStructure[k+1]; i++)
            for (int j = 0; j < networkStructure[k]+1; j++) {
                if ((float)rand()/(float)(RAND_MAX)>0.5) {
                    thetaIndex[k][i][j] = (float)rand()/(float)(RAND_MAX/order);
                } else {
                    thetaIndex[k][i][j] = -(float)rand()/(float)(RAND_MAX/order);
                }
            }
}
void initThetas(void) {
    // create the thetas, the +1 adds the bias unit
    for (int i = 0; i < NUMBER_OF_LAYERS-1; i++)
        thetaIndex[i] = createMatrix(networkStructure[i+1], networkStructure[i] + 1);
}
// initialize the activationIndex  to store all the activations w the bias -1
// [0] is the input
// also store the bias at the end and init it with -1
// TODO: test
void initActivations(float in[]) {
    // TODO: test if that works!!!
    if (activationIndex[0] == NULL) {
        for (int i = 0; i < NUMBER_OF_LAYERS; i++) {
            activationIndex[i] = createVector(1, networkStructure[i]+1);
            activationIndex[i][networkStructure[i]] = 1; // adds the BIAS unit at the end - CONST!!
        }
    }
    // copy the input to the input layer
    for (int i = 0; i < networkStructure[0]; i++)
        activationIndex[0][i] = in[i];
}
// TODO: finish and test
void forwardPropagation(float in[]) {
    // mind the different sizes of the layers!!!
    // arg in[] -> the input layer
    // the first layer is the input layer
    initActivations(in);
    // VECTOR activations = getActivations(in, 1, 1);
    // Go until the 1 before last layer -> last theta EVERY NEURON TANH!! CLASSIFICATION
    for (int i = 1; i<NUMBER_OF_LAYERS; i++)
        activationIndex[i] = getActivations(activationIndex[i-1], i+1, 2);
    /*
     // final layer has a linear activation
     if (NUMBER_OF_LAYERS > 1)
     activationIndex[NUMBER_OF_LAYERS-1] = getActivations(activationIndex[NUMBER_OF_LAYERS-2], NUMBER_OF_LAYERS-1, 0);
     else
     activationIndex[0] = getActivations(activationIndex[0], 1, 1);
     */
}
// TODO:
// delta[0] - first hidden layer, delta[NUMBER_OF_LAYERS-2] - output layer
void backwardPropagation(float groundTruth[]) {
    // create the delta index, size NUMBER_OF_LAYERS-1
    VECTOR delta[NUMBER_OF_LAYERS-1] = {NULL};
    // get the deltas - error signals
    for (int i = 1; i < NUMBER_OF_LAYERS; ++i) {
        delta[i-1] = createVector(1, networkStructure[i]);
    }
    int outputLayer = NUMBER_OF_LAYERS - 1;
    // get the output layer deltas
    int i = 0;
    // get the error signal for the output layer - ALSO TANH
    for (i = 0; i < networkStructure[outputLayer]; ++i)
        delta[outputLayer-1][i] = - (groundTruth[i] - activationIndex[outputLayer][i]) * f_prime(activationIndex[outputLayer][i], 2);
    // get the error signal for every layer - backwards
    for (int j = outputLayer-1; j > 0; --j)
        // for every unit in the layer j
        for (int k = 0; k < networkStructure[j]; ++k)
            delta[j-1][k] = getDeltaForUnit(j, j+1, k, delta[j]); // j for the function getDeltaForUnit needs to be 1 bigger than in the delta index
    
    // update the weights - w_ik from the unit k to the unit i
    for (int k = 0; k < outputLayer; ++k) { // thetaIndex
        for (int i = 0; i < networkStructure[k+1]; ++i) { // units in the next layer
            for (int j = 0; j < networkStructure[k]+1; ++j) { // units in current layer (+1 = the bias)
                thetaIndex[k][i][j] -= LEARNING_RATE * delta[k][i] * activationIndex[k][j]; // delta[0] is the first hidden layer
            }
        }
    }
    
    for (int i = 0; i < NUMBER_OF_LAYERS-1; ++i)
        free(delta[i]);
}
/* ********* MATH ******** */
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
            min2 = data[i][0];
    }
    for (int i=0; i<numberOfExamples; ++i) {
        data[i][0] = (data[i][0] - min1)/(max1 - min1) * (2) - 1;
        data[i][1] = (data[i][1] - min2)/(max2 - min2) * (2) - 1;
    }
}
// TODO: TEST!!!
float getDeltaForUnit(int i, int j, int unit, VECTOR deltaLayer) { // i - current layer, j - posterior layer --> def [0,nr_layers-1] --> 0 means input layer
    if (i == 0) {
        printf("ERROR - DO NOT CALC DELTA FOR THE INPUT UNIT"); return -1000.0;
    }
    float delta_I = 0.0, netIn = 0.0, f_pr = 0.0, sum = 0.0;
    // get the net input to unit i - net input is the weighted sum of the activations of the anterior layer
    for (int tmp = 0; tmp < (networkStructure[i-1]+1) ; ++tmp)
        netIn += thetaIndex[i-1][unit][tmp] * activationIndex[i-1][tmp];
    
    f_pr = f_prime(netIn, 2); // NOTE: CHOOSE THE ACTIVATION FUNCTION
    // multiply the errors of all the posterior layer units and do a weighted sum
    for (int tmp = 0; tmp < networkStructure[j]; ++tmp) {
        sum += (deltaLayer[tmp] * thetaIndex[i][tmp][unit]);
    }
    delta_I = f_pr * sum;
    return delta_I;
}
// function: 0 - linear, 1 - logistic, 2 - tanh
// UNIT - [0,NUMBER_OF_UNITS-1], LAYER analog
// TODO: test
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
// TODO: test!!!
void errorFunction(int outputLayer, float groundTruth[]) {
    float error = 0.0, tmp = 0.0;
    // for every output unit calculate the error and add them together at the end
    for (int i = 0; i < networkStructure[outputLayer-1]; i++) {
        tmp = (activationIndex[outputLayer-1][i] - groundTruth[i]);
        //errorDistributionOutput[1][i] = 0.5 * tmp * tmp;
        error += 0.5 * tmp * tmp;
    }
    // return the final error
    errorDistr = error;
}
// TODO: test the output!
float sigmoid(float x) {
    return (1/(1+expf(-x)));
}
// if doing the multiclass classification
// TODO: test the output!!!
void normalizedSumOutput(VECTOR activationLastUnit, int lastLayer) {
    float sumTemp = 0;
    for (int i = 0; i<networkStructure[lastLayer-1]; i++)
        sumTemp += exp(activationLastUnit[i]);
    for (int i = 0; i<networkStructure[lastLayer-1]; i++)
        activationLastUnit[i] = activationLastUnit[i] / sumTemp;
}

// param layer tells which theta to use -> 0,1,2,3,4... -> from the thetaIndex
// TODO:
void costFunction() {
    
}
// TODO:
void annealing() {
    
}
// TODO:
void sheaking() {
    
}
/* ********* MATRICES ******** */
VECTOR createVector(int rows, int columns) {
    VECTOR vector = (VECTOR)calloc(rows*columns, sizeof(float));
    return vector;
}
MATRIX createMatrix(int rows, int columns) {
    MATRIX matrix = (MATRIX)calloc(rows, sizeof(VECTOR));
    matrix[0] = (VECTOR)calloc(columns*rows, sizeof(float));
    
    for (int i = 0; i<rows; i++)
        matrix[i] = (*matrix + columns*i);
    
    return matrix;
}
// TODO: get it to work!!!
void freeMem(MATRIX matrix, int rows) {
    for (int i = 0; i<rows; i++)
        free(matrix[i]);
    free(matrix);
    matrix = NULL;
    printf("Matrix memory freed - pointer NULLed!");
}
// get the activations for the layer l
// columns are the weights for each unit
// input is a vector of activations of the units from the previous layer
// TODO: Test if the output is right!!!
VECTOR getActivations(VECTOR input, int l, int ACTIVATION_LAST_LAYER) {
    // include the bias!
    VECTOR activation = createVector(1, networkStructure[l-1]);
    MATRIX theta = thetaIndex[l-2]; // theta to calculate from (l-1) to (l)
    // for every unit in the Layer l
    for (int i = 0; i<networkStructure[l-1]; i++) {
        float tempSumm = 0.0;
        // for all the inputs to the unit, including the bias, hence +1
        for (int j = 0; j<networkStructure[l-2]+1; j++) {
            tempSumm += theta[i][j] * input[j];
        }
        // compute the out(net) -> if the last layer, use linear, otherwise sigmoidal
        if (ACTIVATION_LAST_LAYER == 2)
            activation[i] = tanhf(tempSumm);
    }
    return activation;
}
/* ********* INPUT ******** */
bool endCondRead(float x[3]) {
    float eps = 0.000001;
    if (fabs(x[2]-0) <= eps)
        return true;
    return false;
}
void readCSV(MATRIX table, int* index) {
    float buffer[3];
    char peak[50] = {'\0'};
    while (scanf(" %s", peak)) {
        sscanf(peak, "%f,%f,%f", &buffer[0],&buffer[1],&buffer[2]);
        table[*index][0] = buffer[0];
        table[*index][1] = buffer[1];
        table[*index][2] = buffer[2];
        if (endCondRead(buffer)) {
            //++(*index);
            break;
        }
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


int main(int argc, char* argv[]) {
    //printf("Hello Neural Networks!\n");
    MATRIX input = createMatrix(1000, 3);
    int numberOfExamples = 0;
    readCSV(input, &numberOfExamples);
    //printf("Number of Examples: %d\n", numberOfExamples);
    normalizeInput(input, numberOfExamples);
    initThetas();
    initializeWeights(0.01);
    // train
    /*
     for (int j = 0; j < EPOCHS; ++j) {
     forwardPropagation(input[j%numberOfExamples]);
     float groundTruth[1] = {input[j%numberOfExamples][2]};
     backwardPropagation(groundTruth);
     }
     */
    int j = 0, epochsCounter = 0;
    int trueEpochs = 0;
    while ((errorDistr > 0.1) && (trueEpochs < EPOCHS)) {
        forwardPropagation(input[j%numberOfExamples]);
        float groundTruth[1] = {input[j%numberOfExamples][2]};
        errorFunction(NUMBER_OF_LAYERS, groundTruth);
        backwardPropagation(groundTruth);
        ++j;
        ++epochsCounter;
        trueEpochs = epochsCounter/numberOfExamples;
        //printf("Number of Epochs: %d\nError = %f\n", trueEpochs, errorDistr);
    }
    trueEpochs = epochsCounter/numberOfExamples;
    //printf("A surprising finish without SegFaults\nNumber of Epochs: %d\n", trueEpochs);
    // recreate
    float recreate[2] = {0,0};
    while (scanf("%f,%f", &recreate[0], &recreate[1])>0) {
        forwardPropagation(recreate);
        if (activationIndex[NUMBER_OF_LAYERS-1][0] >= 0.0)
            printf("+1\n");
        else
            printf("-1\n");
    }
    return 0;
}






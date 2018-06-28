//
//  nn_4.c
//  CI
//
//  Created by Alek  on 26/11/2017.
//  Copyright © 2017 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define NUMBER_OF_LAYERS 6
#define WEIGHT_INIT 0.01
#define EPOCHS 200000
double LEARNING_RATE=0.001;
double LEARNING_RATE_0 = 0.001;
static const int networkStructure[NUMBER_OF_LAYERS] = {1,2,4,4,4,1};
double weightInitOrder[NUMBER_OF_LAYERS-1] = {1.0,300.0,250.0,400.0,500.0};
//double weightInitOrder[NUMBER_OF_LAYERS-1] = {1.0,10.0,100.0,100.0,50.0};

typedef double** MATRIX;
typedef double* VECTOR;
double max1 = 0.0, max2 = 0.0, min1 = 0.0, min2 = 0.0; // normalization values

MATRIX createMatrix(int rows, int columns);
VECTOR createVector(int rows, int columns);
void printActivations(void);
MATRIX thetaIndex[NUMBER_OF_LAYERS-1];
MATRIX bestTheta[NUMBER_OF_LAYERS-1];
VECTOR activationIndex[NUMBER_OF_LAYERS] = {NULL}; // store the activations + the input values + output values(last activation)
// define the number of units per structure -> first layer are the inputs, last layer the outputs
// UNIT - [0,NUMBER_OF_UNITS-1], LAYER analog
double f_prime(double value, int function); // function: 0 - linear, 1 - logistic, 2 - tanh
// DO NOT USE FOR THE INPUT LAYER!!!
double getDeltaForUnit(int i, int j, int unit, VECTOR deltaLayer); // i - current layer, j - posterior layer --> def [0,nr_layers-1] --> 0 means input layer
void forwardPropagation(double in[], bool useBestTheta);
void getActivations(int l, int ACTIVATION_LAST_LAYER, bool useBestTheta);

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
void initializeWeights(double order) {
    srand((unsigned int)time(NULL));
    for (int k = 0; k<NUMBER_OF_LAYERS-1; ++k)
        for (int i = 0; i < networkStructure[k+1]; ++i)
            for (int j = 0; j < networkStructure[k]+1; ++j) {
                if ((double)rand()/(double)(RAND_MAX)>0.5) {
                    thetaIndex[k][i][j] = weightInitOrder[k]*(double)rand()/(double)(RAND_MAX/order);
                } else {
                    thetaIndex[k][i][j] = -weightInitOrder[k]*(double)rand()/(double)(RAND_MAX/order);
                }
            }
}
void initThetas(MATRIX thetaIndex[]) {
    // create the thetas, the +1 adds the bias unit
    for (int i = 0; i < NUMBER_OF_LAYERS-1; i++)
        thetaIndex[i] = createMatrix(networkStructure[i+1], networkStructure[i] + 1);
}
// initialize the activationIndex  to store all the activations w the bias -1
// [0] is the input
// also store the bias at the end and init it with -1
// TODO: test
void initActivations(double in[]) {
    // TODO: test if that works!!!
    if (activationIndex[0] == NULL) {
        for (int i = 0; i < NUMBER_OF_LAYERS; ++i) {
            activationIndex[i] = createVector(1, networkStructure[i]+1);
            activationIndex[i][networkStructure[i]] = 1.0; // adds the BIAS unit at the end - CONST!!
        }
    } else {
        for (int i = 0; i < NUMBER_OF_LAYERS; ++i) {
            int j = 0;
            for ( j = 0; j < networkStructure[i]; ++j)
                activationIndex[i][j] = 0.0;
            activationIndex[i][j] = 1.0;
        }
    }
    // copy the input to the input layer
    int index = 0;
    for (index = 0; index < networkStructure[0]; ++index)
        activationIndex[0][index] = in[index];
    activationIndex[0][index] = 1.0;
}
// TODO: finish and test
void forwardPropagation(double in[], bool useBestTheta) {
    // mind the different sizes of the layers!!!
    // arg in[] -> the input layer
    // the first layer is the input layer
    initActivations(in);
    // VECTOR activations = getActivations(in, 1, 1);
    // Go until the 1 before last layer -> last theta EVERY NEURON TANH!! CLASSIFICATION
    int i = 1;
    for (i = 1; i<NUMBER_OF_LAYERS-1; i++) {
        getActivations(i+1, 2, useBestTheta);
    }
    getActivations(i+1, 0, useBestTheta); // === OUTPUT LINEAR
    //printf("Input: %lf\n", in[0]);
    //printActivations();
}
// TODO:
// delta[0] - first hidden layer, delta[NUMBER_OF_LAYERS-2] - output layer
void backwardPropagation(double groundTruth[]) {
    // create the delta index, size NUMBER_OF_LAYERS-1
    VECTOR delta[NUMBER_OF_LAYERS-1] = {NULL};
    // get the deltas - error signals
    for (int i = 1; i < NUMBER_OF_LAYERS; ++i) {
        delta[i-1] = createVector(1, networkStructure[i]);
    }
    int outputLayer = NUMBER_OF_LAYERS - 1;
    // get the output layer deltas
    // get the error signal for the output layer - ALSO TANH
    for (int i = 0; i < networkStructure[outputLayer]; ++i) { // ===== NUMERICALLY UNSTABLE!!!
        double error = 2*(groundTruth[i] - activationIndex[outputLayer][i]); //printf("Error: %lf\n", error); // MULTIPLY BY 2 ??
        delta[outputLayer-1][i] = (error) * f_prime(activationIndex[outputLayer][i], 0);
        //printf("Delta[%d-1][%d]: %lf \n", outputLayer, i, delta[outputLayer-1][i]);
    }
    // get the error signal for every layer - backwards
    for (int j = outputLayer-1; j > 0; --j) {
        // for every unit in the layer j
        for (int k = 0; k < networkStructure[j]; ++k) {
            delta[j-1][k] = getDeltaForUnit(j, j+1, k, delta[j]); // j for the function getDeltaForUnit needs to be 1 bigger than in the delta index
            //printf("Delta[%d-1][%d]: %lf ", j, k, delta[j-1][k]);
        }
        //printf("\n");
    }
    // (2,3,0-3,delta[2])
    //printf("\n");
    // update the weights - w_ik from the unit k to the unit i
    for (int k = 0; k < outputLayer; ++k) { // thetaIndex
        for (int i = 0; i < networkStructure[k+1]; ++i) { // units in the next layer
            for (int j = 0; j < networkStructure[k]+1; ++j) { // units in current layer (+1 = the bias)
                // ===== changed to += from -= (also changed the minus in getDeltaForUnit)
                // ===== COMPARE WITH THE EXCERCISE!!!
                thetaIndex[k][i][j] += LEARNING_RATE * delta[k][i] * activationIndex[k][j]; // delta[0] is the first hidden layer
                //printf("Update: Theta[%d][%d][%d] -= %lf * delta[%d][%d] (%lf) * activationIndex[%d][%d] (%lf)\n", k,i,j,LEARNING_RATE, k,i,delta[k][i], k,j, activationIndex[k][j]);
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
        else if (data[i][1] < min2)
            min2 = data[i][1];
    }
    for (int i=0; i<numberOfExamples; ++i) {
        data[i][0] = (data[i][0] - min1)/(max1 - min1) - 0.5;
        data[i][1] = (data[i][1] - min2)/(max2 - min2) - 0.5;
    }
}
// BIG VALUES!!! WATCH OUT!!!
// TODO: watch a youtube video on how to calculate the deltas
//// (2,3,0-3,delta[2])
double getDeltaForUnit(int i, int j, int unit, VECTOR deltaLayer) { // i - current layer, j - posterior layer --> def [0,nr_layers-1] --> 0 means input layer
    if (i == 0) {
        printf("ERROR - DO NOT CALC DELTA FOR THE INPUT UNIT"); return -1000.0;
    }
    double delta_I = 0.0, netIn = 0.0, f_pr = 0.0, sum = 0.0;
    // get the net input to unit i - net input is the weighted sum of the activations of the anterior layer
    /* ==== not the sum, just 1!!
     for (int tmp = 0; tmp < (networkStructure[i-1]+1) ; ++tmp)
     netIn += thetaIndex[i-1][unit][tmp] * activationIndex[i-1][tmp];
     */
    // multiply the errors of all the posterior layer units and do a weighted sum
    // ===== printf if it is correct!!!
    netIn = activationIndex[i][unit];
    f_pr = f_prime(netIn, 2); // NOTE: CHOOSE THE ACTIVATION FUNCTION
    for (int tmp = 0; tmp < networkStructure[j]; ++tmp) {
        sum += (deltaLayer[tmp] * thetaIndex[i][tmp][unit] * f_pr); //
    }
    delta_I = sum;
    //printf("delta_I: %lf, f_Prime: %lf, Net In: %lf, sum: %lf\n", delta_I, f_pr, netIn, sum);
    return delta_I;
}
// function: 0 - linear, 1 - logistic, 2 - tanh
// UNIT - [0,NUMBER_OF_UNITS-1], LAYER analog
// TODO: test
double f_prime(double value, int function) {
    if (function == 0) { // 1
        return 1.0;
    } else if (function == 1){ // f(z) * (1 - f(z))
        return (value * (1 - value));
    } else if (function == 2) { // 1 - f(z)*f(z)
        double ret = 1 - value * value;
        return ret;
    } else if (function == 3) { // RELU
        if (value < 0)
            return -0.01;
        else
            return 0.5;
    } else
        printf("SPECIFY A FUNCTION!"); return -1.0;
}
// TODO: test!!!
double errorFunction(double groundTruth) {
    double error = 0.0, tmp = 0.0;
    // for every output unit calculate the error and add them together at the end
    tmp = activationIndex[NUMBER_OF_LAYERS-1][0] - groundTruth;
    //printf("Tmp: %lf\n",tmp);
    error = 2 * tmp * tmp;
    // return the final error
    return error;
}
// TODO: test the output!
double sigmoid(double x) {
    return (1/(1+expf(-x)));
}
// if doing the multiclass classification
// TODO: test the output!!!
void normalizedSumOutput(VECTOR activationLastUnit, int lastLayer) {
    double sumTemp = 0;
    for (int i = 0; i<networkStructure[lastLayer-1]; i++)
        sumTemp += exp(activationLastUnit[i]);
    for (int i = 0; i<networkStructure[lastLayer-1]; i++)
        activationLastUnit[i] = activationLastUnit[i] / sumTemp;
}

// TODO:
void annealing(int t, int T) {
    LEARNING_RATE = LEARNING_RATE_0 / (1 + t/T);
}

// TODO:
void shaking() {
    double order = WEIGHT_INIT;
    srand((unsigned int)time(NULL));
    for (int k = 0; k<NUMBER_OF_LAYERS-1; k++)
        for (int i = 0; i < networkStructure[k+1]; i++)
            for (int j = 0; j < networkStructure[k]+1; j++) {
                
                if ((double)rand()/(double)(RAND_MAX)>0.5) {
                    thetaIndex[k][i][j] += (double)rand()/(double)(RAND_MAX/order);
                } else {
                    thetaIndex[k][i][j] += -(double)rand()/(double)(RAND_MAX/order);
                }
            }
}
/* ********* MATRICES ******** */
VECTOR createVector(int rows, int columns) {
    VECTOR vector = (VECTOR)calloc(rows*columns, sizeof(double));
    return vector;
}
MATRIX createMatrix(int rows, int columns) {
    MATRIX matrix = (MATRIX)calloc(rows, sizeof(VECTOR));
    matrix[0] = (VECTOR)calloc(columns*rows, sizeof(double));
    
    for (int i = 0; i<rows; i++)
        matrix[i] = (*matrix + columns*i);
    
    return matrix;
}
void freeMem(MATRIX matrix, int rows) {
    for (int i = 0; i<rows; i++)
        free(matrix[i]);
    free(matrix);
    matrix = NULL;
    printf("Matrix memory freed - pointer NULLed!");
}
// get the activations for the layer l
void getActivations(int l, int ACTIVATION_LAST_LAYER, bool useBestTheta) {
    MATRIX theta = NULL;
    if (useBestTheta)
        theta = bestTheta[l-2];
    else
        theta = thetaIndex[l-2]; // theta to calculate from (l-1) to (l)
    // for every unit in the Layer l
    int i = 0;
    for (i = 0; i<networkStructure[l-1]; i++) {
        double tempSumm = 0.0;
        // for all the inputs to the unit, including the bias, hence +1
        for (int j = 0; j<networkStructure[l-2]+1; j++) {
            tempSumm += (theta[i][j] * activationIndex[l-2][j]);
        }
        // compute the out(net) -> if the last layer, use linear, otherwise sigmoidal
        if (ACTIVATION_LAST_LAYER == 0)
            activationIndex[l-1][i] = tempSumm;
        else if (ACTIVATION_LAST_LAYER == 2)
            activationIndex[l-1][i] = (double)tanh(tempSumm);
        else if (ACTIVATION_LAST_LAYER == 3) {
            if (tempSumm < 0)
                activationIndex[l-1][i] = -0.01*tempSumm;
            else
                activationIndex[l-1][i] = 0.5*tempSumm;
        }
    }
    activationIndex[l-1][i] = 1.0;
}
/* ********* INPUT ******** */
bool endCondRead(double x[]) {
    float eps = 0.000001;
    if (fabs(x[1]-0) <= eps)
        if (fabs(x[0]-0) <= eps)
            return true;
    return false;
}
void readCSV(MATRIX table, int* index) {
    double buffer[2];
    char peak[50] = {'\0'};
    while (scanf(" %s", peak)) {
        sscanf(peak,"%lf,%lf", &buffer[0],&buffer[1]);
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

void printActivations() {
    for (int i = 0; i < NUMBER_OF_LAYERS; ++i) {
        for (int j = 0; j < networkStructure[i]+1; ++j)
            printf("%.16lf ", activationIndex[i][j]);
        printf("\n");
    }
}
// copies the bestWeights and saves them
void copyThetaIndex() {
    for (int k = 0; k < NUMBER_OF_LAYERS-1; ++k) { // thetaIndex
        for (int i = 0; i < networkStructure[k+1]; ++i) { // units in the next layer
            for (int j = 0; j < networkStructure[k]+1; ++j) { // units in current layer (+1 = the bias)
                bestTheta[k][i][j] = thetaIndex[k][i][j];
            }
        }
    }
}
void swap (double **a, double **b) {
    double *temp = *a;
    *a = *b;
    *b = temp;
}

void randomize ( MATRIX arr, int n ) {
    // Use a different seed value so that we don't get same
    // result each time we run this program
    srand ((unsigned int)time(NULL));
    
    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (int i = n-1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        int j = rand() % (i+1);
        
        // Swap arr[i] with the element at random index
        swap(&arr[i], &arr[j]);
    }
}

int main(int argc, char* argv[]) {
    time_t begin = time(NULL), end = time(NULL);
    
    MATRIX input = createMatrix(1000, 2);
    int numberOfExamples = 0;
    readCSV(input, &numberOfExamples);
    //printCSV(input, numberOfExamples, 2);
    //printf("Number of Examples: %d\n\n\n", numberOfExamples);
    normalizeInput(input, numberOfExamples);
    randomize(input, numberOfExamples);
    
    initThetas(thetaIndex);
    initializeWeights(WEIGHT_INIT);
    
    //printCSV(input, numberOfExamples, 2);
    /*
    for (int i = 0; i<NUMBER_OF_LAYERS-1; ++i) {
     printCSV(thetaIndex[i], networkStructure[i+1], networkStructure[i]+1);
     printf("\n");
    }
    */
    
    // train
    initThetas(bestTheta); // for saving the best theta

    //int smallestErrorIndex = 0;
    //double smallestError = 10000.0;;
    int epochs = 0;
    int trainingExamples = (int)( 1 * numberOfExamples);
    double error = 1000.0;
    double prevErr = 1000.0;
    //printf("Training Examples: %d\n", trainingExamples);
    while ((error > 0.00001)  && (epochs < EPOCHS) && ((end - begin)<280)) {
        for (int i = 0; i < trainingExamples; ++i) {
            forwardPropagation(input[i], 0);
            double groundTruth[1] = {input[i][1]};
            //groundTruth[0] = (groundTruth[0] - min1)/(max1 - min1) - 0.5;
            backwardPropagation(groundTruth);
        }
        if (epochs % 500 == 0) {
        //double error = 0.0;
        for (int i = 0; i < numberOfExamples; ++i) {
            forwardPropagation(input[i], 0);
            error += errorFunction(input[i][1]);
        }
        error = error / (double)(numberOfExamples);
        printf("Error = %lf\n", error);
        }
        /*
        if ((epochs%5) == 0) {
            //validate
            double error = 0.0;
            for (int i = trainingExamples; i < numberOfExamples; ++i) {
                forwardPropagation(input[i], 0);
                error += errorFunction(input[i][1]);
            }
            error = error / (double)(numberOfExamples-trainingExamples);
            if (error < smallestError) {
                copyThetaIndex();
                smallestError = error;
                smallestErrorIndex = epochs;
            }
            //printf("Epoch: %d, error: %lf\n", epochs, error);
            shaking();
        }
        */
        //annealing(epochs, 5000);
        
        
        if (epochs%200 == 0) {
            //double errorChange = prevErr-error;
            //if (prevErr-error <= 0.001) {
                //double seconds = (double)(end-begin);
                shaking(); //printf("Shaking: %.16lf\n", errorChange);
            //}
        }
        
        prevErr = error;
        end = time(NULL);
        epochs++;
    }
    
    //printf("Smallest error: %fl, at epoch: %d.\n", smallestError, smallestErrorIndex);
    /*
    for (int i = 0; i<NUMBER_OF_LAYERS-1; ++i) {
        printCSV(thetaIndex[i], networkStructure[i+1], networkStructure[i]+1);
        printf("\n");
    }
     */
    
    // recreate
    double example[1] = {0.0};
    while (scanf("%lf", example)>0) {
        //printf("Before Normalization: %lf. ", example[0]);
        example[0] = (example[0] - min1)/(max1 - min1) - 0.5;
        //printf("After Normalization: %lf. ", example[0]);
        forwardPropagation(example, 0);
        //data[i][1] = (data[i][1] - min2)/(max2 - min2) - 0.5;
        double renormalized = 0.0;
        renormalized = (activationIndex[NUMBER_OF_LAYERS-1][0] + 0.5)*(max2 - min2) + min2;
        //renormalized = (activationIndex[NUMBER_OF_LAYERS-1][0] + 0.5)*(max1 - min1) + min1;
        printf("%.16lf\n", renormalized);
    }
    return 0;
    
}






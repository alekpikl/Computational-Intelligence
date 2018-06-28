//
//  vq_1.c
//  CI
//
//  Created by Alek  on 03/01/2018.
//  Copyright © 2018 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define EPOCHS 20000
#define TIME 8
#define PI  3.14159265359
#define FILE_OUT
static double alpha = 0.5;
static double LEARNING_RATE = 0.01;
static double min1 = 0.0;
static double min2 = 0.0;
static double max1 = 0.0;
static double max2 = 0.0;

typedef double** MATRIX;
typedef double* VECTOR;

#ifdef FILE_OUT
    FILE *f = NULL; // write to file
#endif

MATRIX createMatrix(int rows, int columns);
VECTOR createVector(int rows, int columns);

double distance(VECTOR, MATRIX, int);
bool endCondRead(VECTOR);
void readCSV(MATRIX, int*);
void randomize (MATRIX,int);
void swap (double **a, double **b);
void findInputRange(MATRIX, int);

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

void findInputRange(MATRIX data, int numberOfExamples) {
    //find the max and min
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
}

void swap (double **a, double **b) {
    double *temp = *a;
    *a = *b;
    *b = temp;
}

void randomize ( MATRIX arr, int n ) {
    for (int i = n-1; i > 0; i--)
    {
        int j = rand() % (i+1);
        swap(&arr[i], &arr[j]);
    }
}

double distance(VECTOR input, MATRIX centers, int index) {
    double dist = 0.0;
    dist += pow((input[0] - centers[index][0]),2);
    dist += pow((input[1] - centers[index][1]),2);
    dist = sqrt(dist);
    return dist;
}

double distance_v2(VECTOR c1, VECTOR c2) {
    double dist = 0.0;
    //printf("input[0] = %lf", input[0]);
    //printf(", input[1] = %lf\n", input[1]);
    dist += pow((c1[0] - c2[0]),2);
    dist += pow((c1[1] - c2[1]),2);
    dist = sqrt(dist);
    return dist;
}

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
    while (scanf(" %s", peak) != EOF) {
        sscanf(peak,"%lf,%lf", &buffer[0],&buffer[1]);
        table[*index][0] = buffer[0];
        table[*index][1] = buffer[1];
        ++(*index);
    }
}

void initializePrototypes(MATRIX centers, int nrOfInputs, int nrOfClasses) {
    //double scaling_factor = (max1 + max2) / 2.0;
    for (int _class = 0; _class < nrOfClasses; ++_class) {
        centers[_class][0] = min1 + rand()%(int)((max1-min1));
        centers[_class][1] = min2 + rand()%(int)((max2-min2));
    }
}
/*
void initializePrototypes(MATRIX centers, MATRIX input, int nrOfInputs, int nrOfClasses) {
    centers[0] = input[(rand()%nrOfInputs)]; // first center
    //`printf("Center[0][0] = %lf, Center[0][1] = %lf, maxDistIndex = 0\n", centers[0][0],centers[0][1]);
    int nrOfProbes = nrOfInputs;
    //printf("Nr of probes: %d\n", nrOfProbes);
    MATRIX probes = createMatrix(nrOfProbes, 2);
    VECTOR probeDistance = NULL;
    MATRIX probeDistanceToAllCenters= NULL;
    bool appropriateCandidate = true;
    double averageDistance = 0.0;
    for (int j = 1; j < nrOfClasses; ++j) {
        int maxDistIndex = 0, indexBC = 0;
        probeDistance = createVector(nrOfProbes, 1);
        probeDistanceToAllCenters = createMatrix(nrOfProbes, nrOfClasses);
        int probeIndexEncoder[nrOfProbes];
        for (int i = 0; i < nrOfProbes; ++i) {
            appropriateCandidate = true;
            //int randIndex = rand()%nrOfInputs;
            int randIndex = i;
            probes[i] = input[randIndex];
            probeIndexEncoder[i] = randIndex;
            averageDistance = 0.0;
            //printf("Probe[%d][0] = %lf\n", i, probes[i][0]);
            for (int z = 0; z < j; ++z) {
                double distTmp = distance(probes[i], centers, z);
                probeDistanceToAllCenters[i][z] = distTmp;
                probeDistance[i] += distTmp;
                //printf("DistTmp = %lf\n", distTmp);
            }
            if (j == 1) {
                if ((probeDistance[i] > probeDistance[maxDistIndex]) && appropriateCandidate) {
                    maxDistIndex = i;
                }
                if (i > 10) {
                    break;
                }
            } else {
                averageDistance = probeDistance[i] / j;
                //double averageDiv = averageDistance/3;
                //printf("i=%d, Average Dist / 3 = %lf ", i,averageDiv);
                if (j > 0) {
                    for (int z = 0; z < j; ++z) {
                        //printf("probe dist to center %d = %lf, ", z, probeDistanceToAllCenters[i][z]);
                        if (probeDistanceToAllCenters[i][z] < averageDistance/3) {
                            appropriateCandidate = false;
                        }
                    }
                }
                
                //printf("appropriate? %d\n", appropriateCandidate);
                if (appropriateCandidate) {
                    if (probeDistance[i] > probeDistance[maxDistIndex]) {
                        ++indexBC; maxDistIndex = i;
                    }
                    if (indexBC == 3) {
                        break;
                    }
                    
                }
            }
            
            //printf("i = %d, probeDistance[%d] = %lf, maxDistIndex = %d\n", i, i,probeDistance[i], maxDistIndex);
        }
       
        centers[j] = input[probeIndexEncoder[maxDistIndex]]; // PROBE INDEX ENCODER!!!
        //printf("Center[%d][0] = %lf, Center[%d][1] = %lf, maxDistIndex = %d\n", j,centers[j][0], j,centers[j][1], maxDistIndex);
        free(probeDistance);
    }
}
*/
bool check_if_same_convergence(MATRIX centers, int nrOfCenters) {
    VECTOR smallestDistance = createVector(nrOfCenters, 2);
    int* smallestDistanceIdx = (int*)calloc(nrOfCenters, sizeof(int));
    for (int c1 = 0 ; c1 < nrOfCenters; ++c1) {
        if (c1 == 0) {
            smallestDistance[c1] = distance_v2(centers[c1], centers[1]);
            smallestDistanceIdx[c1] = 1;
        } else {
        smallestDistance[c1] = distance_v2(centers[c1], centers[0]);
            smallestDistanceIdx[c1] = 0;
        }
        for (int c2 = 0 ; c2 < nrOfCenters; ++c2){
            double temp = distance_v2(centers[c1], centers[c2]);
            if (temp < smallestDistance[c1] && temp > 0.00000) {
                smallestDistance[c1] = temp;
                smallestDistanceIdx[c1] = c2;
            }
        }
        printf("SmallestDistance of %d to %d = %lf\n", c1, smallestDistanceIdx[c1], smallestDistance[c1]);
    }
    return true;
}


int main(int argc, char *argv[]) {
    time_t begin = time(NULL), end = time(NULL);
    // read the inputs
    MATRIX input = createMatrix(1000,2);
    int nrOfInputs = 0, nrEpoch = 0, nrOfClasses = 0;
    scanf("%d\n", &nrOfClasses);
    readCSV(input, &nrOfInputs);
    
#ifdef FILE_OUT
    f = fopen("input_samples.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    for (int sample = 0; sample < nrOfInputs; ++sample) {
        fprintf(f, "%lf,%lf\n", input[sample][0], input[sample][1]);
    }
    fclose(f);
#endif
    
    //printf("Finished reading input, will start training!\n");
    // create the network
    srand((unsigned int)time(NULL));
    MATRIX centers = createMatrix(nrOfClasses, 2); //x,y coordinates
    findInputRange(input, nrOfInputs);
    //initializePrototypes(centers, input, nrOfInputs, nrOfClasses); // MAYBE ERROR!
    initializePrototypes(centers, nrOfInputs, nrOfClasses);
    /*
    for (int i = 0; i < nrOfClasses; ++i) {
        printf("%lf,%lf\n", centers[i][0], centers[i][1]);
    }
    printf("\n");
     */
    //VECTOR lateralDistance = createVector(nrOfClasses, 1);
    // train
    while ((nrEpoch < EPOCHS) && (end - begin)<TIME) {
        int inputIndex = 0; //indexMin = 0;
        randomize(input, nrOfInputs);
        while (inputIndex < nrOfInputs) {
            int indexMin = 0;
            double min=(centers[0][0]-input[inputIndex][0])*(centers[0][0]-input[inputIndex][0])+(centers[0][1]-input[inputIndex][1])*(centers[0][1]-input[inputIndex][1]);
            for (int j = 0; j<nrOfClasses; ++j) {
                if ((centers[j][0]-input[inputIndex][0])*(centers[j][0]-input[inputIndex][0]) + (centers[j][1]-input[inputIndex][1])*(centers[j][1]-input[inputIndex][1]) < min) {
                    min = (centers[j][0]-input[inputIndex][0])*(centers[j][0]-input[inputIndex][0]) + (centers[j][1]-input[inputIndex][1])*(centers[j][1]-input[inputIndex][1]);
                    indexMin = j;
                }
            }
            //printf("Index min: %d\n", indexMin);
            centers[indexMin][0] += LEARNING_RATE * (input[inputIndex][0] - centers[indexMin][0]);
            centers[indexMin][1] += LEARNING_RATE * (input[inputIndex][1] - centers[indexMin][1]);
            // update the winning center
            ++inputIndex;
        }
        
        if (LEARNING_RATE > 0.05) {
            double temp = exp(-2*(double)nrEpoch/(double)EPOCHS);
            LEARNING_RATE = alpha * temp;
        }
        //printf("Learning Rate: %lf\n", LEARNING_RATE);
        ++nrEpoch;
        end = time(NULL);
    }
    
    // print the centers
    for (int i = 0; i < nrOfClasses; ++i) {
        printf("%lf,%lf\n", centers[i][0], centers[i][1]);
    }
    check_if_same_convergence(centers, nrOfClasses);
#ifdef FILE_OUT
    f = fopen("vectors.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    for (int sample = 0; sample < nrOfClasses; ++sample) {
        fprintf(f, "%lf,%lf\n", centers[sample][0], centers[sample][1]);
    }
    fclose(f);
#endif
}
